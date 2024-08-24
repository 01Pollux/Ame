
# for each file with .hlsli extension, parse it to C string and write it to a <name>.hlsli.hpp file
$files = Get-ChildItem -Path Source -Filter *.hlsli -Recurse
foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    $result = 'R"({0})"' -f $content
    $file_handle  = [System.IO.File]::Open($file.FullName + ".generated.hpp", [System.IO.FileMode]::Truncate)
    $stream_writer = New-Object System.IO.StreamWriter($file_handle)
    $stream_writer.Write($result)
    $stream_writer.Close()
    $file_handle.Close()
}