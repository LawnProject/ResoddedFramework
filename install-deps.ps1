#!/usr/bin/env pwsh

$vcpkgRoot = "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\vcpkg"
$projectRoot = "C:\Users\nydil\source\repos\ResoddedFramework2"

Write-Host "=== Buscando Ninja instalado ===" -ForegroundColor Cyan

# Buscar ninja en PATH del sistema
$ninjaPath = (Get-Command ninja -ErrorAction SilentlyContinue).Source
if ($ninjaPath) {
    Write-Host "Ninja encontrado en: $ninjaPath" -ForegroundColor Green
    $ninjaVersion = & ninja --version
    Write-Host "Versión: $ninjaVersion" -ForegroundColor Green
} else {
    Write-Host "Ninja no encontrado en PATH" -ForegroundColor Yellow

    # Buscar en rutas comunes
    $ninjaSearchPaths = @(
        "C:\Program Files\ninja\ninja.exe",
        "C:\tools\ninja\ninja.exe",
        "C:\bin\ninja.exe"
    )

    foreach ($path in $ninjaSearchPaths) {
        if (Test-Path $path) {
            Write-Host "Ninja encontrado en: $path" -ForegroundColor Green
            $ninjaPath = $path
            break
        }
    }
}

if ($ninjaPath) {
    Write-Host "Usando Ninja: $ninjaPath" -ForegroundColor Green
} else {
    Write-Host "Ninja no encontrado. Descargándolo manualmente..." -ForegroundColor Yellow

    # Crear directorio para ninja
    $ninjaDir = "C:\tools\ninja"
    if (-not (Test-Path $ninjaDir)) {
        New-Item -ItemType Directory -Path $ninjaDir -Force | Out-Null
    }

    # Intentar descargar desde otra fuente
    $ninjaUrl = "https://github.com/ninja-build/ninja/releases/download/v1.13.1/ninja-win.zip"
    $ninjaZip = "$ninjaDir\ninja-1.13.1.zip"

    Write-Host "Descargando desde: $ninjaUrl" -ForegroundColor Cyan

    try {
        [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
        $client = New-Object System.Net.WebClient
        $client.DownloadFile($ninjaUrl, $ninjaZip)

        if (Test-Path $ninjaZip) {
            Write-Host "Descarga completada" -ForegroundColor Green
            Expand-Archive -Path $ninjaZip -DestinationPath $ninjaDir -Force
            $ninjaPath = "$ninjaDir\ninja.exe"
            Write-Host "Ninja extraído en: $ninjaPath" -ForegroundColor Green
        }
    } catch {
        Write-Host "Error en descarga: $_" -ForegroundColor Red
        Write-Host "Continuando con la instalación de vcpkg..." -ForegroundColor Yellow
    }
}

# Preparar variables de entorno
$env:PATH = "C:\Windows\System32;C:\Windows;C:\Program Files\Git\cmd;$vcpkgRoot;" + $env:PATH

Push-Location $projectRoot

if (Test-Path "vcpkg_installed") {
    Write-Host "Eliminando cache anterior..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "vcpkg_installed"
}

Write-Host "Instalando dependencias con vcpkg..." -ForegroundColor Cyan
Write-Host "Esto puede tomar varios minutos..." -ForegroundColor Yellow

& "$vcpkgRoot\vcpkg.exe" install --triplet x64-windows-static

if ($LASTEXITCODE -eq 0) {
    Write-Host "Instalacion completada exitosamente" -ForegroundColor Green
} else {
    Write-Host "Error en la instalacion. Codigo: $LASTEXITCODE" -ForegroundColor Red
}

Pop-Location
