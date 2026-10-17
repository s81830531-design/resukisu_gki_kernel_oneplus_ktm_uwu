/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _DRIVERS_REKERNEL_H
#define _DRIVERS_REKERNEL_H

#include <linux/types.h>

struct task_struct;

#define REKERNEL_NETLINK_MAX 26
#define REKERNEL_NETLINK_MIN 22
#define REKERNEL_USER_PORT 100
#define REKERNEL_PACKET_SIZE 128
#define REKERNEL_PROTOCOL_VERSION 1
#define REKERNEL_MAX_PAYLOAD (REKERNEL_PACKET_SIZE - 1)
#define REKERNEL_MIN_USERAPP_UID 10000
#define REKERNEL_MAX_SYSTEM_UID 2000
#define REKERNEL_WARN_AHEAD_SPACE (1U << 17)

int rekernel_server_start(void);
int rekernel_send_netlink(const char *message, size_t length);
bool rekernel_line_is_frozen(struct task_struct *task);

#endif /* _DRIVERS_REKERNEL_H */