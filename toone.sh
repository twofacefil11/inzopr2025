#!/bin/bash

OUTPUT_FILE="combined_files.txt"

> "$OUTPUT_FILE"

process_directory() {
    local dir=$1
    local extension=$2
    
    find "$dir" -type f -name "*.$extension" | while read -r file; do
        echo "// ======== FILE: $file ========" >> "$OUTPUT_FILE"
        cat "$file" >> "$OUTPUT_FILE"
        echo -e "\n\n" >> "$OUTPUT_FILE"
    done
}

if [ -d "include" ]; then
    process_directory "include" "h"
    process_directory "include" "hpp"
fi

if [ -d "src" ]; then
    process_directory "src" "c"
fi

echo "All combined to: $OUTPUT_FILE"
