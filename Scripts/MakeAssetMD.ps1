
param(
    [Parameter(Mandatory)][string] $Path,
    [string] $Root = "Assets/",
    [string] $Guid = [Guid]::NewGuid().ToString(),
    [string[]] $Dependencies # 'Guid1,Guid2,...'
)

Function Test-Guid
{
    param(
        [Parameter(Mandatory)][string]$Guid
    )

    $ObjectGuid = [System.Guid]::empty;
    if (![System.Guid]::TryParse($Guid,[System.Management.Automation.PSReference]$ObjectGuid))
    {
        Write-Error "Invalid guid was passed '$Guid'";
        Exit-PSHostProcess
    }
}

$DepList = '""';
Test-Guid $Guid
if ($Dependencies.Length -gt 0)
{
    $DepList = "";
    foreach ($ChildGuid in $Dependencies)
    {
        Test-Guid $ChildGuid;
        if ($DepList.Length -gt 0)
        {
            $DepList = "$DepList, $ChildGuid";
        }
        else
        {
            $DepList = "[ $ChildGuid";
        }
    }
    $DepList = "$DepList ]";
}

$Template = '{
    "Guid": "@Guid",
    "Path": "@Path.pamd",
    "LoaderData": "",
    "LoaderId": "1184421948659510",
    "Dependencies": @Dependencies,
    "Hash": "@Hash"
}';

$AssetPath = $Path.IndexOf($Root);
if ($AssetPath -le 0)
{
    Write-Error "Path '$Path' is not a subpath of '$Root'";
    Exit-PSHostProcess
}
else
{
    $RelPath = $Path.Substring($AssetPath + $Root.Length);
}

$Hash = Get-FileHash $Path -Algorithm SHA256 | Select-Object -ExpandProperty Hash
$Template = $Template.Replace("@Guid", $Guid);
$Template = $Template.Replace("@Path", $RelPath);
$Template = $Template.Replace("@Dependencies", $DepList);
$Template = $Template.Replace("@Hash", $Hash);

$Template | Out-File -FilePath "$Path.pamd"
