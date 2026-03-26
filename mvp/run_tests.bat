@echo off
setlocal enabledelayedexpansion

REM Flower Exchange Test Runner (Windows Batch)
REM Runs all tests from tests/input and saves results to tests/output

echo ╔═══════════════════════════════════════════════════╗
echo ║    Flower Exchange - Running All Test Cases      ║
echo ╚═══════════════════════════════════════════════════╝
echo.

REM Get the directory where the script is located
cd /d "%~dp0"

REM Executable path
set EXECUTABLE=flower-exchange.exe

REM Test directories
set INPUT_DIR=tests\input
set OUTPUT_DIR=tests\output

REM Create output directory if it doesn't exist
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Counter for tracking
set TOTAL_TESTS=0
set PASSED_TESTS=0
set FAILED_TESTS=0

echo Input directory:  %INPUT_DIR%
echo Output directory: %OUTPUT_DIR%
echo.
echo ═══════════════════════════════════════════════════
echo   Processing all test cases
echo ═══════════════════════════════════════════════════
echo.

REM Run all tests from tests/input folder
if exist "%INPUT_DIR%" (
    for %%f in (%INPUT_DIR%\*.csv) do (
        set INPUT_FILE=%%f
        set TEST_NAME=%%~nf
        set OUTPUT_FILE=%OUTPUT_DIR%\!TEST_NAME!_output.csv

        echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
        echo Running: !TEST_NAME!
        echo ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

        "%EXECUTABLE%" "!INPUT_FILE!" "!OUTPUT_FILE!" >nul 2>&1
        if !errorlevel! equ 0 (
            echo ✓ SUCCESS: Output saved to !OUTPUT_FILE!
            set /a PASSED_TESTS+=1
        ) else (
            echo ✗ FAILED: Error processing !INPUT_FILE!
            set /a FAILED_TESTS+=1
        )

        set /a TOTAL_TESTS+=1
        echo.
    )
) else (
    echo ⚠ Error: %INPUT_DIR% directory not found
    echo Please make sure test CSV files are in %INPUT_DIR%\
    pause
    exit /b 1
)

REM Print summary
echo ╔═══════════════════════════════════════════════════╗
echo ║              TEST EXECUTION SUMMARY               ║
echo ╚═══════════════════════════════════════════════════╝
echo.
echo Total tests run:     !TOTAL_TESTS!
echo Passed:              !PASSED_TESTS! ✓
echo Failed:              !FAILED_TESTS! ✗
echo.
echo All output files saved to: %OUTPUT_DIR%\
echo.

if !FAILED_TESTS! equ 0 (
    echo 🎉 All tests completed successfully!
) else (
    echo ⚠ Some tests failed. Please review the output.
)

echo.
pause
