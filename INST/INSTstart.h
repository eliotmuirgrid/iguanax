#ifndef __INST_START_H__
#define __INST_START_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTstart
//
// Description:
//
// Routines to start installer mode.
//
// Author: Matthew Sobkowski 
// Date:   Thursday 15 June 2023 - 11:05AM
// ---------------------------------------------------------------------------

class IWEBconfig;

// We pass the IWEBconfig so we can open a random port for the installer.
void INSTstartup(const IWEBconfig& Config);
bool INSTrerunAsAdmin(int argc, const char** argv);
void INSTpauseIfNotWindowsService();

bool INSTisAdmin();

#endif // end of defensive include
