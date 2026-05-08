## ADDED Requirements

### Requirement: Multi-directory path input
The system SHALL allow users to input multiple source directory paths for backup, supporting both absolute and relative paths with Chinese characters.

#### Scenario: User enters single directory
- **WHEN** user inputs one valid directory path
- **THEN** system accepts and displays the path for confirmation

#### Scenario: User enters multiple directories
- **WHEN** user inputs multiple directory paths (semicolon or newline separated)
- **THEN** system parses and validates each path individually

#### Scenario: User enters invalid path
- **WHEN** user inputs a non-existent directory path
- **THEN** system displays error message and prompts for re-entry

#### Scenario: User enters empty input
- **WHEN** user presses Enter without any input
- **THEN** system prompts again with helpful message

#### Scenario: Path with Chinese characters
- **WHEN** user inputs path containing Chinese characters
- **THEN** system correctly handles and displays the Unicode path
