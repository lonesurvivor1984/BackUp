## Why

Users need a simple, reliable way to backup important files to removable drives. This solves the problem of manual, error-prone backup processes by providing an automated C-based utility that handles multi-directory backup with timestamped destination folders.

## What Changes

- **New executable**: `backup.exe` - command-line backup utility
- **Interactive input**: Prompts users to enter one or more source directory paths
- **Confirmation step**: Shows summary before proceeding with backup
- **Automatic U盘 detection**: Finds available removable drives
- **Timestamped folder creation**: Creates destination folder with format `YYYYMMDDHHMM`
- **Recursive directory copying**: Preserves folder structure during backup
- **Progress reporting**: Shows files being copied and any errors

## Capabilities

### New Capabilities
- `directory-input`: Handle multiple directory path input from user
- `drive-detection`: Detect and select available USB/removable drives
- `timestamp-folder`: Create timestamped destination directories
- `recursive-copy`: Copy directory trees with error handling
- `user-confirmation`: Interactive confirmation workflow

### Modified Capabilities
- (none - this is a new standalone utility)

## Impact

- **Platform**: Windows (uses Windows API for drive detection and file operations)
- **Dependencies**: None (standard C library + Windows API)
- **Filesystem**: Requires read access to source directories, write access to target drive
- **Locale**: Chinese (Simplified) for user prompts and messages
