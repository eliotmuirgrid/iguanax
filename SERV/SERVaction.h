#ifndef __SERV_ACTION_H__
#define __SERV_ACTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVaction
//
// Description:
//
// Install/stop/start service - used from the command line.
//
// Author: Eliot Muir 
// Date:   Thursday 22 September 2022 - 03:41PM
// ---------------------------------------------------------------------------
class COLstring;

void SERVactionOutput(const COLstring& Action);
int SERVaction(const COLstring& ActionIn, const COLstring& Username, const COLstring& Password);


#endif // end of defensive include
