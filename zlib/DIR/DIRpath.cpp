// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRpath
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 24 March 2023 - 03:39PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRexpandPath.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

COLstring DIRcheckAboveRoot(const COLstring& Source, const COLstring& RelativeFilePath) {
	COL_FUNCTION(DIRcheckAboveRoot);
	COL_VAR2(Source, RelativeFilePath);
	COLstring Result = FILpathAppend(Source, RelativeFilePath);
	if(Result.find(Source) != npos) { COL_TRC("Path resolved to: " << Result); }
	else {
		COL_TRC("Trying to access above the component's configured directory.");
		Result = "";
	}
	COL_VAR(Result);
	return Result;
}

COLstring DIRconfigRootDirDefault() {
   COLstring Result;
#ifdef _WIN32
   Result = "C:\\ProgramData\\IguanaX";
#elif __linux__
   // TODO - if we run as root then make the default be /var/lib/
   // Need to look HOME variable
   Result = "~/.IguanaX";
#elif __APPLE__
   Result = "~/Library/IguanaX";
#else
   COLMSGPRECONDITION(false, "Unsupported Operating System.");
#endif
   Result = DIRexpandPath(Result); // determines the first ~ expansion
   COL_VAR(Result);
   Result = FILpathResolveEnvironment(Result);
   return Result;
}

COLstring DIRconfigDirDefault() {
   COLstring ConfigDir = DIRconfigRootDirDefault();
   FILaddPathSeparator(ConfigDir);
   return ConfigDir;
}

COLstring DIRcomponentDir(const COLstring& ComponentId){
   COL_FUNCTION(DIRcomponentDir);
   COLstring Result = FILpathAppend(DIRcompsDir(), ComponentId);
   FILaddPathSeparator(Result);
   return Result;
}

COLstring DIRconfigUserDir(const COLstring& Username){ 
   COL_FUNCTION(DIRconfigUserDir);
   COLstring Dir = DIRconfigFile("users/" + Username + "/");
   COL_VAR(Dir);
   return Dir;
}

COLstring DIRconfigUserSshFile(const COLstring& Username){
   COL_FUNCTION(DIRconfigUserSshFile);
   COLstring File = FILpathAppend(DIRconfigUserDir(Username), "privatekey");
   // We need to do this since git expects posix style file separators when reading gitconfig files
   File = FILpathSepToPosix(File);
   return File;
}

COLstring DIRconfigUserFile(const COLstring& Username){
   COL_FUNCTION(DIRconfigUserFile);
	return FILpathAppend(DIRconfigUserDir(Username), "gitconfig");
}

COLstring DIRconfigUserGitFile(const COLstring& Username){
   COL_FUNCTION(DIRconfigUserGitFile);
	return FILpathAppend(DIRconfigUserDir(Username), "git.json");
}

COLstring DIRconfigUserOwnersFile(const COLstring& Username){
   COL_FUNCTION(DIRconfigUserOwnersFile);
   return FILpathAppend(DIRresourcesDir(), "users/" + Username + "/owners.json");
}

COLstring DIRconfigUserOrgsFile(const COLstring& Username){
   COL_FUNCTION(DIRconfigUserOrgsFile);
   return FILpathAppend(DIRconfigUserDir(Username), "organizations.json");
}

COLstring DIRcomponentConfig(const COLstring& ComponentId){
   COL_FUNCTION(DIRcomponentConfig);
	return FILpathAppend(DIRcomponentDir(ComponentId), "component.json");
}

// TODO - check for above root attacks
COLstring DIRcomponentDevDir(const COLstring& ComponentId){
   COL_FUNCTION(DIRcomponentDevDir);
	return FILpathAppend(DIRcomponentDir(ComponentId), "dev/");
}

COLstring DIRcomponentSampleDir(const COLstring& ComponentId){
   COL_FUNCTION(DIRcomponentSampleDir);
	return FILpathAppend(DIRcomponentDevDir(ComponentId), "Samples/");
}

// TODO - check for above root attacks
COLstring DIRcomponentRunDir(const COLstring& ComponentId){
   COL_FUNCTION(DIRcomponentRunDir);
   return FILpathAppend(FILworkingDir(), FILpathAppend("run", ComponentId + "/"));
}

COLstring DIRcomponentProjectDir(const COLstring& ComponentId, bool IsDev){
   COL_FUNCTION(DIRcomponentProjectDir);
   COL_VAR2(ComponentId, IsDev);
   COLstring Dir;
   if (IsDev){
      Dir = DIRcomponentDevDir(ComponentId);
   } else {
      Dir = DIRcomponentRunDir(ComponentId);
   }
   COL_VAR(Dir);
   return Dir;
}

COLstring DIRcompsDir()     { return DIRconfigFile("comps/"); }
COLstring DIRusersDir()     { return DIRconfigFile("users/"); }
COLstring DIRotherDir()     { return DIRconfigFile("other/"); }
COLstring DIRresourcesDir() { return FILpathAppend(FILworkingDir(), "resources/"); }
COLstring DIRcrashDir()     { return FILpathAppend(DIRresourcesDir(), "reports/"); }
COLstring DIRconfigDir()    { return FILpathAppend(FILworkingDir(), "config/"); }

