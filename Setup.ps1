param (
    [string]$XMakeArgs = "",
    [switch]$ShadersToCString = $false
)

if ($ShadersToCString) {
    Write-Host "Converting shaders to C string..."
    .\Project\ConfigureShaders.ps1
}

if ($XMakeArgs -ne "") {
    & xmake $XMakeArgs.Split(' ')
}