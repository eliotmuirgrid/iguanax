#ifndef __INST_CONFIG_H__
#define __INST_CONFIG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTconfig
//
// Description:
//
// IguanaX configure API
// Note: These APIs depends on the INSTworkingDir API to correctly set the
// currently working dir so that they could configure the correct IguanaX
// working dir.
//
// Author: Tyler Brown 
// Date:   Tuesday 08 August 2023 - 01:17PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class COLstring;
class COLvar;

bool INSTgetServerConfig(const COLstring& Username, COLvar* pConfig, COLstring* pError);

void INSTconfigGet(const COLwebRequest& Request);
void INSTconfigSet(const COLwebRequest& Request);

#endif // end of defensive include
