#!/bin/bash

# Count and remove all .res files in the allTests directory and subdirectories
echo "Removing .res files from test directories..."

# Find and count the number of .res files
RES_COUNT=$(find allTests -name "*.res" | wc -l)

# Remove all .res files
find allTests -name "*.res" -delete

echo "Removed $RES_COUNT .res files."
