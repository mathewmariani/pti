#!/usr/bin/env bash
set -euo pipefail

# URL of the Python script
GL3W_URL="https://raw.githubusercontent.com/skaslev/gl3w/refs/heads/master/gl3w_gen.py"

# Directory where this shell script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Temporary location for the downloaded Python script
TMP_SCRIPT="$(mktemp /tmp/gl3w_gen.XXXXXX.py)"

# Download the script
echo "Downloading gl3w_gen.py..."
curl -fsSL "$GL3W_URL" -o "$TMP_SCRIPT"

# Run the script, tell it to output next to the shell script
echo "Running gl3w_gen.py..."
python3 "$TMP_SCRIPT" "$SCRIPT_DIR"

# Clean up
rm -f "$TMP_SCRIPT"
echo "Done. Files are in $SCRIPT_DIR"
