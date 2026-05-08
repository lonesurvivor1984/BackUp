## Why

当前备份工具只检测 `DRIVE_REMOVABLE` 类型的驱动器（U盘），但移动硬盘通过 USB 连接后，Windows 通常将其识别为 `DRIVE_FIXED`（固定磁盘）。用户插入移动硬盘后无法被程序发现，限制了备份目标的选择。

## What Changes

- 驱动器检测从只识别 U 盘扩展为识别所有 USB 接口连接的存储设备（U盘 + USB移动硬盘）
- 使用 `DeviceIoControl` 查询 `STORAGE_DEVICE_DESCRIPTOR` 的 `BusTypeUsb` 属性来区分 USB 设备和本地硬盘
- 用户界面提示文案从"U 盘"改为更通用的"可移动存储设备"

## Capabilities

### New Capabilities
- `usb-drive-detection`: 通过总线类型检测 USB 连接的存储设备（U盘 + USB移动硬盘）

### Modified Capabilities
- （无已有 spec 需要修改）

## Impact

- `backup/main.c`: `detect_removable_drives()` 函数需要增加 USB 总线检测逻辑；部分提示文案需要更新
