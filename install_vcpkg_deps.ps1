#!/usr/bin/env pwsh

# Establecer la variable VCPKG_ROOT correctamente
$env:VCPKG_ROOT = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg"

# Cambiar al directorio del proyecto
Push-Location "C:\Users\nydil\source\repos\ResoddedFramework2"

Write-Host "Instalando dependencias de vcpkg para x64-windows-static..." -ForegroundColor Green

# Ejecutar vcpkg install sin heredar PATH del sistema
$vcp = & {
    $env:PATH = ""
    & "$env:VCPKG_ROOT\vcpkg.exe" install --triplet x64-windows-static --recurse --verbose
}

Write-Host $vcp

Pop-Location
