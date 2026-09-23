param(
    [switch]$BuildOnly
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'

if (-not (Test-Path -LiteralPath $vswhere)) {
    throw 'Visual Studio Installer was not found.'
}

$msbuild = & $vswhere -latest -version '[18.0,19.0)' -products * -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (-not $msbuild) {
    $msbuild = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
}
if (-not $msbuild) {
    throw 'MSBuild was not found.'
}

$requiredFiles = @(
    'lib\ptypes.lib',
    'third_party\sfml\SFML-2.5.1\lib\sfml-graphics-s.lib',
    'third_party\sfml\SFML-2.5.1\lib\sfml-window-s.lib',
    'third_party\sfml\SFML-2.5.1\lib\sfml-system-s.lib',
    'third_party\sfml\SFML-2.5.1\bin\openal32.dll'
)

foreach ($relativePath in $requiredFiles) {
    $fullPath = Join-Path $projectRoot $relativePath
    if (-not (Test-Path -LiteralPath $fullPath)) {
        throw "Missing build dependency: $relativePath"
    }
}

$target = if ($BuildOnly) { 'Build' } else { 'Rebuild' }
$solution = Join-Path $projectRoot 'EoDEV++.sln'

& $msbuild $solution "/t:$target" '/p:Configuration=Release' '/p:Platform=Win32' '/m'
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}
