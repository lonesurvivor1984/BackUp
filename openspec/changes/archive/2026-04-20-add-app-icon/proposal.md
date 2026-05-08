## Why

为备份工具添加应用图标，提升用户体验和程序辨识度。图标可以在文件资源管理器、任务栏和窗口标题栏显示，使程序更加专业。

## What Changes

- **新增图标文件**: `databak.ico` 作为应用程序图标
- **新增资源文件**: `resource.rc` 定义图标资源
- **修改编译流程**: 将图标资源编译并链接到可执行文件
- **无功能变化**: 纯 UI 增强，不影响程序逻辑

## Capabilities

### New Capabilities
- `app-icon`: 应用程序图标显示能力

### Modified Capabilities
- (无)

## Impact

- **构建系统**: 需要 `windres` 工具编译 `.rc` 文件
- **Makefile**: 添加资源编译步骤
- **build.bat**: 添加资源编译命令
- **无运行时影响**: 图标在编译时嵌入，无额外依赖
