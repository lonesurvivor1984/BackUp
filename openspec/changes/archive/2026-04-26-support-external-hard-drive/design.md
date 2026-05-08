## Context

当前 `detect_removable_drives()` 只接受 `DRIVE_REMOVABLE` 类型驱动器。Windows 中 U 盘通常为 `DRIVE_REMOVABLE`，但 USB 移动硬盘多为 `DRIVE_FIXED`。需要通过总线类型而非 `GetDriveTypeW()` 来判断设备是否为 USB 连接。

## Goals / Non-Goals

**Goals:**
- 扩展检测范围，覆盖 USB 接口的 U 盘和移动硬盘
- 排除本地内置硬盘（SATA/NVMe），避免用户误选导致数据覆盖风险

**Non-Goals:**
- 不增加网络驱动器或虚拟磁盘支持
- 不改变备份核心逻辑

## Decisions

1. **USB 检测方法：`DeviceIoControl` + `IOCTL_STORAGE_QUERY_PROPERTY`**
   - 对 `DRIVE_FIXED` 类型的驱动器，用 `CreateFileW("\\\\.\\X:")` 获取句柄，调用 `DeviceIoControl` 查询 `STORAGE_DEVICE_DESCRIPTOR` 的 `BusType` 字段
   - `BusTypeUsb` (7) = USB 设备，予以保留；其他总线类型（SATA、NVMe 等）排除
   - 理由：这是 Windows 原生 API，无需第三方库，兼容性好

2. **备选方案被排除**
   - WMI 查询：需要 COM 初始化，代码量大，且速度慢
   - Setup API 枚举设备树：复杂度高，且与盘符映射困难

3. **保持向后兼容**
   - `DRIVE_REMOVABLE` 设备（U盘）直接保留检测，无需额外总线查询
   - 函数签名不变，`main()` 调用逻辑不变

## Risks / Trade-offs

- [`CreateFileW` 可能需要管理员权限] → 实际只需要读取属性的权限（`GENERIC_READ`），普通用户即可执行；如遇权限问题降级为跳过该盘
- [某些移动硬盘报告 `BusType` 为 `BusTypeUnknown`] → 保守策略：仅保留明确报告为 USB 的设备，避免误包含本地硬盘
- [需要包含 `<winioctl.h>`] → 已在 `<windows.h>` 中间接提供，但可能需要显式包含以获取 `STORAGE_BUS_TYPE` 定义
