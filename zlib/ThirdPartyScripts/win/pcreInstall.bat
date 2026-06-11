@echo off
C:

if not exist C:\ProgramData\ThirdParty (
   mkdir C:\ProgramData\ThirdParty\installed
)

if exist C:\ProgramData\ThirdParty\PcreSetup.log (
   rm C:\ProgramData\ThirdParty\PcreSetup.log
)
set logfile=C:\ProgramData\ThirdParty\PcreSetup.log
echo Logging to %logfile%

cd C:\ProgramData\ThirdParty

REM Download, extract, and install pcre2 for libgit2 to avoid symbol collision with our pcre
echo setting up pcre...
if not exist C:\ProgramData\ThirdParty\pcre (
   md C:\ProgramData\ThirdParty\pcre
   curl https://ftp.pcre.org/pub/pcre/pcre-8.36.tar.gz -o tmp.tar.gz >> %logfile% 2>&1
   tar -xf tmp.tar.gz -C pcre --strip-components=1
   del tmp.tar.gz
   cd C:\ProgramData\ThirdParty\pcre
) else (
   cd C:\ProgramData\ThirdParty\pcre
   rm -rf build
)

if exist C:\ProgramData\ThirdParty\installed\pcre (
   echo Removing existing pcre folder
   rm -rf C:\ProgramData\ThirdParty\installed\pcre
)
md C:\ProgramData\ThirdParty\installed\pcre

md build
cd build

echo Configuring...
cmake .. -G "Unix Makefiles" -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX="C:\ProgramData\ThirdParty\installed\pcre" ^
-DPCRE_BUILD_TESTS=OFF -DZLIB_LIBRARY="C:\ProgramData\ThirdParty\installed\zlib\lib\zlib.a" ^
-DPCRE_BUILD_PCREGREP=OFF >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Building...
cmake --build . --config Release --target install >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to C:\ProgramData\ThirdParty\installed\pcre
echo pcre ready.
