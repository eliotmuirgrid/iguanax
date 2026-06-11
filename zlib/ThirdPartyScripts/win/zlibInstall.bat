@echo off
C:

if not exist C:\ProgramData\ThirdParty (
   mkdir C:\ProgramData\ThirdParty\installed
)

if exist C:\ProgramData\ThirdParty\ZlibSetup.log (
   rm C:\ProgramData\ThirdParty\ZlibSetup.log
)
set logfile=C:\ProgramData\ThirdParty\ZlibSetup.log
echo Logging to %logfile%

cd C:\ProgramData\ThirdParty

REM Download, extract, and build zlib
echo Setting up zlib...
if not exist C:\ProgramData\ThirdParty\zlib (
   md zlib
   curl http://www.zlib.net/zlib-1.2.11.tar.gz -o tmp.tar.gz >> %logfile% 2>&1
   tar -xf tmp.tar.gz -C zlib --strip-components=1
   del tmp.tar.gz
   cd C:\ProgramData\ThirdParty\zlib
) else (
   cd C:\ProgramData\ThirdParty\zlib
   rm -rf build
)

if exist C:\ProgramData\ThirdParty\installed\zlib (
   echo Removed existing zlib
   rm -rf C:\ProgramData\ThirdParty\installed\zlib
)
md C:\ProgramData\ThirdParty\installed\zlib

md build
cd build
echo Configuring...
cmake .. -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="C:\ProgramData\ThirdParty\installed\zlib" >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
cmake --build . --config Release --target install >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to C:\ProgramData\ThirdParty\installed\zlib

rem fix names so that linker can find the library as it expects (thirdparty libs and our code)
mv C:\ProgramData\ThirdParty\installed\zlib\lib\libzlibstatic.a C:\ProgramData\ThirdParty\installed\zlib\lib\zlib.a
cp C:\ProgramData\ThirdParty\installed\zlib\lib\zlib.a C:\ProgramData\ThirdParty\installed\zlib\lib\libz.a

echo zlib ready.
