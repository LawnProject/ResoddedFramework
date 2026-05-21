#!/usr/bin/env pwsh

$vcpkgRoot = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg"
$projectRoot = "C:\Users\nydil\source\repos\ResoddedFramework2"

Write-Host "=== Descargando Ninja 1.13.1 ===" -ForegroundColor Cyan

# URL de descargas alternativas de Ninja
$ninjaUrl = "https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-win.zip"
$ninjaDownloadDir = "$env:TEMP\ninja-download"
$ninjaZip = "$ninjaDownloadDir\ninja-win-1.13.1.zip"

# Crear directorio si no existe
if (-not (Test-Path $ninjaDownloadDir)) {
    New-Item -ItemType Directory -Path $ninjaDownloadDir -Force | Out-Null
}

# Descargar Ninja usando otro método
Write-Host "Intentando descargar Ninja..." -ForegroundColor Yellow

try {
    # Usar curl si está disponible
    & curl -L -o $ninjaZip $ninjaUrl

    if (Test-Path $ninjaZip) {
        Write-Host "Ninja descargado correctamente" -ForegroundColor Green

        # Extraer
        Write-Host "Extrayendo Ninja..." -ForegroundColor Yellow
        Expand-Archive -Path $ninjaZip -DestinationPath $ninjaDownloadDir -Force

        # Copiar a vcpkg downloads
        $vcpkgDownloads = "$vcpkgRoot\downloads"
        if (-not (Test-Path $vcpkgDownloads)) {
            New-Item -ItemType Directory -Path $vcpkgDownloads -Force | Out-Null
        }

        Copy-Item "$ninjaDownloadDir\ninja.exe" "$vcpkgDownloads\ninja.exe" -Force
        Write-Host "Ninja copiado a vcpkg downloads" -ForegroundColor Green
    }
} catch {
    Write-Host "Error descargando Ninja: $_" -ForegroundColor Red
}

# Ahora ejecutar vcpkg
Push-Location $projectRoot

if (Test-Path "vcpkg_installed") {
    Write-Host "Eliminando cache anterior..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "vcpkg_installed"
}

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
