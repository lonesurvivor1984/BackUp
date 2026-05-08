## ADDED Requirements

### Requirement: 应用程序图标显示
系统 SHALL 在 Windows 资源管理器、任务栏和窗口标题栏显示自定义应用图标。

#### Scenario: 资源管理器中显示图标
- **WHEN** 用户查看 backup.exe 文件
- **THEN** 显示 databak.ico 作为文件图标

#### Scenario: 任务栏显示图标
- **WHEN** 程序运行时
- **THEN** 任务栏按钮显示应用图标

#### Scenario: 窗口标题栏显示图标
- **WHEN** 控制台窗口打开时
- **THEN** 窗口左上角显示应用图标

#### Scenario: 图标资源嵌入
- **WHEN** 编译可执行文件
- **THEN** 图标数据嵌入到.exe 文件内部，无需外部文件
