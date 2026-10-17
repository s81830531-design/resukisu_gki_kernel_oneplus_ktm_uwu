# OnePlus Ace6 (KTM) uwuAOSP Kernel · ReSukiSU

用于 OnePlus Ace6（一加 Ace6，KTM）uwuAOSP 的 Linux 6.6.142 内核源码快照，集成 ReSukiSU v4.2.0-rc3。

## 上游来源与致谢

**本仓库的内核源码及设备树来自 [nullcode1337](https://github.com/nullcode1337) 的仓库。**

感谢 nullcode1337 的源码维护与设备适配工作。本仓库基于其成果进行本地 ReSukiSU 集成、配置调整及构建整理，不将上游源码和设备适配工作归为本仓库维护者原创。

当前保存的内核及设备树是未包含 Git 元数据的本地源码快照，因此暂时无法准确标注原始仓库地址、分支及提交版本；确认后会补充。配套模块源码也随工作目录收录，其具体上游来源仍待核实。

其他主要上游项目：

- [Linux](https://www.kernel.org/)
- [Android Common Kernel](https://android.googlesource.com/kernel/common/)
- [ReSukiSU / BakaSU](https://github.com/Baka-SU/BakaSU)

各文件中已有的作者署名、版权声明和许可证均予以保留。

## 版本与配置

| 项目 | 内容 |
|---|---|
| 设备 | OnePlus Ace6 / KTM |
| 使用系统 | uwuAOSP |
| 内核版本 | Linux 6.6.142 |
| 架构 | ARM64 |
| CONFIG_LOCALVERSION | `-4k` |
| Root 集成 | ReSukiSU v4.2.0-rc3 |
| ReSukiSU 提交 | `239e1e8871b8fcd51a6e5b3002e0ba522fdd99fb` |
| KernelSU Hook | Tracepoint |
| Re:Kernel | 支持 (Binder / Signal netlink 墓碑事件桥) |
| SUSFS | 当前配置未启用 |

## 本仓库整理内容

- 基于 nullcode1337 提供的内核及设备树源码进行本地集成。
- 集成上述固定版本的 ReSukiSU。
- 保存本地构建使用的配置、基线配置及配置差异。
- 将内核、配套模块及设备树按原工作目录结构整理收录。

本仓库未重建上游 Git 提交历史，而是从本地源码快照开始新的版本记录。首次导入提交不代表其中全部代码由本仓库维护者编写。

## 目录结构

```text
.
├── kernel/                       # 内核源码，来自 nullcode1337
├── sm8750-modules/                # 配套模块源码，具体来源待核实
├── sm8750-devicetrees/            # 设备树，来自 nullcode1337
├── ReSukiSU/                     # 固定版本的 ReSukiSU 源码
├── sm8750 -> kernel              # 兼容原目录布局的软链接
├── configs/
│   ├── ktm-resukisu.config        # 本地构建输出中的实际配置
│   └── ktm-baseline.config        # 基线配置
├── config-diff.txt               # 基线与候选配置差异
└── upstream-revision.txt         # ReSukiSU 版本记录

## 本地修改与来源边界

本仓库基于 nullcode1337 的内核及设备树，包含本地修改和 ReSukiSU 集成，并非上游原样镜像。

由于本地快照未保留内核及设备树的 Git 历史，当前无法完整列出相对上游的修改。后续确认原始仓库和基准提交后，将补充差异说明。

上游代码、设备适配及原有作者贡献仍归相应作者；本仓库维护者仅对自己的修改和整理工作负责。

## 构建状态

保存了原工作目录中实际使用的配置，但本次上传副本尚未独立重建验证。工具链、完整构建命令和打包步骤待核实后补充。

请保留同级目录结构，内核中的软链接依赖 ReSukiSU 和配套模块目录。

ReSukiSU 以固定版本源码收录，不包含其独立 Git 元数据，重新构建时可能需要处理基于 Git 的版本检测。

## 许可证与测试文件

保留各组件原有许可证、SPDX 标识和版权声明，各组件适用各自许可证。

SGX 自测目录中的 sign_key.pem 与本机 uwuAOSP、Avium 两份内核源码中的同名文件一致，随源码保留；本次尚未完成与公开上游版本的独立比对。
