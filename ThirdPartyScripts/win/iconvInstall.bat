@echo off
C:

if not exist C:\ProgramData\ThirdParty (
   mkdir C:\ProgramData\ThirdParty\installed
)

if exist C:\ProgramData\ThirdParty\iconvSetup.log (
   rm C:\ProgramData\ThirdParty\iconvSetup.log
)
set logfile=C:\ProgramData\ThirdParty\iconvSetup.log
echo Logging to %logfile%...

cd C:\ProgramData\ThirdParty

REM Download, extract, and build iconv
echo Setting up iconv...
if not exist C:\ProgramData\ThirdParty\iconv (
   md iconv
   curl https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz -o tmp.tar.gz >> %logfile% 2>&1
   tar -xf tmp.tar.gz -C iconv --strip-components=1
   del tmp.tar.gz
   cd C:\ProgramData\ThirdParty\iconv
) else (
   cd C:\ProgramData\ThirdParty\iconv
   rm -rf build
)

if exist C:\ProgramData\ThirdParty\installed\iconv (
   rm -rf C:\ProgramData\ThirdParty\installed\iconv
)
md C:\ProgramData\ThirdParty\installed\iconv


echo Configuring...
sh ./configure --host=x86_64-w64-mingw32 --prefix="C:/ProgramData/ThirdParty/installed/iconv"^
 --exec-prefix="C:/ProgramData/ThirdParty/installed/iconv" --enable-static=yes --disable-shared RANLIB=":" >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

sed -i '22,23d' C:\ProgramData\ThirdParty\iconv\libcharset\lib\relocatable-stub.c

echo Building...
make >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

make install >> %logfile% 2>&1
if %errorlevel% neq 0 exit /b %errorlevel%

echo Installed to C:\ProgramData\ThirdParty\installed\iconv
echo iconv ready.
