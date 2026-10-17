// SPDX-License-Identifier: GPL-2.0-only
/* Re:Kernel netlink server and proc status endpoint. */

#include <linux/errno.h>
#include <linux/freezer.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/netlink.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <net/net_namespace.h>
#include <net/sock.h>

#include "rekernel.h"

enum rekernel_server_state {
	REKERNEL_SERVER_DOWN,
	REKERNEL_SERVER_STARTING,
	REKERNEL_SERVER_READY,
};

static DEFINE_MUTEX(rekernel_server_lock);
static enum rekernel_server_state rekernel_state = REKERNEL_SERVER_DOWN;
static struct sock *rekernel_netlink;
static struct proc_dir_entry *rekernel_dir;
static struct proc_dir_entry *rekernel_unit_entry;
static unsigned int rekernel_netlink_unit = REKERNEL_NETLINK_MIN;

static void rekernel_server_cleanup_locked(void)
{
	if (rekernel_unit_entry) {
		proc_remove(rekernel_unit_entry);
		rekernel_unit_entry = NULL;
	}
	if (rekernel_dir) {
		proc_remove(rekernel_dir);
		rekernel_dir = NULL;
	}
	if (rekernel_netlink) {
		netlink_kernel_release(rekernel_netlink);
		rekernel_netlink = NULL;
	}
	rekernel_state = REKERNEL_SERVER_DOWN;
}

static void rekernel_netlink_rcv(struct sk_buff *skb)
{
	/* Re:Kernel is a one-way event channel; userspace owns the protocol. */
	(void)skb;
}

static struct netlink_kernel_cfg rekernel_netlink_cfg = {
	.input = rekernel_netlink_rcv,
};

static int rekernel_unit_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%u\n", READ_ONCE(rekernel_netlink_unit));
	return 0;
}

static int rekernel_unit_open(struct inode *inode, struct file *file)
{
	return single_open(file, rekernel_unit_show, NULL);
}

static const struct proc_ops rekernel_unit_fops = {
	.proc_open = rekernel_unit_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int rekernel_create_netlink_locked(void)
{
	for (rekernel_netlink_unit = REKERNEL_NETLINK_MIN;
	     rekernel_netlink_unit < REKERNEL_NETLINK_MAX;
	     rekernel_netlink_unit++) {
		rekernel_netlink = netlink_kernel_create(&init_net,
				rekernel_netlink_unit, &rekernel_netlink_cfg);
		if (rekernel_netlink)
			return 0;
	}

	return -ENODEV;
}

int rekernel_server_start(void)
{
	char unit_name[sizeof("4294967295")];
	int ret;

	mutex_lock(&rekernel_server_lock);
	if (rekernel_state == REKERNEL_SERVER_READY) {
		ret = 0;
		goto out;
	}

	rekernel_state = REKERNEL_SERVER_STARTING;
	ret = rekernel_create_netlink_locked();
	if (ret)
		goto fail;

	rekernel_dir = proc_mkdir("rekernel", NULL);
	if (!rekernel_dir) {
		ret = -ENOMEM;
		goto fail;
	}

	scnprintf(unit_name, sizeof(unit_name), "%u", rekernel_netlink_unit);
	rekernel_unit_entry = proc_create(unit_name, 0444, rekernel_dir,
					   &rekernel_unit_fops);
	if (!rekernel_unit_entry) {
		ret = -ENOMEM;
		goto fail;
	}

	rekernel_state = REKERNEL_SERVER_READY;
	ret = 0;
	goto out;

fail:
	rekernel_server_cleanup_locked();
out:
	mutex_unlock(&rekernel_server_lock);
	return ret;
}

int rekernel_send_netlink(const char *message, size_t length)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int ret;

	if (!message || !length || length > REKERNEL_MAX_PAYLOAD)
		return -EMSGSIZE;

	mutex_lock(&rekernel_server_lock);
	if (rekernel_state != REKERNEL_SERVER_READY || !rekernel_netlink) {
		ret = -ENOTCONN;
		goto out;
	}

	skb = nlmsg_new(length, GFP_ATOMIC);
	if (!skb) {
		ret = -ENOMEM;
		goto out;
	}

	nlh = nlmsg_put(skb, 0, 0, rekernel_netlink_unit, length, 0);
	if (!nlh) {
		nlmsg_free(skb);
		ret = -EMSGSIZE;
		goto out;
	}

	memcpy(nlmsg_data(nlh), message, length);
	ret = netlink_unicast(rekernel_netlink, skb, REKERNEL_USER_PORT,
				      MSG_DONTWAIT);
out:
	mutex_unlock(&rekernel_server_lock);
	return ret;
}

bool rekernel_line_is_frozen(struct task_struct *task)
{
	if (!task)
		return false;

	return frozen(task->group_leader) || freezing(task->group_leader);
}