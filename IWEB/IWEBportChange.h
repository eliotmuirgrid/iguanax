#ifndef __IWEB_PORT_CHANGE_H__
#define __IWEB_PORT_CHANGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBportChange
//
// Description:
//
// Change the port for the webserver - useful for hostile environments like Windows
//
// Author: Eliot Muir 
// Date:   Tuesday 11 July 2023 - 04:34PM
// ---------------------------------------------------------------------------

class COLostream;

void IWEBportChange(int NewPort);
void IWEBportChangeInfo(COLostream& Out);

#endif // end of defensive include
