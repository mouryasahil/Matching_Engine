@echo off
echo Building Order Matching Engine...

:: Set compiler and flags
set COMPILER=g++
set FLAGS=-std=c++17 -pthread

:: Source files
set SOURCES=main.cpp Order.cpp OrderBook.cpp

:: Output file
set OUTPUT=MatchingEngine.exe

:: Compile the program
%COMPILER% %FLAGS% %SOURCES% -o %OUTPUT%

:: Check for success
if %errorlevel% equ 0 (
    echo  Build successful!
) else (
    echo  Build failed!
)

pause
