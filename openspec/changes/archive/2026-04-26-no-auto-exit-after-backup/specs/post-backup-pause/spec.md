## ADDED Requirements

### Requirement: Post-backup confirmation pause
备份操作完成后（无论成功或部分失败），程序在显示最终报告后必须暂停，等待用户按 Enter 键才退出，不得自动退出。

#### Scenario: Backup completes successfully
- **WHEN** 所有文件拷贝成功
- **THEN** 程序显示"备份成功完成"报告，并提示"按 Enter 键退出"，等待用户输入后才退出

#### Scenario: Backup completes with errors
- **WHEN** 部分文件拷贝失败
- **THEN** 程序显示包含失败统计的报告，并提示"按 Enter 键退出"，等待用户输入后才退出

#### Scenario: User cancels before backup
- **WHEN** 用户在确认步骤选择取消备份
- **THEN** 程序显示"备份已取消"提示后，暂停等待用户按 Enter 键再退出
