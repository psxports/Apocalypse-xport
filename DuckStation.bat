@echo off
setlocal
cd /d "%~dp0"
python -B "%~dp0..\xport\tools\xport.py" --project "%~dp0." duckstation_run --role user --visible %*
if errorlevel 1 (
  echo DuckStation launch failed. See the error above.
  pause
  exit /b 1
)
endlocal
