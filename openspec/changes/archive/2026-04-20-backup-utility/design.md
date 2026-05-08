## Context

A simple backup utility is needed for Windows users to backup multiple directories to USB drives. The program must:
- Accept multiple directory paths from user input
- Detect available USB/removable drives
- Create timestamped destination folders
- Copy files recursively with error handling
- Display progress and results in Chinese

**Constraints:**
- Windows-only (uses Win32 API)
- No external dependencies beyond standard C library
- Must handle long paths (MAX_PATH limitation)
- Must work with various filesystems (NTFS, FAT32, exFAT)

## Goals / Non-Goals

**Goals:**
- Single executable with no dependencies
- Support unlimited source directories
- Auto-detect USB drives
- Create unique timestamped folders (YYYYMMDDHHMM format)
- Preserve directory structure during copy
- Handle errors gracefully with clear messages
- Chinese language interface

**Non-Goals:**
- Cross-platform support (Linux/Mac)
- Incremental/differential backups
- Compression or encryption
- Network drives
- GUI interface
- Scheduling or automation

## Decisions

### 1. Windows API vs Cross-platform libraries
**Decision:** Use Win32 API directly

**Rationale:**
- No external dependencies needed
- Full access to drive detection APIs (`GetDriveTypeW`, `GetLogicalDrives`)
- Better Unicode support with wide character functions
- Simpler error handling with `GetLastError()`

**Alternatives considered:**
- POSIX libraries (would require MinGW compatibility layer)
- libuv (adds dependency, overkill for this use case)

### 2. Path handling: ANSI vs Unicode
**Decision:** Use wide characters (wchar_t) throughout

**Rationale:**
- Chinese characters in file paths
- Long path support (>260 characters)
- Windows internally uses UTF-16

**Implementation:**
- Use `wchar_t*` strings
- `_wgetcwd()`, `_wchdir()`, `_wmkdir()`
- `_wfindfirst()`, `_wfindnext()` for directory traversal

### 3. Recursive copy: Custom vs Shell API
**Decision:** Custom recursive implementation with `CopyFileW`

**Rationale:**
- Full control over error handling
- Progress reporting per file
- No COM initialization needed
- Smaller binary size

**Alternatives considered:**
- `SHFileOperation()` (deprecated, requires COM)
- `CopyFileEx()` (more complex callback handling)

### 4. Drive detection strategy
**Decision:** Check all drive letters A-Z with `GetDriveTypeW`

**Rationale:**
- Simple and reliable
- No WMI queries needed
- Fast execution
- Handles hot-plugged drives

### 5. Timestamp format
**Decision:** `YYYYMMDDHHMM` (12 characters, no separators)

**Rationale:**
- Filesystem-safe (no colons or special chars)
- Sortable lexicographically
- Clear and unambiguous
- Matches user requirement (e.g., 202604201046)

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| USB drive full during copy | Check free space before each file, report clear error |
| Source directory deleted mid-copy | Validate paths at start, fail fast |
| Long paths (>260 chars) | Use `\\?\` prefix for extended-length paths |
| File in use/locked | Skip with warning, continue with rest |
| FAT32 4GB file limit | Warn if file >4GB on FAT32 drive |
| USB drive disconnected mid-operation | OS handles this, report write error |
| Special characters in paths | Wide character handling throughout |
| Insufficient permissions | Check access early, clear error messages |

## Open Questions

- Should we add a dry-run mode to preview what would be copied?
- Should we support exclude patterns (e.g., *.tmp, .git)?
- Should we create a log file in the backup folder?
