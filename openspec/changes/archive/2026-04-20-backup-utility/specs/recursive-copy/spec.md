## ADDED Requirements

### Requirement: Recursive directory copy
The system SHALL copy all files and subdirectories from source directories to the destination, preserving the directory structure and file attributes.

#### Scenario: Simple file copy
- **WHEN** copying a directory with files
- **THEN** all files are copied with their content intact

#### Scenario: Nested subdirectories
- **WHEN** source directory contains subdirectories
- **THEN** complete directory structure is recreated in destination

#### Scenario: File in use/locked
- **WHEN** a source file is locked by another process
- **THEN** system skips the file, logs warning, and continues with remaining files

#### Scenario: Copy progress display
- **WHEN** copying files
- **THEN** system displays current file path and running count

#### Scenario: Empty directories
- **WHEN** source contains empty directories
- **THEN** empty directories are also created in destination

#### Scenario: Large file on FAT32
- **WHEN** copying file larger than 4GB to FAT32 filesystem
- **THEN** system warns user about file size limitation
