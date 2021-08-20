param([String]$psyq_path4)

$psyq_path = "E:\Data\mgs\psyq\psyq_4.6\bin"

$ErrorActionPreference = "Stop"

Push-Location $PSScriptRoot

$oldPathEnv = $Env:path 

Function psyq_setup($psyq_path)
{
    # Setup PSYQ ini
    $psyq_path_without_bin = $psyq_path
    if ($psyq_path_without_bin.EndsWith("\bin\", "CurrentCultureIgnoreCase"))
    {
        $psyq_path_without_bin = $psyq_path_without_bin.Substring(0, $psyq_path_without_bin.Length - 5)
    }
    elseif ($psyq_path_without_bin.EndsWith("\bin", "CurrentCultureIgnoreCase"))
    {
        $psyq_path_without_bin = $psyq_path_without_bin.Substring(0, $psyq_path_without_bin.Length - 4)
    }


    $Env:PSX_PATH = $psyq_path
    $Env:LIBRARY_PATH = "$psyq_path_without_bin\lib"
    $Env:C_PLUS_INCLUDE_PATH = "$psyq_path_without_bin\include"
    $Env:C_INCLUDE_PATH = "$psyq_path_without_bin\include"
    $Env:PSYQ_PATH = $psyq_path

    $Env:COMPILER_PATH = $psyq_path
    $Env:ASSEMBLER_PATH = $psyq_path
    $Env:LINKER_PATH = $psyq_path

    $Env:path = $oldPathEnv + ";" + $psyq_path

    Write-Host $Env:PSX_PATH
    Write-Host $Env:LIBRARY_PATH
    Write-Host $Env:C_PLUS_INCLUDE_PATH
    Write-Host $Env:C_INCLUDE_PATH
    Write-Host $Env:PSYQ_PATH

    Write-Host $Env:COMPILER_PATH
    Write-Host $Env:ASSEMBLER_PATH
    Write-Host $Env:LINKER_PATH

    (Get-Content $psyq_path\psyq.ini.template) | 
    Foreach-Object {$_ -replace '\$PSYQ_PATH',$psyq_path_without_bin}  | 
    Out-File $psyq_path\psyq.ini
}

Write-Host "Enable psyq 4.4" -ForegroundColor "DarkMagenta" -BackgroundColor "Black"
psyq_setup($psyq_path)

del main.cpe -ea silentlycontinue
del main.exe -ea silentlycontinue
del main.cpe -ea silentlycontinue
del *.obj -ea silentlycontinue

$testOnly = $false

$defineTest = ""
if ($testOnly -eq $true)
{
    $defineTest = "-DTEST_ONLY"
}

ccpsx -O2 -G 8 -g -c -Wall $defineTest "seqplayer.cpp" "-oseqplayer.obj" -I $Env:C_INCLUDE_PATH
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

ccpsx -O2 -G 8 -g -c -Wall $defineTest "main.cpp" "-omain.obj" -I $Env:C_INCLUDE_PATH
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

ccpsx -O2 -G 8 -g -c -Wall $defineTest "lib_snd.cpp" "-olib_snd.obj" -I $Env:C_INCLUDE_PATH
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

ccpsx -O2 -G 8 -g -c -Wall $defineTest "lib_spu.cpp" "-olib_spu.obj" -I $Env:C_INCLUDE_PATH
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

ccpsx -O2 -c -Wall $defineTest "vs_vtc.c" "-oVS_VTC.obj" -I $Env:C_INCLUDE_PATH
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

ccpsx -O2 -c -Wall $defineTest "vs_vt.c" "-oVS_VT.obj" -I $Env:C_INCLUDE_PATH
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

#pause

psylink.exe /m /wl /wm /c /l $Env:LIBRARY_PATH "@$PSScriptRoot\linker_command_file.txt",$PSScriptRoot\main.cpe,$PSScriptRoot\main.sym,$PSScriptRoot\main.map
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

cpe2exe main.cpe
if ($lastExitCode -ne 0) {Write-Error "Command exited with code " $lastExitCode}

del iso\main.exe -ea silentlycontinue
del iso.cue -ea silentlycontinue
del iso.bin -ea silentlycontinue
copy main.exe iso\main.exe

if ($testOnly -eq $false)
{
    Write-Host "Build iso..."
    psxbuild -c iso.cat
}

Write-Host "Emu boot.."
if ($testOnly -eq $false)
{
    invoke-expression 'cmd /c start powershell -Command { Start-Process "C:\Users\paul\Downloads\duckstation-windows-x64-release (1)\duckstation-qt-x64-ReleaseLTCG.exe" -WorkingDirectory "C:\Users\paul\Downloads\duckstation-windows-x64-release (1)" -ArgumentList "-batch", "E:\Data\alive\reversing\sound_rev\psx_seq_player\iso.cue" }'
}
else
{
    C:\Users\paul\Downloads\pcsx-redux-nightly-4530.20210713.2-x64\pcsx-redux.exe -run -bios "C:\Users\paul\Downloads\pcsx-redux-nightly-4530.20210713.2-x64\openbios.bin" -stdout -testmode -loadexe E:\Data\alive\reversing\sound_rev\psx_seq_player\main.exe
}