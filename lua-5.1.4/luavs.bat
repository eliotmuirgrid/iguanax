@rem Script to build Lua under "Visual Studio .NET Command Prompt".
@rem It creates lua51.dll, lua51.lib, lua.exe, and luac.exe in src.
@rem (contributed by David Manura and Mike Pall)

@setlocal
@rem # Added /TP /EHsc for C++ compilation. -Eric
@rem # Used /MT instead of /MD, as the latter caused crashes. -Eric
@set MYCOMPILE=cl /nologo /MT /O2 /W3 /c /D_CRT_SECURE_NO_DEPRECATE /TP /EHsc
@set MYLINK=link /nologo
@set MYMT=mt /nologo

cd src
%MYCOMPILE% /DLUA_BUILD_AS_DLL l*.c ifware.c
del lua.obj luac.obj
%MYLINK% /DLL /out:lua51.dll l*.obj ifware.obj
if exist lua51.dll.manifest^
  %MYMT% -manifest lua51.dll.manifest -outputresource:lua51.dll;2
%MYCOMPILE% /DLUA_BUILD_AS_DLL lua.c
%MYLINK% /out:lua.exe lua.obj lua51.lib
if exist lua.exe.manifest^
  %MYMT% -manifest lua.exe.manifest -outputresource:lua.exe
%MYCOMPILE% l*.c print.c
del lua.obj linit.obj lbaselib.obj ldblib.obj liolib.obj lmathlib.obj^
    loslib.obj ltablib.obj lstrlib.obj loadlib.obj
%MYLINK% /out:luac.exe *.obj
if exist luac.exe.manifest^
  %MYMT% -manifest luac.exe.manifest -outputresource:luac.exe
del *.obj *.manifest
cd ..
