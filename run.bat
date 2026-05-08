@echo off
chcp 65001 > nul
echo =========================================
echo   DummyDataGenerator for DataPersistence
echo =========================================
echo.

python dummy_generator.py %*

echo.
pause
