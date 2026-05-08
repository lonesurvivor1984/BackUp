## ADDED Requirements

### Requirement: User confirmation workflow
The system SHALL display a summary of the backup operation and require explicit user confirmation before proceeding with the copy.

#### Scenario: User confirms backup
- **WHEN** user inputs 'Y' or 'Yes' (case-insensitive)
- **THEN** system proceeds with backup operation

#### Scenario: User cancels backup
- **WHEN** user inputs 'N' or 'No' or 'Q' (case-insensitive)
- **THEN** system aborts and exits cleanly without making changes

#### Scenario: Invalid confirmation input
- **WHEN** user inputs unrecognized text
- **THEN** system re-prompts with valid options

#### Scenario: Summary display
- **WHEN** showing confirmation prompt
- **THEN** system displays: source directories, target drive, destination folder name, estimated file count
