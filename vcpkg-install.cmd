@echo off
REM Set VCPKG root
set VCPKG_ROOT=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg

REM Change to project directory
cd /d C:\Users\nydil\source\repos\ResoddedFramework2

REM Remove old cache
if exist vcpkg_installed rmdir /s /q vcpkg_installed

REM Install all dependencies for x64-windows-static
echo Installing dependencies for x64-windows-static...
"%VCPKG_ROOT%\vcpkg.exe" install sdl3:x64-windows-static zlib:x64-windows-static openal-soft:x64-windows-static libogg:x64-windows-static libvorbis:x64-windows-static glm:x64-windows-static freetype:x64-windows-static cpptrace:x64-windows-static curl:x64-windows-static

echo Done!
pause
