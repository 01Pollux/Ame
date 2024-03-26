param(
    [String] $Os
)

Function Get-DxcZip {
    switch ($Os) {
        "Linux" {
            $DxcPath = "../Resources/linux_dxc_2023_08_14.x86_64.zip";
            break;
        }
        "Windows" {
            $DxcPath = "../Resources/dxc_2023_08_14.zip";
            break;
        }
        Default {
            Write-Error "Unsupported Operating System";
            Exit-PSHostProcess
        }
    }
    if (!(Test-Path $DxcPath)) {
        Write-Error "Missing '$([System.IO.Path]::GetFileName($DxcPath))'";
        Exit-PSHostProcess
    }
    return $DxcPath;
}

$CMakeListPath = "$PSScriptRoot/Dxc_CMakeLists.txt";
$DxcLibPath = "$PSScriptRoot/../Deps/Externals/DxC";

# Extract Dxc
$DxcZip = Get-DxcZip;
Expand-Archive -Force -Path $DxcZip -DestinationPath $DxcLibPath;
