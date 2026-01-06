# Exit on errors
$ErrorActionPreference = "Stop"

# URL of the Python script
$gl3wUrl = "https://raw.githubusercontent.com/skaslev/gl3w/refs/heads/master/gl3w_gen.py"

# Directory where this script is located
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Temporary file to store the script
$tmpFile = [System.IO.Path]::GetTempFileName() + ".py"

Write-Host "Downloading gl3w_gen.py..."
Invoke-WebRequest -Uri $gl3wUrl -OutFile $tmpFile

Write-Host "Running gl3w_gen.py..."
python3 $tmpFile --root "$scriptDir"

# Remove temporary file
Remove-Item $tmpFile -Force
Write-Host "Done. Files are in $scriptDir"
