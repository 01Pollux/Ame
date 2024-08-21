param (
    [string]$XMakeArgs = "",
    [switch]$ShadersToCString = $false
)

if ($ShadersToCString) {
    Write-Host "Converting shaders to C string..."
    .\Project\ConfigureShaders.ps1
}

# invoke xmake with the provided arguments
& xmake $XMakeArgs.Split(' ')