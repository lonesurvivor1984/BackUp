## Why

当前备份工具在传输完成或出错后立即退出程序，用户无法查看完整的备份报告，也无法在出错后重试失败的操作。这导致用户需要重新启动程序并重复所有配置步骤，体验不佳。

## What Changes

- 备份完成显示报告后，暂停程序并等待用户确认后再退出
- 传输过程中出错时，提供重试选项而非直接终止
- U 盘检测失败时，允许用户插入 U 盘后重试而非直接退出
- 增加"返回上一步"选项，允许用户重新选择源目录或目标驱动器

## Capabilities

### New Capabilities
- `post-backup-pause`: 备份报告后暂停并等待用户确认退出
- `error-retry-loop`: 关键步骤出错时提供重试/返回/退出的交互循环

### Modified Capabilities
- (无已有 spec 需要修改)

## Impact

- `backup/main.c`: `main()` 函数的主流程需要改造为循环式交互，多个步骤的错误处理分支需要增加重试逻辑
- `print_final_report()` 函数后增加确认等待逻辑
- `detect_removable_drives()` 和 `select_drive()` 调用处增加重试循环
