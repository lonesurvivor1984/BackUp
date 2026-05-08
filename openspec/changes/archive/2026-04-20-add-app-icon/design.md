## Context

当前备份工具 `backup.exe` 没有应用图标，在 Windows 资源管理器中显示为默认齿轮图标，缺乏辨识度和专业感。用户已准备好图标文件 `databak.ico`。

**约束条件:**
- Windows 平台
- 使用 GCC/MinGW 编译
- 保持单文件可执行，无外部依赖

## Goals / Non-Goals

**Goals:**
- 将 `databak.ico` 嵌入到 `backup.exe` 中
- 图标在资源管理器、任务栏、窗口标题栏正确显示
- 编译流程自动化，无需手动操作

**Non-Goals:**
- 多分辨率图标支持（.ico 已包含所需尺寸）
- 运行时动态更换图标
- 跨平台图标支持

## Decisions

### 1. 图标资源格式
**Decision:** 使用标准 Windows `.rc` 资源文件

**Rationale:**
- GCC/MinGW 原生支持
- 与 `windres` 工具兼容
- 简单成熟的技术方案

### 2. 图标资源 ID
**Decision:** 使用 `IDI_APP_ICON` (整数值 101)

**Rationale:**
- 整数 ID 便于引用
- 101 是常见的应用图标起始 ID
- 可在代码中通过 `MAKEINTRESOURCE(101)` 引用

### 3. 编译集成
**Decision:** 在 Makefile 和 build.bat 中添加资源编译步骤

**流程:**
1. `windres resource.rc -o resource.o` 编译资源
2. `gcc ... resource.o -o backup.exe` 链接到可执行文件

**Rationale:**
- 一步编译，无需手动命令
- 保持现有构建习惯

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| windres 工具缺失 | 确保 MinGW 完整安装，bin 目录在 PATH 中 |
| .ico 文件格式错误 | 使用标准 Windows ICO 格式，包含 16x16 到 256x256 尺寸 |
| 图标不显示 | 检查资源 ID 正确，重新编译整个项目 |
| 文件体积增加 | 图标通常仅几 KB，影响可忽略 |

## Open Questions

- 是否需要为图标准备多个尺寸变体？(当前.ico 应已包含)
