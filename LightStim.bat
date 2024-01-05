@echo off
setlocal enabledelayedexpansion

REM Get the directory of the batch file (where the batch file is located)
set "batch_dir=%~dp0"

REM Set the Python script file name
set "script_name=testUI.py"

REM Combine the batch file directory and script name to create the full script path
set "script_path=!batch_dir!!script_name!"

REM Run the Python script using Python
python "!script_path!"

endlocal
