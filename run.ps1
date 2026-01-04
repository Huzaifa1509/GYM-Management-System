# GYM Management System - Windows Runner
# This script builds and runs the project using MSYS2

Write-Host "🏋️  GYM Management System - Building..." -ForegroundColor Cyan

# Set MSYS2 environment
$env:MSYSTEM = "MINGW64"
$msys2Path = "C:\msys64"
$mingw64Bin = "$msys2Path\mingw64\bin"

# Change to project directory
$projectDir = "E:\GYM Management System"

# Build the project using MSYS2 bash
Write-Host "Building project..." -ForegroundColor Yellow
& "$msys2Path\usr\bin\bash.exe" -lc "cd '$($projectDir -replace '\\', '/')' && make"

if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Build successful!" -ForegroundColor Green
    Write-Host ""
    Write-Host "🚀 Starting GYM Management System..." -ForegroundColor Cyan
    Write-Host ""
    
    # Run the executable
    & "$msys2Path\usr\bin\bash.exe" -lc "cd '$($projectDir -replace '\\', '/')' && ./bin/gym_system"
} else {
    Write-Host "❌ Build failed! Check the errors above." -ForegroundColor Red
    exit 1
}
