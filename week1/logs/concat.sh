#!/bin/bash

# Output file
OUTPUT_FILE="logs/final.txt"

# Remove existing output file
rm -f "$OUTPUT_FILE"

# Loop through each file
for file in "$@"; do
    # Get the filename without the path
    filename=$(basename "$file")
    
    # Output filename followed by a colon
    echo "$filename:" >> "$OUTPUT_FILE"
    
    # Output the content of the file
    cat "$file" >> "$OUTPUT_FILE"
    
    # Add a newline after each file's content
    echo >> "$OUTPUT_FILE"
done

echo "Concatenation completed. Output written to $OUTPUT_FILE"
