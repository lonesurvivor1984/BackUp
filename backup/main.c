/*
 * 资料备份工具 - Backup Utility
 * 支持多目录备份到可移动存储设备，时间戳文件夹命名
 * 使用 UTF-8 编码输出中文
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <windows.h>
#include <winioctl.h>

#define MAX_PATHS 100
#define MAX_PATH_LEN 1024
#define MAX_INPUT_LEN 4096

/* 全局统计 */
static int g_files_copied = 0;
static int g_files_skipped = 0;
static int g_files_failed = 0;
static int g_fat32_warning_shown = 0;
static FILE *g_log = NULL;

/* 显示错误信息 */
void print_error(const char *msg) {
    DWORD err = GetLastError();
    fprintf(stderr, "[错误] %s (错误代码：%lu)\n", msg, err);
}

/* 日志写入 */
void log_write(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (g_log) {
        vfprintf(g_log, fmt, args);
        fflush(g_log);
    }
    vfprintf(stdout, fmt, args);
    va_end(args);
}

/* 打开日志文件 */
int log_open(const wchar_t *dest_folder) {
    wchar_t log_path[MAX_PATH];
    SYSTEMTIME st;
    GetLocalTime(&st);
    swprintf(log_path, MAX_PATH, L"%s\\backup_log_%04d%02d%02d_%02d%02d%02d.txt",
             dest_folder, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    g_log = _wfopen(log_path, L"w, ccs=UTF-8");
    if (g_log) {
        log_write("=== 备份日志 ===\n");
        log_write("开始时间：%04d-%02d-%02d %02d:%02d:%02d\n",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        log_write("----------------------------------------\n\n");
        return 0;
    }
    return -1;
}

/* 关闭日志文件 */
void log_close() {
    if (g_log) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fprintf(g_log, "\n----------------------------------------\n");
        fprintf(g_log, "结束时间：%04d-%02d-%02d %02d:%02d:%02d\n",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        fprintf(g_log, "总计文件：%d  成功：%d  跳过：%d  失败：%d\n",
            g_files_copied + g_files_skipped + g_files_failed,
            g_files_copied, g_files_skipped, g_files_failed);
        fclose(g_log);
        g_log = NULL;
    }
}

/* 输入行 */
int read_line(char *buffer, int max_len) {
    if (fgets(buffer, max_len, stdin) == NULL) {
        return -1;
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    return (int)len;
}

/* 目录列表 */
typedef struct {
    wchar_t paths[MAX_PATHS][MAX_PATH_LEN];
    int count;
} PathList;

/* 驱动器信息 */
typedef struct {
    wchar_t drive_letter;
    wchar_t label[256];
    wchar_t path[8];
    ULONGLONG free_space;
    ULONGLONG total_space;
} DriveInfo;

/* 检查 FAT32 */
int check_fat32_limit(const wchar_t *drive_path) {
    wchar_t fs_type[32];
    if (GetVolumeInformationW(drive_path, NULL, 0, NULL, NULL, NULL, fs_type, 32)) {
        if (_wcsicmp(fs_type, L"FAT32") == 0) {
            return 1;
        }
    }
    return 0;
}

/* 检查是否为 USB 连接的设备（用于识别移动硬盘） */
int is_usb_drive(const wchar_t *drive_path) {
    /* 构造设备路径: \\\\.\\X: */
    wchar_t device_path[16];
    swprintf(device_path, 16, L"\\\\.\\%c:", drive_path[0]);

    HANDLE h = CreateFileW(device_path, GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        return 0;
    }

    STORAGE_PROPERTY_QUERY query;
    memset(&query, 0, sizeof(query));
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;

    BYTE buffer[1024];
    DWORD bytes_returned = 0;

    if (!DeviceIoControl(h, IOCTL_STORAGE_QUERY_PROPERTY,
            &query, sizeof(query), buffer, sizeof(buffer),
            &bytes_returned, NULL)) {
        CloseHandle(h);
        return 0;
    }
    CloseHandle(h);

    STORAGE_DEVICE_DESCRIPTOR *desc = (STORAGE_DEVICE_DESCRIPTOR *)buffer;
    return (desc->BusType == BusTypeUsb);
}

/* 解析目录输入 */
int parse_directory_input(PathList *list) {
    char input[MAX_INPUT_LEN];
    wchar_t w_input[MAX_INPUT_LEN];

    list->count = 0;

    printf("\n请输入要备份的目录路径：\n");
    printf("  - 多个目录用分号 (; 或 ；) 分隔\n");
    printf("  - 例如：C:\\Documents;D:\\Photos\n");
    printf("\n目录路径：");

    if (read_line(input, MAX_INPUT_LEN) < 0) {
        printf("输入错误，请重试。\n");
        return -1;
    }

    if (strlen(input) == 0) {
        printf("输入不能为空，请重新输入。\n");
        return -1;
    }

    /* 转为宽字符 */
    MultiByteToWideChar(CP_UTF8, 0, input, -1, w_input, MAX_INPUT_LEN);

    /* 解析分号分隔的路径 */
    wchar_t *token = wcstok(w_input, L";；");
    while (token != NULL && list->count < MAX_PATHS) {
        wchar_t *start = token;
        while (*start == L' ' || *start == L'\t') start++;
        wchar_t *end = start + wcslen(start) - 1;
        while (end > start && (*end == L' ' || *end == L'\t')) *end-- = L'\0';

        if (wcslen(start) > 0) {
            wcsncpy(list->paths[list->count], start, MAX_PATH_LEN - 1);
            list->paths[list->count][MAX_PATH_LEN - 1] = L'\0';
            list->count++;
        }
        token = wcstok(NULL, L";；");
    }

    if (list->count == 0) {
        printf("未找到有效路径，请重新输入。\n");
        return -1;
    }

    return 0;
}

/* 验证目录 */
int validate_directories(PathList *list) {
    int valid_count = 0;
    char mb_path[MAX_PATH_LEN * 3];

    for (int i = 0; i < list->count; i++) {
        DWORD attrs = GetFileAttributesW(list->paths[i]);

        if (attrs == INVALID_FILE_ATTRIBUTES) {
            WideCharToMultiByte(CP_UTF8, 0, list->paths[i], -1, mb_path, sizeof(mb_path), NULL, NULL);
            printf("[警告] 目录不存在或无法访问：%s\n", mb_path);
        } else if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            WideCharToMultiByte(CP_UTF8, 0, list->paths[i], -1, mb_path, sizeof(mb_path), NULL, NULL);
            printf("[警告] 路径不是目录：%s\n", mb_path);
        } else {
            valid_count++;
        }
    }
    return valid_count;
}

/* 检测可移动存储设备（U盘 + USB移动硬盘） */
int detect_removable_drives(DriveInfo *drives, int max_drives) {
    DWORD drives_mask = GetLogicalDrives();
    int count = 0;

    for (char letter = 'A'; letter <= 'Z' && count < max_drives; letter++) {
        DWORD bit = 1 << (letter - 'A');
        if (drives_mask & bit) {
            wchar_t drive_path[8];
            swprintf(drive_path, 8, L"%c:\\", letter);

            DWORD type = GetDriveTypeW(drive_path);
            if (type == DRIVE_REMOVABLE ||
                (type == DRIVE_FIXED && is_usb_drive(drive_path))) {
                drives[count].drive_letter = letter;
                wcscpy(drives[count].path, drive_path);

                DWORD serial;
                wchar_t fs_type[32];
                if (GetVolumeInformationW(drive_path, drives[count].label, 256,
                        &serial, NULL, NULL, fs_type, 32)) {
                    ULARGE_INTEGER free, total;
                    if (GetDiskFreeSpaceExW(drive_path, &free, &total, NULL)) {
                        drives[count].free_space = free.QuadPart;
                        drives[count].total_space = total.QuadPart;
                    } else {
                        drives[count].free_space = 0;
                        drives[count].total_space = 0;
                    }
                } else {
                    wcscpy(drives[count].label, L"未知");
                    drives[count].free_space = 0;
                    drives[count].total_space = 0;
                }
                count++;
            }
        }
    }
    return count;
}

/* 选择目标驱动器 */
int select_drive(DriveInfo *drives, int count) {
    char input[32];

    if (count == 0) {
        printf("\n[错误] 未检测到可移动存储设备！\n");
        printf("请插入 U 盘或移动硬盘后重试。\n");
        return -1;
    }

    printf("\n检测到 %d 个可移动驱动器：\n", count);
    for (int i = 0; i < count; i++) {
        char mb_label[512];
        WideCharToMultiByte(CP_UTF8, 0, drives[i].label, -1, mb_label, sizeof(mb_label), NULL, NULL);
        printf("  [%d] %c: - %s\n", i + 1, drives[i].drive_letter, mb_label);
        if (drives[i].free_space > 0) {
            printf("      可用空间：%.2f GB\n", (double)drives[i].free_space / (1024 * 1024 * 1024));
        }
    }

    printf("\n请选择驱动器编号 (1-%d): ", count);
    if (read_line(input, sizeof(input)) < 0) {
        printf("无效的选择。\n");
        return -1;
    }

    int choice = atoi(input);
    if (choice < 1 || choice > count) {
        printf("无效的选择。\n");
        return -1;
    }
    return choice - 1;
}

/* 创建时间戳文件夹 */
int create_timestamp_folder(const wchar_t *drive_path, wchar_t *dest_path, int max_len) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    wchar_t base_name[32];
    swprintf(base_name, 32, L"%04d%02d%02d%02d%02d",
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

    swprintf(dest_path, max_len, L"%s%s", drive_path, base_name);

    DWORD attrs = GetFileAttributesW(dest_path);
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        int seq = 1;
        do {
            swprintf(dest_path, max_len, L"%s%s_%02d", drive_path, base_name, seq++);
            attrs = GetFileAttributesW(dest_path);
        } while (attrs != INVALID_FILE_ATTRIBUTES && seq < 100);

        if (seq >= 100) {
            printf("[错误] 无法创建唯一文件夹名\n");
            return -1;
        }
    }

    if (!CreateDirectoryW(dest_path, NULL)) {
        DWORD err = GetLastError();
        if (err != ERROR_ALREADY_EXISTS) {
            print_error("无法创建备份文件夹");
            return -1;
        }
    }

    char mb_dest[MAX_PATH * 3];
    WideCharToMultiByte(CP_UTF8, 0, dest_path, -1, mb_dest, sizeof(mb_dest), NULL, NULL);
    printf("[信息] 已创建备份文件夹：%s\n", mb_dest);
    return 0;
}

/* 拷贝文件 */
int copy_file_with_progress(const wchar_t *src, const wchar_t *dst, int is_fat32) {
    char mb_src[MAX_PATH * 3];
    WideCharToMultiByte(CP_UTF8, 0, src, -1, mb_src, sizeof(mb_src), NULL, NULL);

    WIN32_FILE_ATTRIBUTE_DATA attr;
    ULARGE_INTEGER file_size = {{0}};
    int got_attrs = 0;

    if (GetFileAttributesExW(src, GetFileExInfoStandard, &attr)) {
        file_size.LowPart = attr.nFileSizeLow;
        file_size.HighPart = attr.nFileSizeHigh;
        got_attrs = 1;

        if (is_fat32 && file_size.QuadPart > 4294967295ULL && !g_fat32_warning_shown) {
            log_write("  [警告] FAT32 不支持大于 4GB 的文件：%s\n", mb_src);
            log_write("  [日志] 源路径: %s\n", mb_src);
            g_fat32_warning_shown = 1;
            g_files_skipped++;
            return -1;
        }
    }

    /* 拷贝前显示进度 */
    if (got_attrs && file_size.QuadPart > 0) {
        log_write("[%d+] > %s (%.2f MB)\n",
            g_files_copied + g_files_skipped + g_files_failed + 1,
            mb_src,
            (double)file_size.QuadPart / (1024 * 1024));
    } else {
        log_write("[%d+] > %s\n",
            g_files_copied + g_files_skipped + g_files_failed + 1,
            mb_src);
    }

    if (CopyFileW(src, dst, FALSE)) {
        g_files_copied++;
        log_write("[%d+] OK\n",
            g_files_copied + g_files_skipped + g_files_failed);
        return 0;
    } else {
        DWORD err = GetLastError();
        g_files_failed++;
        log_write("  x %s (错误：%lu)\n", mb_src, err);
        return -1;
    }
}

/* 递归拷贝目录 */
int copy_directory_recursive(const wchar_t *src_dir, const wchar_t *rel_path,
                             const wchar_t *dest_folder, int is_fat32) {
    wchar_t search_path[MAX_PATH];
    wchar_t src_path[MAX_PATH];
    wchar_t dst_sub_path[MAX_PATH];

    if (wcslen(rel_path) > 0) {
        swprintf(search_path, MAX_PATH, L"%s\\%s\\*.*", src_dir, rel_path);
    } else {
        swprintf(search_path, MAX_PATH, L"%s\\*.*", src_dir);
    }

    WIN32_FIND_DATAW find_data;
    HANDLE h_find = FindFirstFileW(search_path, &find_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        return -1;
    }

    do {
        if (wcscmp(find_data.cFileName, L".") == 0 ||
            wcscmp(find_data.cFileName, L"..") == 0) {
            continue;
        }

        /* 跳过 junction / symlink，防止无限递归 */
        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
            char mb_name[MAX_PATH * 3];
            WideCharToMultiByte(CP_UTF8, 0, find_data.cFileName, -1,
                mb_name, sizeof(mb_name), NULL, NULL);
            log_write("  [跳过] %s (链接)\n", mb_name);
            g_files_skipped++;
            continue;
        }

        /* 检查路径长度，超长则跳过 */
        size_t src_len = wcslen(src_dir) + 1 + wcslen(rel_path) +
            wcslen(find_data.cFileName) + 4;
        if (src_len >= MAX_PATH) {
            char mb_name[MAX_PATH * 3];
            WideCharToMultiByte(CP_UTF8, 0, find_data.cFileName, -1,
                mb_name, sizeof(mb_name), NULL, NULL);
            log_write("  [跳过] %s (路径过长)\n", mb_name);
            g_files_skipped++;
            continue;
        }

        if (wcslen(rel_path) > 0) {
            swprintf(src_path, MAX_PATH, L"%s\\%s\\%s", src_dir, rel_path, find_data.cFileName);
            swprintf(dst_sub_path, MAX_PATH, L"%s\\%s\\%s", dest_folder, rel_path, find_data.cFileName);
        } else {
            swprintf(src_path, MAX_PATH, L"%s\\%s", src_dir, find_data.cFileName);
            swprintf(dst_sub_path, MAX_PATH, L"%s\\%s", dest_folder, find_data.cFileName);
        }

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            wchar_t new_rel_path[MAX_PATH];
            if (wcslen(rel_path) > 0) {
                swprintf(new_rel_path, MAX_PATH, L"%s\\%s", rel_path, find_data.cFileName);
            } else {
                wcscpy(new_rel_path, find_data.cFileName);
            }

            if (CreateDirectoryW(dst_sub_path, NULL) ||
                GetLastError() == ERROR_ALREADY_EXISTS) {
                copy_directory_recursive(src_dir, new_rel_path, dest_folder, is_fat32);
            }
        } else {
            copy_file_with_progress(src_path, dst_sub_path, is_fat32);
        }

    } while (FindNextFileW(h_find, &find_data));

    FindClose(h_find);
    return 0;
}

