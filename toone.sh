#!/bin/bash

OUTPUT_FILE="combined_files.txt"

# Clear or create the output file
> "$OUTPUT_FILE"

# Function to process files in a directory
process_directory() {
    local dir=$1
    local extension=$2
    
    find "$dir" -type f -name "*.$extension" | while read -r file; do
        echo "// ======== FILE: $file ========" >> "$OUTPUT_FILE"
        cat "$file" >> "$OUTPUT_FILE"
        echo -e "\n\n" >> "$OUTPUT_FILE"
    done
}

# Process .h and .hpp files in include/
if [ -d "include" ]; then
    process_directory "include" "h"
    process_directory "include" "hpp"
fi

# Process .cpp files in src/
if [ -d "src" ]; then
    process_directory "src" "c"
fi

echo "All files have been combined into $OUTPUT_FILE"
