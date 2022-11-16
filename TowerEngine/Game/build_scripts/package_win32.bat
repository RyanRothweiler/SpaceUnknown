@echo off
rmdir Package /s /q
mkdir Package
mkdir Package\assets

xcopy assets Package\assets /s /e /y
copy build\Windows\Platform_win32.exe Package
ren Package\Platform_win32.exe Game.exe
copy build\Windows\Game*.dll Package

echo Done