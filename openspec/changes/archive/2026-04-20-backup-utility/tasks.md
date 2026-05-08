## 1. Project Setup

- [x] 1.1 Create main.c file with basic structure (includes, main function)
- [x] 1.2 Configure UTF-8 locale for Chinese output
- [x] 1.3 Add Windows headers (windows.h, winbase.h, winnt.h)

## 2. Directory Input Module

- [x] 2.1 Implement wide character input function for user prompts
- [x] 2.2 Parse multiple directory paths (semicolon/newline separated)
- [x] 2.3 Validate each directory exists using GetFileAttributesW
- [x] 2.4 Store valid paths in dynamic array for later processing

## 3. Drive Detection Module

- [x] 3.1 Implement GetLogicalDrives enumeration
- [x] 3.2 Filter drives by GetDriveTypeW == DRIVE_REMOVABLE
- [x] 3.3 Get drive label using GetVolumeInformationW
- [x] 3.4 Display available drives and handle user selection
- [x] 3.5 Handle edge cases: no drives, multiple drives

## 4. Timestamp Folder Creation

- [x] 4.1 Get current system time using GetLocalTime
- [x] 4.2 Format timestamp as YYYYMMDDHHMM (wchar_t buffer)
- [x] 4.3 Handle folder name collision with sequence suffix
- [x] 4.4 Create directory using CreateDirectoryW with full path

## 5. Recursive Copy Module

- [x] 5.1 Implement directory traversal using FindFirstFileW/FindNextFileW
- [x] 5.2 Copy files using CopyFileW with progress display
- [x] 5.3 Recurse into subdirectories
- [x] 5.4 Handle locked files (skip with warning)
- [x] 5.5 Check filesystem type and warn about FAT32 4GB limit

## 6. User Confirmation Workflow

- [x] 6.1 Display backup summary (sources, target, folder name)
- [x] 6.2 Prompt for Y/N confirmation with Chinese prompts
- [x] 6.3 Handle invalid input with re-prompt loop
- [x] 6.4 Execute backup or exit cleanly based on response

## 7. Error Handling & Reporting

- [x] 7.1 Centralized error display function with GetLastError info
- [x] 7.2 File copy counter (success/skip/fail)
- [x] 7.3 Final summary report after backup completes

## 8. Build & Testing

- [x] 8.1 Create Makefile or build script for gcc/clang
- [x] 8.2 Compile with Unicode flags (-DUNICODE -D_UNICODE)
- [x] 8.3 Test with sample directories and USB drive
- [x] 8.4 Verify edge cases: empty dirs, long paths, Chinese filenames