COLstring DIRconfigFile(const COLstring& Path) {
   COL_FUNCTION(DIRconfigFile);
   COLstring FileName = FILpathAppend(DIRconfigDir(), Path);
   COL_VAR2(FileName, Path);
   FILcorrectPathSeparators(FileName);
   return FileName;
}

// old format file
COLstring DIRfileConfigUsers() { return FILpathAppend(FILworkingDir(), "configurations/users/users.json"); }

COLstring DIRsettingsDir()             { return DIRconfigFile("settings/");                      }
COLstring DIRfileConfigEncrypt()       { return DIRconfigFile("other/.encrypt");                 }
COLstring DIRfileConfigLicense()       { return DIRconfigFile("other/IguanaLicense");            }
COLstring DIRfileConfigWebCert()       { return DIRconfigFile("other/webcert.pem");              }
COLstring DIRfileConfigWebKey()        { return DIRconfigFile("other/webkey.pem");               }
COLstring DIRfileConfigTunnelKey()     { return DIRconfigFile("other/clusterprivatekey");        }
COLstring DIRfileConfigDatabase()      { return DIRconfigFile("settings/database.json");         }
COLstring DIRfileConfigGitCatalogue()  { return DIRconfigFile("settings/catalogue.json");        }
COLstring DIRfileConfigServers()       { return DIRconfigFile("settings/servers.json");          }
COLstring DIRfileConfigNotifications() { return DIRconfigFile("settings/notifications.json");    }
COLstring DIRfileConfigRoles()         { return DIRconfigFile("settings/roles.json");            }
COLstring DIRfileConfigWeb()           { return DIRconfigFile("settings/web.json");              }
COLstring DIRfileConfigLogging()       { return DIRconfigFile("settings/logging.json");          }
COLstring DIRfileConfigLog()           { return DIRconfigFile("settings/log.json");              }
COLstring DIRfileConfigAutoUpgrade()   { return DIRconfigFile("settings/auto_upgrade.json") ;    }
COLstring DIRfileConfigEnvironment()   { return DIRconfigFile("settings/environment.txt");       }
COLstring DIRfileConfigInstance()      { return DIRconfigFile("settings/instance.json");         }
COLstring DIRfileConfigSecurity()      { return DIRconfigFile("settings/security.json");         }
COLstring DIRfileConfigSso()           { return DIRconfigFile("settings/sso.json");              }
COLstring DIRfileConfigHa()            { return DIRconfigFile("settings/ha.json");               }

static COLstring DIRconfigFileRelative(COLstring Path) {
   COL_FUNCTION(DIRconfigFileRelative);
   FILcorrectPathSeparators(Path);
   return Path;
}
COLstring DIRfileConfigWebCertRelative() { return DIRconfigFileRelative("config/other/webcert.pem");  }
COLstring DIRfileConfigWebKeyRelative()  { return DIRconfigFileRelative("config/other/webkey.pem");   }

COLstring DIRresourceFile(const COLstring& Path){
   COL_FUNCTION(DIRresourceFile);
   const COLstring FileName = FILpathAppend(FILworkingDir(), "resources/" + Path);
   COL_VAR2(FileName, Path);
   return FileName;
}

// returns resources/.scratch/<Username>/
COLstring DIRresourceUserScratchDir(const COLstring& Username){
   COL_FUNCTION(DIRresourceUserScratchDir);
   COLstring Dir = DIRresourceFile(".scratch/" + Username + "/");
   COL_VAR(Dir);
   return Dir;
}

COLstring DIRfileResourceCaFile() { return DIRresourceFile("cacert.pem"); }

COLstring DIRscratchDir(const COLstring& SubDir) { return FILpathAppend(DIRresourcesDir(), ".scratch/" + SubDir + "/"); }

COLstring DIRarchiveDir() { return FILpathAppend(FILworkingDir(), "archive/"); }

COLstring DIRclusterDir(const COLstring& PemFile) { return FILpathAppend(DIRconfigDir(), FILpathAppend("cluster", PemFile)); }

COLstring DIRvmdDir(const COLstring& VmdName) {
   COLstring VmdDir = FILpathAppend(FILexecutableDir(), "..");
   VmdDir           = FILpathAppend(VmdDir, "vmd/" + VmdName);
   return VmdDir;
}

COLstring DIRiguanaDir(const COLstring& IguanaId)  { return FILpathAppend(DIRconfigFile("iguanas/"), IguanaId + "/"); }
COLstring DIRiguanaJson(const COLstring& IguanaId) { return FILpathAppend(DIRiguanaDir(IguanaId), "iguana.json"); }
COLstring DIRiguanaNameCache()                     { return FILpathAppend(DIRresourcesDir(), "iguana_names.json"); }