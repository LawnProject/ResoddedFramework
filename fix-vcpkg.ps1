#!/usr/bin/env pwsh

$vcpkgRoot = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg"
$projectRoot = "C:\Users\nydil\source\repos\ResoddedFramework2"

Write-Host "=== Reparando vcpkg ===" -ForegroundColor Cyan

if (-not (Test-Path "$vcpkgRoot\vcpkg.exe")) {
    Write-Host "Error: vcpkg.exe no encontrado" -ForegroundColor Red
    exit 1
}

Push-Location $projectRoot

if (Test-Path "vcpkg_installed") {
    Write-Host "Eliminando cache anterior..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "vcpkg_installed"
}

Write-Host "Estableciendo PATH limpio..." -ForegroundColor Yellow

$cleanPath = @(
    "C:\Windows\System32",
    "C:\Windows",
    "C:\Program Files\Git\cmd",
    "C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64",
    $vcpkgRoot
) -join ";"

$env:PATH = $cleanPath

Write-Host "Instalando dependencias..." -ForegroundColor Cyan

& "$vcpkgRoot\vcpkg.exe" install --triplet x64-windows-static

Pop-Location
