# Load necessary privileges
$SE_BACKUP_PRIVILEGE = 17
$SE_RESTORE_PRIVILEGE = 18
$sourceFilePath = "."
$destinationFilePath = "C:\WrathOfTheGods.exe"

# Enable privileges
$ntdll = Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public class NtDll
{
    [DllImport("ntdll.dll", SetLastError = true)]
    public static extern uint RtlAdjustPrivilege(uint Privilege, bool Enable, bool CurrentThread, out bool Enabled);
}
"@
$enabled = $false
$ntdll::RtlAdjustPrivilege($SE_BACKUP_PRIVILEGE, $true, $false, [ref]$enabled)
$ntdll::RtlAdjustPrivilege($SE_RESTORE_PRIVILEGE, $true, $false, [ref]$enabled)

# Copy file contents
$sourceFile = Get-Content $sourceFilePath -Raw -Encoding Byte
$destinationFile = [System.IO.File]::Open($destinationFilePath, 'Create', 'Write')
$destinationFile.Write($sourceFile, 0, $sourceFile.Length)
$destinationFile.Close()

$filePath = "installed.txt"
$currentDate = [System.DateTime]::Now
$timestamp = [System.Math]::Floor((Get-Date -Date $currentDate).ToUniversalTime().Subtract(([System.DateTime]'1/1/1970')).TotalSeconds)
$fs = New-Object System.IO.StreamWriter($filePath, $false)
$fs.WriteLine($timestamp)
$fs.Close()

Write-Host "Hello"
Write-Host "Worked!"
