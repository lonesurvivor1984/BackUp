## ADDED Requirements

### Requirement: Timestamped destination folder
The system SHALL create a new directory on the USB drive using the format YYYYMMDDHHMM (e.g., 202604201046) to ensure unique backup destinations.

#### Scenario: First backup of the minute
- **WHEN** no folder with current timestamp exists
- **THEN** system creates folder with format YYYYMMDDHHMM

#### Scenario: Folder already exists (same minute)
- **WHEN** a folder with the current timestamp already exists
- **THEN** system appends sequence number (e.g., 202604201046_01)

#### Scenario: Invalid characters in folder name
- **WHEN** creating folder on filesystem with naming restrictions
- **THEN** system uses only filesystem-safe characters (0-9, underscore)

#### Scenario: Folder creation failure
- **WHEN** folder cannot be created (permissions, etc.)
- **THEN** system displays error and aborts backup
