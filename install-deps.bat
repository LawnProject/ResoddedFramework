@echo off
REM Script para instalar dependencias de vcpkg sin problemas de PATH

setlocal enabledelayedexpansion

set VCPKG_ROOT=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg
set PROJECT_ROOT=C:\Users\nydil\source\repos\ResoddedFramework2

echo === Instalando dependencias de vcpkg ===
echo.

REM Limpiar PATH completamente - usar solo rutas necesarias
set PATH=C:\Windows\System32;C:\Windows;C:\Program Files\Git\cmd;%VCPKG_ROOT%

REM Cambiar a directorio del proyecto
cd /d "%PROJECT_ROOT%"

REM Limpiar cache anterior
if exist vcpkg_installed (
    echo Eliminando cache anterior...
    rmdir /s /q vcpkg_installed
    echo.
)

REM Ejecutar vcpkg install
echo Ejecutando vcpkg install...
echo Esto puede tomar varios minutos...
echo.

"%VCPKG_ROOT%\vcpkg.exe" install --triplet x64-windows-static

if %errorlevel% equ 0 (
    echo.
    echo === Instalacion completada exitosamente ===
) else (
    echo.
    echo === Error durante la instalacion ===
    echo Codigo de error: %errorlevel%
    pause
    exit /b %errorlevel%
)

pause
