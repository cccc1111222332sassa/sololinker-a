# Sololinker-A 资料合集

Hinlink Sololinker-A（RV1106）开发板资料汇总。

## 目录结构

| 目录 | 内容 |
| --- | --- |
| `01图纸` | 核心板 V2 原理图、位号图 |
| `02结构` | 3D 结构文件（STL、结构文件 zip 已解压） |
| `03使用指南` | 芯球介绍、引脚定义、排线上网、sd 启动、WiFi 使用指南 |
| `05SDK包` | Sololinker SDK、YOLOv5 demo |
| `06配件规格书` | 屏幕、摄像头、4G 模块、WIFI 开发资料等 |
| `08固件Alpine-内部发测` | Alpine / Ubuntu 固件（eMMC、NAND 等） |
| `RK芯片驱动+烧录+调试工具` | RK 驱动、烧录与调试工具（已解压） |
| `SoloLinker-A底板开发板` | 底板 BOM、位号图、丝印图 |
| `Sololinker-A芯片说明书` | RV1106 Datasheet/TRM、GC2093、WB800D80U、aic8800 移植包 |
| `群友驱动补丁固件` | 群友提供的驱动补丁、固件、示例、设备树等 |

## 本仓库的处理说明

1. **未包含演示视频**：原资料中的 4 个 mp4 视频未上传。
2. **中小压缩包已解压**：文档/工具/驱动/示例类压缩包（zip/rar/7z/tar.xz）已解压为普通文件夹，方便直接浏览。
3. **超大包改为多分卷**：以下 4 个包因超过 GitHub 单文件 100MB 上限，被切分为多个 **<100MB** 的分卷（7-Zip 存储模式，不改变内容）：

   | 位置 | 分卷数量 | 合计 | 还原得到的文件 |
   | --- | --- | --- | --- |
   | `05SDK包/` | 11 | 974.3 MB | `sololinker-sdk-20231219.7z` |
   | `08固件Alpine-内部发测/` | 3 | 252.8 MB | `RV1106_V1_EMMC_WIFI_UBUNTU.zip` |
   | `08固件Alpine-内部发测/` | 3 | 251.8 MB | `RV1106_V2_EMMC_WIFI_UBUNTU.zip` |
   | `群友驱动补丁固件/` | 3 | 187.1 MB | `非官方ubuntu-emmc-(GT911,RTL8821CU).7z` |

   > 分卷文件名形如 `xxx.7z.001`、`xxx.7z.002` …

### 如何还原分卷

把同一组全部分卷放到同一个目录下，用 7-Zip 打开 **`.001`** 这个文件并解压即可得到原始压缩包：

```bash
7z x sololinker-sdk-20231219.7z.7z.001        # 还原出 sololinker-sdk-20231219.7z
7z x RV1106_V1_EMMC_WIFI_UBUNTU.zip.7z.001    # 还原出 RV1106_V1_EMMC_WIFI_UBUNTU.zip
```

4. **保留为压缩包的固件**（体积本身 <100MB，未解压）：`EMMC-Alpine-WiFi.7z`、`NAND-128M-Alpine-WiFi.7z`、`RV1106_SOLOLINKER_EMMC8G_ALPINE_DOCKER.zip`、`lvgl_app_ubuntu.zip`、`image.rar`、`alpine_rootfs.zip`、`汣的update.img.zip`。

## 相关链接

- 上游 SDK 参考：`https://gitee.com/dataflood/rv1106_sdk`
- 主芯片：Rockchip RV1106（ARM Cortex-A7 + RISC-V，内置 NPU）
- WiFi/BT：AIC8800（USB 接口）
