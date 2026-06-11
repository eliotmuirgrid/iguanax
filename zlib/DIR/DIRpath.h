#ifndef __DIR_PATH_H__
#define __DIR_PATH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRpath
//
// Description:
//
// Just pure functions for defining paths, filenames etc. Goal is not
// to get into dependency hell with including the world with this info.
//
// But to define it one spot.
//
// Author: Eliot Muir
// Date:   Friday 24 March 2023 - 03:39PM
// ---------------------------------------------------------------------------
class COLstring;


COLstring DIRcheckAboveRoot(const COLstring& Source, const COLstring& RelativeFilePath);

// returns the iNTERFACEWARE/ directory
COLstring DIRconfigRootDirDefault();
// returns iNTERFACEWARE/IguanaX/
COLstring DIRconfigDirDefault();

COLstring DIRcomponentDir(const COLstring& ComponentId);
COLstring DIRcomponentConfig(const COLstring& ComponentId);
COLstring DIRcomponentDevDir(const COLstring& ComponentId);
COLstring DIRcomponentRunDir(const COLstring& ComponentId);
COLstring DIRcomponentSampleDir(const COLstring& ComponentId);

COLstring DIRcomponentProjectDir(const COLstring& ComponentId, bool IsDev);

COLstring DIRcompsDir();
COLstring DIRusersDir();
COLstring DIRcrashDir();
COLstring DIRsettingsDir();
COLstring DIRotherDir();
COLstring DIRresourcesDir();
COLstring DIRconfigDir();
COLstring DIRconfigFile(const COLstring& Path);

COLstring DIRconfigUserDir(const COLstring& Username);
COLstring DIRconfigUserFile(const COLstring& Username);
COLstring DIRconfigUserGitFile(const COLstring& Username);
COLstring DIRconfigUserSshFile(const COLstring& Username);
COLstring DIRconfigUserOwnersFile(const COLstring& Username);
COLstring DIRconfigUserOrgsFile(const COLstring& Username);

COLstring DIRfileConfigUsers();

COLstring DIRfileConfigEncrypt();
COLstring DIRfileConfigLicense();
COLstring DIRfileConfigDatabase();
COLstring DIRfileConfigGitCatalogue();
COLstring DIRfileConfigServers();
COLstring DIRfileConfigNotifications();
COLstring DIRfileConfigRoles();
COLstring DIRfileConfigWeb();
COLstring DIRfileConfigLogging();
COLstring DIRfileConfigLog();
COLstring DIRfileConfigAutoUpgrade();
COLstring DIRfileConfigWebCert();
COLstring DIRfileConfigWebKey();
COLstring DIRfileConfigTunnelKey();
COLstring DIRfileConfigEnvironment();
COLstring DIRfileConfigInstance();
COLstring DIRfileConfigSecurity();
COLstring DIRfileConfigSso();
COLstring DIRfileConfigHa() ;

COLstring DIRfileConfigWebCertRelative();
COLstring DIRfileConfigWebKeyRelative();

COLstring DIRresourceUserScratchDir(const COLstring& Username);

COLstring DIRfileResourceCaFile();

COLstring DIRscratchDir(const COLstring& SubDir);

COLstring DIRarchiveDir();

COLstring DIRclusterDir(const COLstring& PemFile);

COLstring DIRvmdDir(const COLstring& VmdName);

COLstring DIRiguanaDir(const COLstring& IguanaId);
COLstring DIRiguanaJson(const COLstring& IguanaId);
COLstring DIRiguanaNameCache();

#endif // end of defensive include