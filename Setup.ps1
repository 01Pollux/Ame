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

if ($XMakeArgs -like "*vsxmake*") {
    # set directory as ./vsxmake2022
    Push-Location .\vsxmake2022

    dotnet sln add ..\Source\Scripting\AmeSharp\AmeSharp\ -s "Ame/Scripting"
    dotnet sln add ..\Source\Scripting\AmeSharp\Sample\ -s "Ame/Scripting"

    Pop-Location
}