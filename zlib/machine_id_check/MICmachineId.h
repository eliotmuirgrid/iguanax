#ifndef __MIC_MACHINE_ID_H__
#define __MIC_MACHINE_ID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MICmachineId
//
// Description:
//
// MICmachineId class.
//
// Author: Art Harrison
// Date:   Tuesday, June 15th, 2010 @ 04:26:22 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLostream;

COLstring MICmachineId(COLostream& Log, bool Read, bool Write, bool UseHash5);

#endif // end of defensive include
