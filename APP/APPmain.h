#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: APPmain
//
// Description:
//
// C++ Land for our Iguana Controller.
//
// Author: Eliot Muir
// Date:   Thursday 06 July 2023 - 11:01AM
// ---------------------------------------------------------------------------

class COLvar;

bool APPactivateTracing(int argc, const char * argv[]);

void APPexePath(char** ppData);
void APPservicePath(char** ppData);

void APPiguanaStart();
void APPiguanaStop();
void APPiguanaHelp();
void APPgitHelp();
void APPhomeSpacesHelp();

bool APPiguanaStatus();

void APPiguanaDashboard();

void APPinstallService();
void APPiguanaUninstall();

void APPloadResourceData(const char* pFilePath, char** ppData, int* pSize);
void APPfreeData(char* pData);

bool APPlaunchAsController();

bool APPinstalled();

void APPwriteControllerPid();
bool APPkillMacController();

bool APPisControllerRunning();

bool APPiguanaLogExists();
bool APPiguanaServiceLog();

bool APPshowStartupPrompt();

bool APPhomeHasSpaces();

#endif // end of defensive include
