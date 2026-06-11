#ifndef __TSM_OBJECT_HANDLER_H__
#define __TSM_OBJECT_HANDLER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMobjectHandler
//
// Description:
//
// TSMobjectHandler class.
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 01:17:46 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>


class TSMhandlerStack;
class TSMlabel;
class TSMattributeList;

typedef void (*TSMobjectHandler)(TSMhandlerStack& HandlerStack, const TSMlabel& TagName, const TSMattributeList& AttributeList);

#endif // end of defensive include
