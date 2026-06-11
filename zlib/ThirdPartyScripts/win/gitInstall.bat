REM An attempt to download git on windows the problem is the final installed product depends on the ~500MB mingw64 folder
REM @echo off
REM C:

REM if not exist C:\ProgramData\ThirdParty (
REM    mkdir C:\ProgramData\ThirdParty\installed
REM )

REM if exist C:\ProgramData\ThirdParty\GitSetup.log (
REM    rm C:\ProgramData\ThirdParty\GitSetup.log
REM )
REM set logfile=C:\ProgramData\ThirdParty\GitSetup.log

REM cd C:\ProgramData\ThirdParty

REM REM Clone, checkout, and install libgit2
REM echo setting up git...
REM if not exist C:\ProgramData\ThirdParty\GitInstaller.exe (
REM    curl -LJ https://github.com/git-for-windows/git/releases/download/v2.30.0.windows.2/Git-2.30.0.2-64-bit.exe -o GitInstaller.exe >> %logfile% 2>&1
REM )

REM if exist C:\ProgramData\ThirdParty\installed\git (
REM    rm -rf C:\ProgramData\ThirdParty\installed\git
REM )
REM md C:\ProgramData\ThirdParty\installed\git

REM GitInstaller.exe  /VERYSILENT /NORESTART /NOCANCEL /SP- /CLOSEAPPLICATIONS /COMPONENTS="icons,assoc,assoc_sh" /LOADINF=%~dp0\GitInstallInfo>> %logfile% 2>&1
REM if %errorlevel% neq 0 exit /b %errorlevel%

REM echo git ready.