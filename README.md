# leccion-simulacion

To create profile:

New-Item -Path $PROFILE -Type File -Force
notepad $PROFILE
function Start-DevEnv {
    & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1" -Arch amd64 -SkipAutomaticLocation
}

# Optional: create a shorter alias for the function
Set-Alias dev Start-DevEnv

This is a write example
