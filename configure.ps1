param(
	[switch]$clean = $false
)

if ($clean -eq $true) {
	Write-Host "Cleaning Win64 build directory..."
	cmake --build ./build/Win64 --target clean
}
else {
	Write-Host "Configuring Win64 build..."
	cmake -S . -B ./build/Win64 -G "Visual Studio 17 2022" -A x64
}
