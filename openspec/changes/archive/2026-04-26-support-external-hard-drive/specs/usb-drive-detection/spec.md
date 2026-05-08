## ADDED Requirements

### Requirement: USB bus type detection for fixed drives
对于 `DRIVE_FIXED` 类型的驱动器，必须通过 `DeviceIoControl` 查询总线类型，仅保留 `BusTypeUsb` 的设备，排除本地内置硬盘。

#### Scenario: Fixed drive is USB-connected
- **WHEN** 驱动器类型为 `DRIVE_FIXED` 且总线类型为 `BusTypeUsb`
- **THEN** 该驱动器被包含在候选列表中

#### Scenario: Fixed drive is internal (SATA/NVMe)
- **WHEN** 驱动器类型为 `DRIVE_FIXED` 且总线类型非 `BusTypeUsb`
- **THEN** 该驱动器被排除，不显示在候选列表中

#### Scenario: Bus type query fails
- **WHEN** 对某驱动器调用 `DeviceIoControl` 失败
- **THEN** 该驱动器被跳过，不影响其他驱动器的检测

### Requirement: Removable drive detection preserved
`DRIVE_REMOVABLE` 类型驱动器（U盘）必须继续保持检测，无需额外总线查询。

#### Scenario: USB flash drive detected
- **WHEN** 驱动器类型为 `DRIVE_REMOVABLE`
- **THEN** 该驱动器被直接包含在候选列表中

### Requirement: Unified candidate list
候选列表同时包含 `DRIVE_REMOVABLE` 设备和通过 USB 总线检测的 `DRIVE_FIXED` 设备。

#### Scenario: Both USB flash drive and external hard drive connected
- **WHEN** 系统中同时存在 U 盘和 USB 移动硬盘
- **THEN** 两者同时出现在候选列表中，用户可自由选择
