# Tasks

## 1. 新增 USB 总线检测函数

- [x] 1.1 在 `main.c` 中新增 `is_usb_drive()` 函数，通过 `CreateFileW` + `DeviceIoControl` + `IOCTL_STORAGE_QUERY_PROPERTY` 查询 `STORAGE_DEVICE_DESCRIPTOR` 的 `BusType` 字段
- [x] 1.2 新增 `is_usb_bus_type()` 辅助函数，判断 `BusType` 是否为 `BusTypeUsb`

## 2. 改造驱动器检测逻辑

- [x] 2.1 修改 `detect_removable_drives()` 函数，对 `DRIVE_FIXED` 类型调用 `is_usb_drive()` 进行总线类型过滤
- [x] 2.2 `DRIVE_REMOVABLE` 类型保持原有逻辑不变

## 3. 编译验证

- [x] 3.1 使用 MinGW 编译，确保无警告无错误
- [x] 3.2 验证生成的 `backup.exe` 可正常启动
