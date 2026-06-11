#ifndef __HELP_SERIALIZE_H__
#define __HELP_SERIALIZE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPserialize
//
// Description:
//
// Routines to load and save from COLvar <--> HELPobject
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
class HELPobject;
class COLvar;
class COLstring;

void HELPmapObject(HELPobject* pHelp, const COLvar& Data);
void HELPtoObject (HELPobject* pHelp, const COLstring& Content);
void HELPtoVar    (COLvar* pOut,      const HELPobject& Help);

#endif // end of defensive include