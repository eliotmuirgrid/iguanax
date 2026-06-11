
Where gcc.exe
if %errorlevel% neq 0 echo no msys or gcc && exit /b %errorlevel%

Where git.exe
if %errorlevel% neq 0 echo no git && exit /b %errorlevel%

Where ccache.exe
if %errorlevel% neq 0 echo no ccache && exit /b %errorlevel%

Where cmake.exe
if %errorlevel% neq 0 echo no cmake && exit /b %errorlevel%

Where cygcheck.exe
if %errorlevel% neq 0 echo no cygwin && exit /b %errorlevel%