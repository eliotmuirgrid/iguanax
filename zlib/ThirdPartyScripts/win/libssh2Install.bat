@echo off
C:

if not exist C:\ProgramData\ThirdParty (
   mkdir C:\ProgramData\ThirdParty\installed
)

if exist C:\ProgramData\ThirdParty\Ssh2Setup.log (
   rm C:\ProgramData\ThirdParty\SSh2Setup.log
)
set logfile=C:\ProgramData\ThirdParty\Ssh2Setup.log
echo Logging to %logfile%...

cd C:\ProgramData\ThirdParty

REM Clone, checkout, and install libssh2
echo Setting up libssh2...
if not exist C:\ProgramData\ThirdParty\libssh2 (
   git clone https://github.com/libssh2/libssh2.git >> %logfile% 2>&1
   cd C:\ProgramData\ThirdParty\libssh2
   git checkout libssh2-1.9.0 >> %logfile% 2>&1
) else (
   cd C:\ProgramData\ThirdParty\libssh2
   git clean -dfx >> %logfile% 2>&1
)

if exist C:\ProgramData\ThirdParty\installed\libssh2 (
   rm -rf C:\ProgramData\ThirdParty\installed\libssh2
)
md C:\ProgramData\ThirdParty\installed\libssh2

md build
cd build

echo Configuring...
cmake .. -G "Unix Makefiles" -DBUILD_SHARED_LIBS=NO -DCMAKE_INSTALL_PREFIX="C:\ProgramData\ThirdParty\installed\libssh2" ^
-DCMAKE_PREFIX_PATH="C:\ProgramData\ThirdParty\installed\openssl" -DCRYPTO_BACKEND=OpenSSL ^
-DCMAKE_INSTALL_PREFIX="C:\ProgramData\ThirdParty\installed\libssh2" -DBUILD_EXAMPLES=OFF ^
-DBUILD_TESTING=OFF -DENABLE_ZLIB_COMPRESSION=ON ^
-DZLIB_LIBRARY="C:/ProgramData/ThirdParty/installed/zlib/lib/zlib.a" >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
cmake --build . --target install >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to C:\ProgramData\ThirdParty\installed\libssh2
echo libssh2 ready.
