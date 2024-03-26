param(
    [String] $Os
)

if ([String]::IsNullOrEmpty($Os)) {
    if ($IsLinux) {
        $Os = "Linux";
    }
    elseif ($IsWindows) {
        $Os = "Windows";
    }
}

if ([String]::IsNullOrEmpty($Os)) {
    Write-Host "Invalid Operating System";
}

Push-Location Scripts
Write-Host "Installing Dxc for $Os"
./Dxc.ps1 $Os
Pop-Location