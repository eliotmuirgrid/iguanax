@echo off
C:

set LIB_NAME=libcurl
set LIB_REPO=https://github.com/curl/curl.git
set LIB_VERSION=curl-7_86_0

set PARTY_DIR=C:\ProgramData\ThirdParty
set INSTALL_DIR=%PARTY_DIR%\installed
set LIB_SRC_DIR=%PARTY_DIR%\%LIB_NAME%
set LIB_INSTALL_DIR=%INSTALL_DIR%\%LIB_NAME%
set LIB_INSTALL_LOG=%PARTY_DIR%\%LIB_NAME%Setup.log


if not exist %PARTY_DIR% (
   mkdir %INSTALL_DIR%
)

if exist %LIB_INSTALL_LOG% (
   rm %LIB_INSTALL_LOG%
)
echo Logging to %LIB_INSTALL_LOG%

cd %PARTY_DIR%

@REM Clone, checkout, build and install
echo Setting up %LIB_NAME%...
if not exist %LIB_SRC_DIR% (
   echo Cloning %LIB_NAME% repository
   git clone %LIB_REPO% %LIB_NAME% >> %LIB_INSTALL_LOG% 2>&1
   cd %LIB_SRC_DIR%
) else (
   echo Cleaning %LIB_NAME% repository
   cd %LIB_SRC_DIR%
   git clean -dfx >> %LIB_INSTALL_LOG% 2>&1
   echo Cleaned %LIB_NAME% repository
   git pull >> %LIB_INSTALL_LOG% 2>&1
)
git checkout %LIB_VERSION% >> %LIB_INSTALL_LOG% 2>&1
echo Checked out commit %LIB_VERSION%

if exist %LIB_INSTALL_DIR% (
   echo Removed installed %LIB_NAME%
   rm -rf %LIB_INSTALL_DIR%
)
md %LIB_INSTALL_DIR%

@REM curl uses CMake's built-in FindZLIB and FindOpenSSL function to find the libraries,
@REM so we only need to provide hints (OPENSSL_ROOT_DIR and ZLIB_ROOT) about library location.
@REM the "Findlibssh2" is in CMake/FindLibSSH2.cmake, which doesn't support non-default location,
@REM so we have to hard code the include dir and library file in LIBSSH2_INCLUDE_DIR and LIBSSH2_LIBRARY
echo Configuring...
md build
cd build
cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%LIB_INSTALL_DIR%" ^
-DCURL_USE_OPENSSL=ON -DOPENSSL_USE_STATIC_LIBS=ON -DOPENSSL_ROOT_DIR="%INSTALL_DIR%\OpenSSL" ^
-DZLIB_USE_STATIC_LIBS=ON -DZLIB_ROOT="%INSTALL_DIR%\zlib" ^
-DLIBSSH2_INCLUDE_DIR="%INSTALL_DIR%\libssh2\include" -DLIBSSH2_LIBRARY="%INSTALL_DIR%\libssh2\lib\libssh2.lib" >> %LIB_INSTALL_LOG% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
cmake --build . --config Release --target install >> %LIB_INSTALL_LOG% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to %LIB_INSTALL_DIR%
echo %LIB_NAME% ready.
