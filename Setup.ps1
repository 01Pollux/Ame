param (
    [string]$XMakeArgs = "",
    [switch]$ShadersToCString = $false,
    [switch]$BuildAmeSharpRuntime = $false
)

#

if ($ShadersToCString) {
    Write-Host "Converting shaders to C string..."
    .\Project\ConfigureShaders.ps1
}

if ($XMakeArgs -ne "") {
    & xmake $XMakeArgs.Split(' ')
}

if ($BuildAmeSharpRuntime) {
    Write-Host "Building C#..."
    dotnet publish .\Source\Scripting\CSharpRuntime\AmeSharp.Runtime -o ./install/dotnet/AmeSharp.Runtime
}