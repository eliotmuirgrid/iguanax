#ifndef __DBD_MODE_H__
#define __DBD_MODE_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDmode.h
//
//  Description: 
// 
//  Returns the current mode.
//
//  Author: Matthew Sobkowski
//  Date:   5/2/23 9:58 AM
//  ---------------------------------------------------------------------------

class DBDworld;
class COLwebRequest;
class COLstring;

enum class DBD_MODE {
   INSTALLER =0,
   NORMAL    =1,
   INITIALIZATON =2,
   LOG_UNLOCK =3
};

COLstring DBDmodeAsString(DBD_MODE Mode);

void DBDmodeWeb(const COLwebRequest& Request, DBDworld* pWorld);

#endif // end of defensive include
