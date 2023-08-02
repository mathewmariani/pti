#!/bin/bash

# Get extension from user input
ext="ase"
app="/Applications/Aseprite/aseprite"

# Find all files with the given extension and concatenate their names into a string
files=$(find . -type f -name "*.$ext" -exec printf "%s\n" {} + | sed "s|^\./||" | tr '\n' ' ')

# Output the concatenated string of file names
echo "File names with extension .$ext: $files"
$app --batch --list-tags --list-slices --export-tileset $files --sheet output.png --data output.json