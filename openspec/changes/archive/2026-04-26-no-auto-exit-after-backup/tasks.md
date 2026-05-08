# Tasks

## 1. 添加等待退出辅助函数

- [x] 1.1 在 `main.c` 中新增 `wait_for_exit()` 函数，清空输入缓冲区后等待用户按 Enter 键
- [x] 1.2 确保 `wait_for_exit()` 在 UTF-8 控制台模式下正常工作

## 2. 改造退出路径：所有 return 前调用 wait_for_exit

- [x] 2.1 将 `main()` 中 U 盘检测失败 (`return 1`) 改为调用 `wait_for_exit()` 后退出
- [x] 2.2 将 `main()` 中 U 盘选择失败 (`return 1`) 改为调用 `wait_for_exit()` 后退出
- [x] 2.3 将 `main()` 中文件夹创建失败 (`return 1`) 改为调用 `wait_for_exit()` 后退出
- [x] 2.4 将 `main()` 中用户取消备份 (`return 0`) 改为调用 `wait_for_exit()` 后退出
- [x] 2.5 将 `main()` 末尾备份完成后的 `return` 改为调用 `wait_for_exit()` 后退出

## 3. 增加 U 盘检测重试逻辑

- [x] 3.1 将 U 盘检测步骤包裹在 `while` 循环中，检测失败时提供重试/退出选项
- [x] 3.2 用户选择重试时重新检测，检测到后跳出循环进入下一步

## 4. 增加文件夹创建失败重试逻辑

- [x] 4.1 将 `create_timestamp_folder()` 失败后的处理改为询问用户重试或退出
- [x] 4.2 用户选择重试时重新调用 `create_timestamp_folder()`

## 5. 编译验证

- [x] 5.1 使用 `make` 编译，确保无警告无错误
- [x] 5.2 运行 `backup.exe` 快速验证交互流程
