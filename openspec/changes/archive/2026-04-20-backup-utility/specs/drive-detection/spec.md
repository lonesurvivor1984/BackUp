## ADDED Requirements

### Requirement: USB drive detection
The system SHALL automatically detect available USB/removable drives and allow selection of target drive for backup.

#### Scenario: Single USB drive connected
- **WHEN** exactly one USB drive is connected
- **THEN** system automatically selects it and displays drive letter and label

#### Scenario: Multiple USB drives connected
- **WHEN** multiple removable drives are available
- **THEN** system lists all drives and prompts user to select one

#### Scenario: No USB drive connected
- **WHEN** no removable drives are detected
- **THEN** system displays error message and prompts user to insert a drive

#### Scenario: USB drive with insufficient space
- **WHEN** selected drive has less free space than estimated backup size
- **THEN** system warns user but allows continuation

#### Scenario: Drive letter access failure
- **WHEN** drive is detected but cannot be accessed
- **THEN** system displays specific error and offers to retry
