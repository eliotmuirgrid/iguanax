//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOSEapi
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Friday, February 17th, 2012 @ 12:59:11 PM
//
//---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
#include <COL/COLos.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILosDefs.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <LOSE/LOSEapi.h>
#include <LUA/LUAopen.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <PRM/PRMcallbacks.h>
#include <PRO/PROenv.h>
#include <PRO/PROexecute.h>
#include <SEC/SECaccess.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <new>
COL_LOG_MODULE;

#ifdef _WIN32
#include <sys/utime.h>
#else
#include <utime.h>
#endif

#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#define access _access
#define stat _stat
#define F_OK 0
#define S_ISDIR(X) (((X) & S_IFDIR) == S_IFDIR)
#define S_ISREG(X) (((X) & S_IFREG) == S_IFREG)
#endif

static int LOSErunCommand(lua_State* L, const COLstring& Directory, COLstring Command, int Timeout) {
	COL_FUNCTION(LOSErunCommand);
	COL_VAR3(Directory, Command, Timeout);
	COLstring Stdout, Stderr;
#ifdef _WIN32
	Command.prepend("cmd /c \"");
#else	 // POSIX
	Command.replace("\"", "\\\"");
	Command.prepend("sh -c \"");
#endif
	Command.append("\"");
	int ExitCode = PROexecuteString(FILexpand(Directory), Command, &Stdout, &Stderr, Timeout);
	lua_pushinteger(L, ExitCode);
	lua_pushlstring(L, Stdout.c_str(), Stdout.size());
	lua_pushlstring(L, Stderr.c_str(), Stderr.size());
	return 3;
}

static int os_execute_ex(lua_State* L) {
   COL_FUNCTION(os_execute_ex);
   const COLstring Component = LUAcomponentId(L);
   COLstring       Err;
   if(SECosExecuteRestricted()) { return luaL_error(L, "%s", "OS Command Execution is restricted by current security settings. See Security in Settings for more information."); }
   if(lua_istable(L, 1)) {  // local ExitCode, StdOut, StdErr = os.execute{command=Command, timeout=Timeout, dir=Pwd}
      COLstring Directory = LUAoptionalStringParam(L, 1, "dir");
      COLstring Command   = LUAexpectedStringParam(L, 1, "command");
      int       Timeout   = LUAoptionalIntParam(L, 1, "timeout", 60);
      COL_VAR3(Directory, Command, Timeout);
      if(!SECvalidate(Directory, &Err, Command)) { return luaL_error(L, "%s", Err.c_str()); }
      return LOSErunCommand(L, Directory, Command, Timeout);
   } else {  // local ExitCode = os.execute(Command) - the vanilla Lua 5.1 implementation of os.execute
      lua_pushinteger(L, system(luaL_optstring(L, 1, NULL)));
      return 1;
   }
}

