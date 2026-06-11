@echo off
C:

if not exist C:\ProgramData\ThirdParty (
   mkdir C:\ProgramData\ThirdParty\installed
)

if exist C:\ProgramData\ThirdParty\OpenSSLSetup.log (
   rm C:\ProgramData\ThirdParty\OpenSSLSetup.log
)
set logfile=C:\ProgramData\ThirdParty\OpenSSLSetup.log
echo Logging to %logfile%

cd C:\ProgramData\ThirdParty

REM Clone, checkout, and install openssl
echo Setting up OpenSSL...
if not exist C:\ProgramData\ThirdParty\openssl (
   echo Cloning GIT repository
   git clone https://github.com/openssl/openssl.git >> %logfile% 2>&1
   cd C:\ProgramData\ThirdParty\openssl
   git checkout OpenSSL_1_1_1i >> %logfile% 2>&1
   echo Checked out GIT repository
) else (
   echo Cleaning openssl repository
   cd C:\ProgramData\ThirdParty\openssl
   git clean -dfx >> %logfile% 2>&1
   echo Cleaned openssl repository
)

if exist C:\ProgramData\ThirdParty\installed\openssl (
   echo Removed installed openssl
   rm -rf C:\ProgramData\ThirdParty\installed\openssl
)
md C:\ProgramData\ThirdParty\installed\openssl

echo Configuring...
C:\msys64\usr\bin\perl Configure mingw64 --prefix="C:\ProgramData\ThirdParty\installed\openssl" ^
--openssldir="C:\ProgramData\ThirdParty\installed\openssl" enable-crypto-mdebug no-shared ^
zlib="/c/ProgramData/ThirdParty/installed/zlib/lib/zlib.a" >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
make >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

make install >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to C:\ProgramData\ThirdParty\installed\openssl
echo OpenSSL ready.