/* 用户确认 */
int confirm_backup(PathList *paths, DriveInfo *drive, const wchar_t *dest_folder) {
    char input[32];
    char mb_folder[MAX_PATH * 3];
    char mb_label[512];

    printf("\n========================================\n");
    printf("        备份操作确认\n");
    printf("========================================\n\n");

    printf("源目录 (%d 个):\n", paths->count);
    for (int i = 0; i < paths->count; i++) {
        char mb_path[MAX_PATH * 3];
        WideCharToMultiByte(CP_UTF8, 0, paths->paths[i], -1, mb_path, sizeof(mb_path), NULL, NULL);
        printf("  - %s\n", mb_path);
    }

    WideCharToMultiByte(CP_UTF8, 0, drive->label, -1, mb_label, sizeof(mb_label), NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, dest_folder, -1, mb_folder, sizeof(mb_folder), NULL, NULL);

    printf("\n目标驱动器：%c: (%s)\n", drive->drive_letter, mb_label);
    printf("备份文件夹：%s\n", mb_folder);

    printf("\n----------------------------------------\n");

    while (1) {
        printf("是否继续备份？(Y/N): ");
        if (read_line(input, sizeof(input)) < 0) {
            return 0;
        }

        /* 转小写 */
        for (int i = 0; input[i]; i++) {
            if (input[i] >= 'A' && input[i] <= 'Z') {
                input[i] += 32;
            }
        }

        if (strcmp(input, "y") == 0 || strcmp(input, "yes") == 0 ||
            strcmp(input, "是") == 0) {
            return 1;
        } else if (strcmp(input, "n") == 0 || strcmp(input, "no") == 0 ||
                   strcmp(input, "否") == 0 || strcmp(input, "q") == 0) {
            printf("备份已取消。\n");
            return 0;
        } else {
            printf("无效输入，请输入 Y 或 N: ");
        }
    }
}