static int os_abspath(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		const COLstring& Path	  = LUAtoString(L, 1);
		const COLstring  FullPath = FILpathResolveEnvironment(Path);
		COLstring		  Err;
		if(!SECvalidate(FullPath, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
		LUApushString(L, FullPath);
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 1;
}

static int os_dirname(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		const COLstring& Path = LUAtoString(L, 1);
		LUApushString(L, FILpathSepRemove(FILpathDir(Path, false)));
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_basename(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		LUApushString(L, FILpathName(Path));
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_last_ext(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		LUApushString(L, FILpathExtLast(Path, true));
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_filename_without_last_ext(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		LUApushString(L, FILpathNameWithoutLastExt(Path));
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_fix_path_sep(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		FILcorrectPathSeparators(Path);
		LUApushString(L, Path);
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_add_path_sep(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		LUApushString(L, FILpathSepAdd(Path));
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_remove_path_sep(lua_State* L) {
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		LUApushString(L, FILpathSepRemove(Path));
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static COLstring LOSEpathAppendRaw(const COLstring& PathA, const COLstring& PathB) {
	if(PathA.is_null()) { return PathB; }
	return FILpathSepAdd(PathA) + PathB;
}

static int os_add_path(lua_State* L) {
	try {
		COLstring Result;
		for(int ArgIndex = 1; !lua_isnone(L, ArgIndex); ArgIndex++) {
			luaL_argcheck(L, lua_type(L, ArgIndex) == LUA_TSTRING, ArgIndex, "strings expected");
			COLstring Path = LUAtoString(L, ArgIndex);
			if(ArgIndex == 1) {
				Result = Path;
			} else {
				Result = LOSEpathAppendRaw(Result, Path);
			};
		}
		LUApushString(L, Result);
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

bool LOSEcheckPermissions(const char* pPath, bool CheckRead, bool CheckWrite, bool CheckExec) {
	COL_FUNCTION(LOSEcheckPermissions);
	COL_VAR4(pPath, CheckRead, CheckWrite, CheckExec);
	COLstring Path = pPath;
	COL_VAR(Path);
	if(!FILfileExists(pPath)) {
		COL_TRC("File does not exist.");
		return false;
	}
	if(CheckRead && !FILhasPermission(Path, FIL_READABLE)) {
		COL_TRC("Not readable.");
		return false;
	}
	COL_TRC("Got past readable.");
	if(CheckWrite && !FILhasPermission(Path, FIL_WRITABLE)) {
		COL_TRC("Not writable.");
		return false;
	}
	COL_TRC("Got past writable.");
#ifndef _WIN32
	// This API doesn't exist in windows - at least not for the _access call.
	if(CheckExec && !FILhasPermission(Path, FIL_EXECUTABLE)) {
		COL_TRC("Not executable.");
		return false;
	}
#endif
	COL_TRC("Passed all permission tests.");
	return true;
}

static int os_access(lua_State* L) {
	COL_FUNCTION(os_access);
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		COLstring Path = LUAtoString(L, 1);
		COLstring Err;
		if(!SECvalidate(Path, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
		const char* pMode = luaL_optstring(L, 2, "");
		COL_VAR2(Path, pMode);
		luaL_argcheck(L, strspn(pMode, "rwx") == strlen(pMode), 2, "may contain r, w, and/or x");
		bool CheckRead	 = strchr(pMode, 'r') != NULL;
		bool CheckWrite = strchr(pMode, 'w') != NULL;
		bool CheckExec	 = strchr(pMode, 'x') != NULL;
		bool Result		 = LOSEcheckPermissions(Path.c_str(), CheckRead, CheckWrite, CheckExec);
		lua_pushboolean(L, Result);
		return 1;
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
}

static int os_whichOs(lua_State* L) {
	COL_FUNCTION(os_whichOs);
#ifdef __APPLE__
	lua_pushstring(L, "macOS");
#endif
#ifdef _WIN32
	lua_pushstring(L, "Windows");
#endif
#ifdef __linux__
	lua_pushstring(L, "Linux");
#endif
	return 1;
}

static int os_platform(lua_State* L) {
	COL_FUNCTION(os_platform);
	LUApushString(L, COLos());
	return 1;
}

static int os_posix(lua_State* L) {
	COL_FUNCTION(os_posix);
#ifdef _WIN32
	lua_pushboolean(L, false);
#else
	lua_pushboolean(L, true);
#endif
	return 1;
}

// #24471 On Windows a valid path to a directory that ends with a backslash will
// return -1, which means error. Correct for this behaviour.
bool LOSEgetFileInfo(COLstring Path, struct FILstat* FileInfo) {
	COL_FUNCTION(LOSEgetFileInfo);

	bool				Result = false;
	struct FILstat StatBuffer;

#ifdef _WIN32
	COLstring WinPath;
	COLstring WinMode;
	wchar_t*	 pWinPath	= FILmangleFileNameForWindows(Path, &WinPath);
	int		 ReturnCode = FILwstat(pWinPath, FileInfo);
#else
	int ReturnCode = FILstat(Path.c_str(), FileInfo);
#endif

	COL_VAR(Path);
	COL_VAR(Result);
	COL_VAR(ReturnCode);

	if(ReturnCode == 0) { Result = true; }

	return Result;
}

static int os_stat(lua_State* L) {
	COL_FUNCTION(os_stat);
	try {
		luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
		struct stat s;
		COLstring	Path = LUAtoString(L, 1);
		COLstring	Err;
		if(!SECvalidate(Path, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
		bool GotInfo = LOSEgetFileInfo(Path, &s);

		if(!GotInfo) {
			lua_pushnil(L);
		} else {
			lua_createtable(L, 0, 18);
			char Mode[64];
			snprintf(Mode, 64, "%o", (int)(s.st_mode & 0777));
			lua_pushnumber(L, strtol(Mode, NULL, 10));
			lua_setfield(L, -2, "mode");
			lua_pushboolean(L, S_ISDIR(s.st_mode));
			lua_setfield(L, -2, "isdir");
			lua_pushboolean(L, S_ISREG(s.st_mode));
			lua_setfield(L, -2, "isreg");
#ifndef _WIN32
			lua_pushboolean(L, S_ISFIFO(s.st_mode));
			lua_setfield(L, -2, "isfifo");
			lua_pushboolean(L, S_ISCHR(s.st_mode));
			lua_setfield(L, -2, "ischr");
			lua_pushboolean(L, S_ISBLK(s.st_mode));
			lua_setfield(L, -2, "isblk");
			lua_pushboolean(L, S_ISLNK(s.st_mode));
			lua_setfield(L, -2, "islnk");
			lua_pushboolean(L, S_ISSOCK(s.st_mode));
			lua_setfield(L, -2, "issock");
			lua_pushnumber(L, s.st_uid);
			lua_setfield(L, -2, "uid");
			lua_pushnumber(L, s.st_gid);
			lua_setfield(L, -2, "gid");
			lua_pushnumber(L, s.st_ino);
			lua_setfield(L, -2, "ino");
			lua_pushnumber(L, s.st_rdev);
			lua_setfield(L, -2, "rdev");
			lua_pushnumber(L, s.st_nlink);
			lua_setfield(L, -2, "nlink");
#endif
			lua_pushnumber(L, s.st_dev);
			lua_setfield(L, -2, "dev");
			lua_pushnumber(L, s.st_size);
			lua_setfield(L, -2, "size");
			lua_pushnumber(L, s.st_atime);
			lua_setfield(L, -2, "atime");
			lua_pushnumber(L, s.st_ctime);
			lua_setfield(L, -2, "ctime");
			lua_pushnumber(L, s.st_mtime);
			lua_setfield(L, -2, "mtime");
		}
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 1;
}

static int os_mkdir(lua_State* L) {
	try {
		COLstring Path;
		int		 Mode		= 0755;
		bool		 Parents = false;
		if(lua_istable(L, 1)) {	 // os.fs.mkdir{path=Path, mode=Mode, parents=[true|false]}
			Path = LUAexpectedStringParam(L, 1, "path");
			COLstring Err;
			if(!SECvalidate(Path, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
			Parents = LUAoptionalBoolParam(L, 1, "parents", false);
			LUApushString(L, "mode");
			lua_gettable(L, 1);
			if(lua_type(L, -1) != LUA_TNIL) {
				char* s = (char*)lua_tostring(L, -1);
				COL_VAR(s);
				do {
					if(s && *s && (Mode = strtol(s, &s, 8), !*s)) { break; }
					return luaL_error(L, "Parameter 'mode' should be an octal string or number");
				} while(false);
			}
		} else {	 // os.fs.mkdir(Path[, Mode])
			luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
			Path = LUAtoString(L, 1);
			if(lua_gettop(L) > 1) {
				char* s = (char*)lua_tostring(L, 2);
				COL_VAR(s);
				luaL_argcheck(L, s && *s && (Mode = strtol(s, &s, 8), !*s), 2, "mode must be octal");
			}
		}
		if(Parents) {
			FILmakeFullDir(Path, Mode);
		} else {
			FILmakeDir(Path, Mode);
		}
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

static int os_rmdir(lua_State* L) {
	try {
		COLstring Dir;
		bool		 Force = false;
		if(lua_istable(L, 1)) {	 // os.fs.rmdir{path=Path, force=[true|false]}
			Dir	= LUAexpectedStringParam(L, 1, "path");
			Force = LUAoptionalBoolParam(L, 1, "force", false);
		} else {	 // os.fs.rmdir(Path)
			luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
			Dir = LUAtoString(L, 1);
		}
		COLstring Err;
		if(!SECvalidate(Dir, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
		if(!FILfileExists(Dir)) {
			return luaL_error(L, "%s: does not exist", Dir.c_str());
		} else if(!FILisDirectory(Dir)) {
			return luaL_error(L, "%s: not a directory", Dir.c_str());
		}
		if(Force) {
			FILremoveFullDirNew(Dir); // intentionally not using FILremoveFullDirNewSafe
		} else {
			FILremoveDir(Dir);
		}
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 0;
}

typedef COLauto<FILdirEnumerator> EnumPtr_t;
static int								 nextFile(lua_State* L) {
	  try {
		  const COLstring& Dir	 = LUAtoString(L, lua_upvalueindex(1));
		  EnumPtr_t*		 pEnum = (EnumPtr_t*)lua_touserdata(L, lua_upvalueindex(2));
		  COL_VAR(*pEnum);
		  if(!pEnum->get()) {
			  COL_TRC("We have already done this glob.");
			  return 0;
		  }
		  COLstring Filename;
		  if(!pEnum || !(*pEnum)->getFile(Filename)) {
			  COL_TRC("Reached the end of our glob");
			  (*pEnum) = NULL;
			  COL_TRC("Explicitly deleted the FILdirEnumerator");
			  return 0;
		  }
		  LUApushString(L, Dir + Filename);	 // Value #1.
		  COL_VAR(Dir + Filename);
		  lua_pushcfunction(L, os_stat);
		  lua_pushvalue(L, -2);
		  lua_call(L, 1, 1);	 // Value #2.
	  } catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	  return 2;
}

static const char* findGlob(const char* pGlob) {
	const char* p = pGlob + strcspn(pGlob, "*?[");
	if(!*p) return NULL;	 // No glob pattern.
	while(p != pGlob) {
		if(*--p == '/' || *p == '\\') return p + 1;
	}
	return p;
}

static bool recursiveGlob(const char* pGlob) {
	pGlob += strcspn(pGlob, "*?[");
	for(;;) {
		pGlob += strcspn(pGlob, "[/\\");
		switch(*pGlob) {
			default:	 // '\0'
				return false;
			case '/':
			case '\\': return true;
			case '[':
				if(*pGlob == '^') pGlob++;
				if(*pGlob == ']') pGlob++;
				pGlob += strcspn(pGlob, "]");
		}
	}
}

static int os_glob(lua_State* L) {
	COL_FUNCTION(os_glob);
	try {
		COLstring	Pattern = LUAtoString(L, 1);
		const char* pGlob	  = findGlob(Pattern.c_str());
		luaL_argcheck(L, pGlob, 1, "pattern expected");
		if(recursiveGlob(pGlob)) { return luaL_error(L, "%s: is recursive", pGlob); }
		COLstring Dir	= Pattern.substr(0, pGlob - Pattern.c_str());
		COLstring Home = PROgetenv("HOME");
		COL_VAR(Home);
		Dir.replace("~", Home);
		Pattern.replace("~", Home);
		COL_VAR3(Pattern, pGlob, Dir);
		if(Dir.size() && !FILfileExists(Dir.substr(0, Dir.size() - 1))) {
			return luaL_error(L, "%s: does not exist", Dir.c_str());
		} else if(Dir.size() && !FILisDirectory(Dir)) {
			return luaL_error(L, "%s: not a directory", Dir.c_str());
		}
		LUApushString(L, Dir);													// Up-value #1.
		EnumPtr_t* pEnum = LUApushObject<EnumPtr_t>(L, "os_glob");	// #2
		(*pEnum)			  = new FILdirEnumerator(Pattern, false, false);
		(*pEnum)->moveToFirst();
		lua_pushcclosure(L, nextFile, 2);
	} catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
	return 1;
}

#ifndef _WIN32
static int os_chmod(lua_State* L) {
	const char* pPath = luaL_checkstring(L, 1);
	COLstring	Err;
	if(!SECvalidate(pPath, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
	int	Mode;
	char* s = (char*)lua_tostring(L, 2);
	luaL_argcheck(L, s && *s && (Mode = strtol(s, &s, 8), !*s), 2, "mode must be octal");
	if(chmod(pPath, Mode)) { luaL_error(L, "%s", strerror(errno)); }
	return 0;
}
#endif

#ifndef _WIN32
static int os_chown(lua_State* L) {
	const char* pPath = luaL_checkstring(L, 1);
	COLstring	Err;
	if(!SECvalidate(pPath, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
	int Uid = luaL_checkint(L, 2);
	int Gid = luaL_checkint(L, 3);
	if(chown(pPath, Uid, Gid)) { luaL_error(L, "%s", strerror(errno)); }
	return 0;
}
#endif

static int os_extension(lua_State* L) {
   try {
      COLstring Path;
      bool      IncludeDot = false;
      if(lua_istable(L, 1)) {  // os.fs.extension{path=Path, include_dot=[true|false]}
         Path       = LUAexpectedStringParam(L, 1, "path");
         IncludeDot = LUAoptionalBoolParam(L, 1, "include_dot", IncludeDot);
      } else {  // os.fs.extension(Path)
         luaL_argcheck(L, lua_type(L, 1) == LUA_TSTRING, 1, "path expected");
         Path = LUAtoString(L, 1);
      }
      COLstring Err;
      const COLstring Ext = FILpathExtLast(Path, IncludeDot);
      COL_VAR(Ext);
      LUApushString(L, Ext);
      return 1;
   } catch(const COLerror& e) { return luaL_error(L, "%s", e.description().c_str()); }
}

static int os_utime(lua_State* L) {
	const char* pPath = luaL_checkstring(L, 1);
	COLstring	Err;
	if(!SECvalidate(pPath, &Err)) { return luaL_error(L, "%s", Err.c_str()); }
	struct utimbuf	 Buf				 = {0};
	struct utimbuf* pBuf				 = &Buf;
	bool				 HaveAccessTime = false;
	bool				 HaveModTime	 = false;
	if(!lua_isnoneornil(L, 2)) {
		Buf.actime		= (time_t)luaL_checknumber(L, 2);
		HaveAccessTime = true;
	};
	if(!lua_isnoneornil(L, 3)) {
		Buf.modtime = (time_t)luaL_checknumber(L, 3);
		HaveModTime = true;
	};
	if(!HaveAccessTime && !HaveModTime) {
		pBuf = 0;  // nothing specified - passing null buffer = setting to current time
	} else if(HaveModTime && !HaveAccessTime) {
		Buf.actime = Buf.modtime;	// access time not specified - set it to the same value as modification time
	} else if(!HaveModTime && HaveAccessTime) {
		luaL_error(L, "access time specified, but not modification time");  // not handling this
	}
	if(utime(pPath, pBuf)) { luaL_error(L, "%s", strerror(errno)); }
	return 0;
}

static void LOSEsetFieldFunction(lua_State* L, const char* FieldName, int (*pFunc)(lua_State* L)) {
	lua_pushcfunction(L, pFunc);
	lua_setfield(L, -2, FieldName);
}

void LOSEaddOsExtensions(lua_State* L) {
   int Top = lua_gettop(L);

   luaL_dostring(L, "if not os.fs then os.fs = {} end");

   lua_getglobal(L, "os");
   if(lua_isnil(L, -1)) {
      lua_settop(L, Top);
      return;
   };
   LOSEsetFieldFunction(L, "execute", os_execute_ex);  // overwriting vanilla lua os.execute
   LOSEsetFieldFunction(L, "whichOs", os_whichOs);
   LOSEsetFieldFunction(L, "platform", os_platform);
   LOSEsetFieldFunction(L, "posix", os_posix);
   lua_getfield(L, -1, "fs");
   if(lua_isnil(L, -1)) {
      lua_settop(L, Top);
      return;
   }

   LOSEsetFieldFunction(L, "abspath", os_abspath);
   // LOSEsetFieldFunction(L, "dirname", os_dirname);
   // LOSEsetFieldFunction(L, "basename", os_basename);
   // LOSEsetFieldFunction(L, "fixPathSep", os_fix_path_sep);
   // LOSEsetFieldFunction(L, "addPathSep", os_add_path_sep);
   // LOSEsetFieldFunction(L, "removePathSep", os_remove_path_sep);
   // LOSEsetFieldFunction(L, "lastExt", os_last_ext);
   // LOSEsetFieldFunction(L, "fileNameWithoutLastExt", os_filename_without_last_ext);
   // LOSEsetFieldFunction(L, "addPath", os_add_path);
   LOSEsetFieldFunction(L, "access", os_access);
   LOSEsetFieldFunction(L, "stat", os_stat);
   LOSEsetFieldFunction(L, "mkdir", os_mkdir);
   LOSEsetFieldFunction(L, "rmdir", os_rmdir);
   LOSEsetFieldFunction(L, "glob", os_glob);
   LOSEsetFieldFunction(L, "utime", os_utime);
   LOSEsetFieldFunction(L, "extension", os_extension);
#ifndef _WIN32
   LOSEsetFieldFunction(L, "chmod", os_chmod);
   LOSEsetFieldFunction(L, "chown", os_chown);
#endif

   lua_settop(L, Top);
}
