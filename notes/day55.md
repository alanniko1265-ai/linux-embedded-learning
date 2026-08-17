# Day55 — VehicleSensorLab 启动与内核构建环境

日期：2026-08-17  
状态：环境准备基本完成，内核完整构建尚未完成；停在 DTC 兼容补丁已应用、等待重新增量构建的位置。

## 一、今天为什么先做环境

从 Day55 起，主线切换为：

```text
linux_projects/vehicle_sensor_lab
```

目标是从 Linux 应用开发逐步进入驱动/BSP：

```text
最小内核模块
  → /dev/vehicle_sensor
  → read
  → 阻塞与 poll
  → ioctl
  → 内核环形缓冲区
  → platform driver / 设备树
  → 板载按键与 LED
  → C++ 服务和 Qt/CLI 交互
```

外部内核模块不能只依靠普通应用交叉编译器。它还依赖运行内核对应的配置、生成头文件、符号CRC和Kbuild输出，因此今天先建立匹配的内核构建环境。

这部分命令反馈较弱，但只需解决一次。后续将开始出现 `/dev/vehicle_sensor`、事件读取、按键碰撞、LED告警和Qt控制等可直接交互的结果。

## 二、今天确认的开发板信息

开发板：野火 EBF6ULL S1 Pro  
SoC：NXP i.MX6ULL / ARMv7 Cortex-A7

```text
主机名：npi
登录用户：debian
内核 release：4.19.35-imx6
内核构建标识：#1.2508stable
内核构建编译器：Ubuntu/Linaro GCC 7.5.0
架构：armv7l
配置：SMP PREEMPT
```

模块相关配置：

```text
CONFIG_MODULES=y
CONFIG_MODULE_UNLOAD=y
CONFIG_MODVERSIONS=y
```

含义：

- 支持加载 `.ko`；
- 支持 `rmmod`；
- 会校验内核导出符号CRC，因此需要完整构建得到 `Module.symvers`。

板端目录：

```text
/lib/modules/4.19.35-imx6       存在
/lib/modules/4.19.35-imx6/build 不存在
```

结论：不在开发板原地编译模块；在WSL准备匹配内核并交叉编译，再通过SCP部署到开发板。

## 三、WSL环境和源码

WSL用户：`anllenge`

可用交叉编译器：

```text
/usr/bin/arm-linux-gnueabihf-gcc
  GCC 11.4.0

/home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
  GCC 8.3.0
```

后续优先使用GCC 8.3，因为它与板载内核使用的GCC 7.5年代更接近。

运行内核配置已复制到：

```text
/home/anllenge/work/imx6ull-kernel/npi-config.gz
```

验证结果：

```text
大小：约32 KiB
gzip -t退出状态：0
```

野火内核源码：

```text
目录：/home/anllenge/work/imx6ull-kernel/ebf_linux_kernel
分支：ebf_4.19.35_imx6ul
提交：927f64b95
构建输出：build_image/build
```

## 四、今天理解的构建参数

```text
ARCH=arm
```

指定目标是32位ARM，不是WSL的x86_64。

```text
CROSS_COMPILE=/.../arm-linux-gnueabihf-
```

这是工具链前缀，末尾减号必须保留。Kbuild会在后面拼接`gcc`、`ld`、`ar`等工具名。

```text
O=build_image/build
```

将生成文件放入独立输出目录，避免污染源码树。

```text
LOCALVERSION=-imx6
```

野火官方`make_deb.sh`通过它把基础版本`4.19.35`变为`4.19.35-imx6`。已经验证：

```text
kernelrelease = 4.19.35-imx6
```

与开发板`uname -r`完全一致。

## 五、今天遇到的问题

### 1. 命令拼写

| 错误 | 正确 | 原因 |
|---|---|---|
| `whoiam` | `whoami` | 命令拼写错误 |
| `ls *ld` | `ls -ld` | `*`是通配符，`-`才用于选项 |
| `~work/...` | `~/work/...` | `~`后需要 `/` 才表示当前用户主目录 |
| `imx6u11` | `imx6ull` | 两个小写字母`l`被写成数字`1` |
| `ARCH=aem` | `ARCH=arm` | Kbuild会寻找不存在的`arch/aem` |
| `ARCG=arm` | `ARCH=arm` | `ARCG`不是Kbuild识别的变量 |

### 2. 直接执行`make O=...`进入交互配置

没有指定目标时，`make`会执行默认的完整构建。由于刚复制的`.config`尚未同步，Kconfig进入交互提问。

正确顺序：

```text
复制.config
  → olddefconfig
  → kernelrelease
  → 完整构建
```

### 3. `kernelrelease`最初显示`4.19.35+`

原因是没有传入野火构建脚本使用的：

```text
LOCALVERSION=-imx6
```

加入后得到正确的`4.19.35-imx6`。

### 4. DTC链接失败

错误核心：

```text
multiple definition of `yylloc'
```

这里失败的是WSL主机工具DTC：

```text
HOSTCC/HOSTLD → /usr/bin/gcc 11.4 和 /usr/bin/ld
```

不是ARM交叉编译器。旧版Linux 4.19的DTC lexer和parser重复定义`yylloc`；新GCC默认`-fno-common`后不再合并重复定义。

采用Linux上游兼容修复，删除lexer中的冗余声明：

```diff
-YYLTYPE yylloc;
```

项目中保存了可追踪补丁：

```text
linux_projects/vehicle_sensor_lab/patches/0001-dtc-remove-redundant-yylloc.patch
```

补丁已经完成：

```text
git apply --check   通过
git apply           成功
git diff --check    通过
```

当前内核源码状态：

```text
 M scripts/dtc/dtc-lexer.l
```

## 六、最小模块骨架

已创建：

```text
linux_projects/vehicle_sensor_lab/driver/vehicle_sensor_module.c
linux_projects/vehicle_sensor_lab/driver/Makefile
```

模块生命周期：

```text
insmod
  → vehicle_sensor_init()

rmmod
  → vehicle_sensor_exit()
```

下一次需要完成两个`pr_info()`日志TODO和作者字段。目前还没有字符设备、设备树、定时器或真实硬件逻辑。

## 七、今天结束时的准确断点

已完成：

- 开发板内核、编译器和模块配置确认；
- 运行内核`.config`复制和完整性验证；
- 野火匹配内核分支下载；
- 独立构建目录创建；
- `olddefconfig`完成；
- `kernelrelease=4.19.35-imx6`验证；
- DTC构建错误定位；
- 上游兼容补丁应用和检查。

尚未完成：

- 补丁后的内核增量构建；
- `Module.symvers`生成；
- 最小模块编译；
- 上传、加载、卸载和10次循环测试。

## 八、下次从这里继续

WSL进入源码：

```bash
cd ~/work/imx6ull-kernel/ebf_linux_kernel
```

直接增量构建，不执行`clean`：

```bash
make O=build_image/build ARCH=arm LOCALVERSION=-imx6 CROSS_COMPILE=/home/anllenge/toolchains/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf- -j4
```

成功后检查：

```bash
echo $?
ls -lh build_image/build/Module.symvers
head -n 3 build_image/build/Module.symvers
```

完成环境关后，下一阶段开始产生真正可操作的结果：

```text
/dev/vehicle_sensor
  → read读到事件
  → 无数据时阻塞
  → poll等待唤醒
  → ioctl调整采样周期
  → 板载按键模拟碰撞
  → LED显示告警
  → Qt控制和观察
```

