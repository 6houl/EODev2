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

$testProject = Join-Path $projectRoot 'tests\EODev.PacketTests.vcxproj'
& $msbuild $testProject '/t:Rebuild' '/p:Configuration=Release' '/p:Platform=Win32' '/m'
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& (Join-Path $projectRoot 'tests\build\EODev.PacketTests.exe')
exit $LASTEXITCODE
