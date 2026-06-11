@echo off
C:

SET ORIGINDIR = %cd%

set PARTY_DIR=C:\ProgramData\ThirdParty
set INSTALL_DIR=%PARTY_DIR%\installed
set LIB_SRC_DIR=C:\ProgramData\ThirdParty\git
set LIB_INSTALL_DIR=C:\ProgramData\ThirdParty\installed\git

if not exist %PARTY_DIR% (
   mkdir %INSTALL_DIR%
)

@REM Download
echo Setting up git...

if not exist %LIB_INSTALL_DIR% (
   md %LIB_INSTALL_DIR%
   echo Download git installer
   curl -L -o "%LIB_INSTALL_DIR%/PortableGitInstaller.exe" "https://github.com/git-for-windows/git/releases/download/v2.38.1.windows.1/PortableGit-2.38.1-64-bit.7z.exe" 
   if %errorlevel% neq 0 exit /b %errorlevel%
)

echo Downloaded to %LIB_INSTALL_DIR%
echo Git ready.
