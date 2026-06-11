@echo off
C:

set ORIGIN=%CD%

set PARTY_DIR=C:\ProgramData\ThirdParty
set INSTALL_DIR=%PARTY_DIR%\installed
set LIB_SRC_DIR=%PARTY_DIR%\zlib
set LIB_INSTALL_DIR=%INSTALL_DIR%\zlib

if not exist %PARTY_DIR% (
   mkdir %INSTALL_DIR%
)

cd %PARTY_DIR%

@REM Clone, checkout, build and install
echo Setting up zlib...
if not exist %LIB_SRC_DIR% (
   echo Cloning zlib repository
   git clone https://github.com/madler/zlib.git zlib
   cd %LIB_SRC_DIR%
) else (
   echo Cleaning zlib repository
   cd %LIB_SRC_DIR%
   git clean -dfx 
   echo Cleaned zlib repository
   git pull 
)
git checkout v1.2.13
echo Checked out zlib version

if exist %LIB_INSTALL_DIR% (
   echo Removed installed zlib
   rmdir /S /Q  %LIB_INSTALL_DIR%
)
md %LIB_INSTALL_DIR%

echo Configuring...
md build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="%LIB_INSTALL_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
cmake --build . --config Release --target install 
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to %LIB_INSTALL_DIR%
echo zlib ready.

cd %ORIGIN%