/* 最终报告 */
void print_final_report() {
    printf("\n========================================\n");
    printf("        备份完成报告\n");
    printf("========================================\n\n");

    int total = g_files_copied + g_files_skipped + g_files_failed;
    printf("总计文件：%d\n", total);
    printf("成功拷贝：%d\n", g_files_copied);
    printf("跳过：%d\n", g_files_skipped);
    printf("失败：%d\n", g_files_failed);

    if (g_files_failed > 0) {
        printf("\n[警告] 部分文件拷贝失败，请检查日志。\n");
    } else {
        printf("\n备份成功完成！\n");
    }
}

/* 程序头 */
void print_header() {
    printf("\n========================================\n");
    printf("     资料备份工具 v1.0\n");
    printf("========================================\n");
}

/* 等待用户按 Enter 键后退出 */
void wait_for_exit() {
    /* 清空输入缓冲区残留 */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
    printf("\n按 Enter 键退出...");
    getchar();
}

int main() {
    /* 设置控制台为 UTF-8 模式 */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    print_header();

    PathList paths;
    DriveInfo drives[26];
    wchar_t dest_folder[MAX_PATH];
    int drive_count = 0;
    int selected_drive = -1;
    char input[32];

    /* 步骤 1: 获取目录输入 */
    while (1) {
        if (parse_directory_input(&paths) < 0) {
            continue;
        }
        if (validate_directories(&paths) > 0) {
            break;
        }
    }

    /* 步骤 2: 检测可移动存储设备（支持重试） */
    while (1) {
        drive_count = detect_removable_drives(drives, 26);
        if (drive_count > 0) {
            break;
        }

        printf("\n[错误] 未检测到可移动存储设备！\n");
        printf("请插入 U 盘或移动硬盘后重试。\n");
        printf("是否重新检测？(Y/N): ");
        if (read_line(input, sizeof(input)) < 0) {
            printf("备份已取消。\n");
            wait_for_exit();
            return 0;
        }
        /* 转小写 */
        for (int i = 0; input[i]; i++) {
            if (input[i] >= 'A' && input[i] <= 'Z') {
                input[i] += 32;
            }
        }
        if (strcmp(input, "y") != 0 && strcmp(input, "yes") != 0 && strcmp(input, "是") != 0) {
            printf("已退出。\n");
            wait_for_exit();
            return 0;
        }
        printf("重新检测中...\n");
    }

    /* 步骤 2.5: 选择驱动器 */
    selected_drive = select_drive(drives, drive_count);
    if (selected_drive < 0) {
        printf("未选择驱动器，已退出。\n");
        wait_for_exit();
        return 1;
    }

    /* 步骤 3: 创建时间戳文件夹（支持重试） */
    while (1) {
        if (create_timestamp_folder(drives[selected_drive].path, dest_folder, MAX_PATH) == 0) {
            break;
        }

        printf("\n是否重试？(Y/N): ");
        if (read_line(input, sizeof(input)) < 0) {
            printf("备份已取消。\n");
            wait_for_exit();
            return 0;
        }
        for (int i = 0; input[i]; i++) {
            if (input[i] >= 'A' && input[i] <= 'Z') {
                input[i] += 32;
            }
        }
        if (strcmp(input, "y") != 0 && strcmp(input, "yes") != 0 && strcmp(input, "是") != 0) {
            printf("已退出。\n");
            wait_for_exit();
            return 1;
        }
        printf("重新尝试创建文件夹...\n");
    }

    /* 步骤 4: 用户确认 */
    if (!confirm_backup(&paths, &drives[selected_drive], dest_folder)) {
        RemoveDirectoryW(dest_folder);
        wait_for_exit();
        return 0;
    }

    /* 步骤 5: 执行备份 */
    printf("\n开始备份...\n\n");

    /* 打开日志文件 */
    log_open(dest_folder);
    for (int i = 0; i < paths.count; i++) {
        char mb_path[MAX_PATH * 3];
        WideCharToMultiByte(CP_UTF8, 0, paths.paths[i], -1, mb_path, sizeof(mb_path), NULL, NULL);
        log_write("源目录 %d: %s\n", i + 1, mb_path);
    }
    log_write("\n");

    /* 检查 FAT32 */
    int is_fat32 = check_fat32_limit(drives[selected_drive].path);
    if (is_fat32) {
        log_write("[警告] 目标驱动器为 FAT32 格式，不支持单文件大于 4GB\n\n");
    }

    for (int i = 0; i < paths.count; i++) {
        char mb_dir[MAX_PATH * 3];
        WideCharToMultiByte(CP_UTF8, 0, paths.paths[i], -1, mb_dir, sizeof(mb_dir), NULL, NULL);
        printf("\n处理目录：%s\n", mb_dir);
        printf("----------------------------------------\n");

        /* 获取目录名作为子文件夹 */
        wchar_t *last_slash = wcsrchr(paths.paths[i], L'\\');
        wchar_t dir_name[MAX_PATH] = L"";
        if (last_slash) {
            wcscpy(dir_name, last_slash + 1);
        } else {
            wcscpy(dir_name, paths.paths[i]);
        }

        /* 在备份文件夹中创建子目录 */
        wchar_t src_dir_dest[MAX_PATH];
        swprintf(src_dir_dest, MAX_PATH, L"%s\\%s", dest_folder, dir_name);
        CreateDirectoryW(src_dir_dest, NULL);

        /* 递归拷贝 */
        copy_directory_recursive(paths.paths[i], L"", src_dir_dest, is_fat32);
    }

    /* 步骤 6: 关闭日志并显示报告 */
    log_close();
    print_final_report();

    wait_for_exit();
    return (g_files_failed > 0) ? 1 : 0;
}
