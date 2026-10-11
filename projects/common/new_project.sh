#!/bin/bash
# Usage: ./new_project.sh project_number_project_name
# Example: ./new_project.sh 02 blink  (creates directory 02_blink)

if [ $# -ne 2 ]; then
	echo "Usage: $0 project_number project_name"
	echo "Example $0 02 blink"
	exit 1
fi

PROJECT_NUM="$1"
PROJECT_NAME="$2"
# Convert project name to lowercase for directory name
PROJECT_NAME_LOWER=$(echo "$PROJECT_NAME"| tr '[:upper:]' '[:lower:]')
PROJECT_DIR="${PROJECT_NUM}_${PROJECT_NAME_LOWER}"

# Check if project number is already taken
if ls -d ${PROJECT_NUM}_* 1> /dev/null 2>&1; then
	echo "Error: Project number '$PROJECT_NUM' is already taken"
	echo "Existing project: $(ls -d ${PROJECT_NUM}_*)"
	echo "Please use a different project number or remove the existing project first."
	exit 1
fi

# Check if project name is already taken (by any project number)
if ls -d *_${PROJECT_NAME_LOWER} 1> /dev/null 2>&1; then
	echo "Error: Project name '$PROJECT_NAME' is already taken"
	echo "Existing project: $(ls -d *_${PROJECT_NAME_LOWER})"
	echo "Please use a different project name or remove the existing project first."
	exit 1
fi

# Create directory if it doesn't exist
mkdir -p "$PROJECT_DIR"

# Copy Makefile template
cp common/Makefile.template "$PROJECT_DIR/Makefile"

# Create main.c
cat > "$PROJECT_DIR/main.c" << EOF
#include <avr/io.h>
#include <util/delay.h>
#include "../common/avr_io.h"

int main(void) {
	// TODO: initialize hardware

	while(1) {
		// TODO: Main loop
	}

	return 0;
}
EOF

# Create README
cat > "$PROJECT_DIR/README.md" << EOF
# Project: $PROJECT_NAME
# Note: Directory name is lowercase: $PROJECT_DIR

## Objective
[Describe the project goal]

## Hardware Required (PicsimLab)
- [List required parts]

## How to Build
To build the project, run:
```
make
```

## How to Run in PicsimLab
1. [Step 1]
2. [Step 2]

## Code Explanation
[Explain key code sections]

## Learnings
[Document what you learned]
EOF

echo "Project '$PROJECT_NAME' created in directory '$PROJECT_DIR'"
echo "Next steps:"
echo "  1. Edit $PROJECT_DIR/main.c"
echo "  2. Run 'cd $PROJECT_DIR && make'"
echo "  3. Load hex file in PicsimLab"

