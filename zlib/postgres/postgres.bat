rem Go to the postgres build directory
cd postgres\src\tools\msvc
rem Super hack - we have to change the postgres build from using dynamic linking of VCRUNTIME to static linking
rem Only way to do this was to modify Project.pm with SED.  This might break in the future <shrug!> IE-1860
sed 's/MultiThreadedDLL/MultiThreaded/g' MSBuildProject.pm > MSBuildProject.mod.pm
mv MSBuildProject.mod.pm MSBuildProject.pm
rem Remove cygwin tools from path to avoid using cygwib perl - use activestate perl instead - do it now after we have used SED
set PATH=%PATH:C:\cygwin64\bin;=%
rem Set mydir to local dir
SET mydir=%~dp0
@perl build.pl
@perl install.pl %mydir%winPostgres
rem Put cygwin back on path
set PATH=%PATH%;C:\cygwin64\bin;