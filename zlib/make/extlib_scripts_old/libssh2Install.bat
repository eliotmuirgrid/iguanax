@echo off
C:

set ORIGINDIR = %CD%

set LIB_SRC_DIR=C:\ProgramData\ThirdParty\libssh2
set LIB_INSTALL_DIR=C:\ProgramData\ThirdParty\installed\libssh2


if not exist C:\ProgramData\ThirdParty\installed (
   mkdir C:\ProgramData\ThirdParty\installed
)
cd C:\ProgramData\ThirdParty\

@REM Clone, checkout, build and install
echo Setting up libssh2..
if not exist %LIB_SRC_DIR% (
   echo Cloning libssh2 repository
   git clone https://github.com/libssh2/libssh2.git libssh2
   cd %LIB_SRC_DIR%
) else (
   echo Cleaning libssh2 repository
   cd %LIB_SRC_DIR%
   git clean -dfx 
   echo Cleaned libssh2 repository
   git pull
)
git checkout libssh2-1.10.0
echo Checked out version

if exist %LIB_INSTALL_DIR% (
   echo Removed installed libssh2
   rmdir /S /Q %LIB_INSTALL_DIR%
)
md %LIB_INSTALL_DIR%

echo Configuring...
md build
cd build
@echo %INSTALL_DIR%\OpenSSL
@REM -DENABLE_ZLIB_COMPRESSION=ON -DZLIB_LIBRARY="%INSTALL_DIR%\zlib\lib\zlibstatic.lib" ^
cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%LIB_INSTALL_DIR%" ^
-DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF ^
-DCRYPTO_BACKEND=OpenSSL -DOPENSSL_USE_STATIC_LIBS=ON -DOPENSSL_ROOT_DIR="C:\ProgramData\ThirdParty\installed\OpenSSL" 
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
cmake --build . --config Release --target install 
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to %LIB_INSTALL_DIR%
echo libssh2 ready.
cd %ORIGINDIR%