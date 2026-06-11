@echo off
C:

set ORIGINDIR = %CD%

set LIB_SRC_DIR=C:\ProgramData\ThirdParty\OpenSSL
set LIB_INSTALL_DIR=C:\ProgramData\ThirdParty\installed\OpenSSL

if not exist C:\ProgramData\ThirdParty\ (
   mkdir C:\ProgramData\ThirdParty\
)

cd C:\ProgramData\ThirdParty\

@REM Clone, checkout, build and install
echo Setting up OpenSSL
if not exist %LIB_SRC_DIR% (
   echo Cloning OpenSSL repository
   git clone https://github.com/openssl/openssl.git OpenSSL 
   cd %LIB_SRC_DIR%
) else (
   echo Cleaning OpenSSL repository
   cd %LIB_SRC_DIR%
   git clean -dfx 
   echo Cleaned OpenSSL repository
   git pull 
)
git checkout OpenSSL_1_1_1m

if exist %LIB_INSTALL_DIR% (
   echo Removed installed OpenSSL from %LIB_INSTALL_DIR%
   rmdir /S /Q %LIB_INSTALL_DIR%
)
md %LIB_INSTALL_DIR%

echo Configuring...
call perl Configure VC-WIN64A no-shared --prefix="%LIB_INSTALL_DIR%" --openssldir="%LIB_INSTALL_DIR%" 
echo Building...
nmake LIBS="libcrypto.lib libssl.lib" build_libs 
echo Installing...
nmake install_dev 
echo Installed to %LIB_INSTALL_DIR%
echo OpenSSL ready.

cd %ORIGINDIR%