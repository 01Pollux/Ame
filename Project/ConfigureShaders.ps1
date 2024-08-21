
# for each file with .hlsli extension, parse it to C string and write it to a <name>.hlsli.hpp file
$files = Get-ChildItem -Path Source -Filter *.hlsli -Recurse
foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    $result = 'R"({0})"' -f $content
    $result | Set-Content -Path ($file.FullName + ".generated.hpp")
}