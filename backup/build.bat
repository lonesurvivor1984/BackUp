@echo off
chcp 65001 >nul
echo ========================================
echo      资料备份工具 - 构建脚本
echo ========================================
echo.

REM 检查 gcc 是否可用
where gcc >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到 GCC 编译器
    echo.
    echo 请安装 MinGW 或 WSL，确保 gcc 在 PATH 中
    echo.
    pause
    exit /b 1
)

REM 检查 windres 是否可用
where windres >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [警告] 未找到 windres 工具，将跳过图标编译
    echo.
)

echo [信息] 使用编译器：gcc
gcc --version | findstr /R "^gcc"
echo.

echo [信息] 开始编译...
echo.

REM 编译资源文件 (图标)
if exist resource.rc (
    echo [信息] 编译资源文件 resource.rc...
    windres -o resource.o resource.rc
    if %ERRORLEVEL% NEQ 0 (
        echo [警告] 资源编译失败，将继续编译但不包含图标
    ) else (
        echo [信息] 资源编译完成
        echo.
    )
)

REM 编译主程序
if exist resource.o (
    gcc -Wall -Wextra -O2 -DUNICODE -D_UNICODE -o backup.exe main.c resource.o
) else (
    gcc -Wall -Wextra -O2 -DUNICODE -D_UNICODE -o backup.exe main.c
)

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo      编译成功!
    echo ========================================
    echo.
    echo 生成的文件：backup.exe
    echo.
    echo 运行方式:
    echo   1. 直接双击运行 backup.exe
    echo   2. 或在命令行运行：backup.exe
    echo.
) else (
    echo.
    echo ========================================
    echo      编译失败!
    echo ========================================
    echo.
    echo 请检查错误信息 above
    echo.
)

pause
