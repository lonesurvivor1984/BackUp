## ADDED Requirements

### Requirement: Removable drive detection retry
U 盘检测失败时，程序必须提供重试选项，允许用户插入 U 盘后重新检测，而非直接退出。

#### Scenario: No removable drive detected initially
- **WHEN** 程序启动后未检测到任何可移动驱动器
- **THEN** 程序显示提示信息，并询问用户是否重新检测或退出

#### Scenario: User chooses to retry drive detection
- **WHEN** 用户选择重新检测
- **THEN** 程序重新调用驱动器检测逻辑，检测到后继续后续步骤

#### Scenario: User chooses to exit from drive detection
- **WHEN** 用户选择退出
- **THEN** 程序显示退出信息后暂停，等待用户按 Enter 键再退出

### Requirement: Drive selection retry
U 盘选择或时间戳文件夹创建失败时，必须允许用户重新选择或重试，而非直接退出。

#### Scenario: Invalid drive selection
- **WHEN** 用户输入无效的驱动器编号
- **THEN** 程序提示无效选择，并允许用户重新输入

#### Scenario: Timestamp folder creation fails
- **WHEN** 创建备份文件夹失败
- **THEN** 程序询问用户是否重试，重试时重新执行文件夹创建流程

### Requirement: Graceful exit on all paths
所有退出路径（成功、失败、取消）都必须先暂停等待用户确认，不得静默退出。

#### Scenario: Any exit path
- **WHEN** 程序即将退出（任何原因）
- **THEN** 程序先显示提示信息，并等待用户按 Enter 键后才执行 `return`
