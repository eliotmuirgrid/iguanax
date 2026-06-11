#ifndef __HELP_MODEL_H__
#define __HELP_MODEL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPobject
//
// Description:
//
// Data object to store help data in the backend
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <COL/COLarray.h>

class HELPparameter {
 public:
   bool      Required=false;
   COLstring Name;
   COLstring Description;
};

class HELPobject {
 public:
   COLstring                Name;
   bool                     ParameterTable=false;
   COLstring                SummaryLine;
   COLstring                Description;
   COLarray<HELPparameter> Parameters;
 };

COLostream& operator<<(COLostream& Stream, const HELPobject& Object);
COLostream& operator<<(COLostream& Stream, const HELPparameter& Parameter);

#endif // end of defensive include