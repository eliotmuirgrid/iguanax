#ifndef __TSM_ATTRIBUTE_H__
#define __TSM_ATTRIBUTE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMattribute
//
// Description:
//
// TSMattribute class.  This just a bit more than a struct representing
// the attribute of a XML tag - but not much more.
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 12:30:45 PM
//
//---------------------------------------------------------------------------

#include "TSMlabel.h"
#include "TSMvariant.h"

class TSMattribute 
{
public:
   TSMattribute();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSMattribute();

   const TSMlabel& name() const;
   TSMlabel& name();

   const TSMvariant& value() const;
   TSMvariant& value();

private:
   TSMlabel Name;
   TSMvariant Value;
};

COLostream& operator<<(COLostream& Stream, const TSMattribute& Attr);

#endif // end of defensive include
