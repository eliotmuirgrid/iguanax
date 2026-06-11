#ifndef __CFG_CONFIG_SAVE_H_
#define __CFG_CONFIG_SAVE_H_
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfigSave
//
// Description:
//
// CFGconfigSave is VERY DELIBERATELY in it's own file.  The purpose of this
// logic is to save a configuration for a component using a type safe interface
// in a way which is tightly controled.
//
// Author: Eliot Muir
// Date:   Mon 11 Jul 2022 14:34:23 EDT
//---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <COL/COLarray.h>

struct CFGconfig;

COLvar CFGconfigSerializeFields(const COLmap<COLstring, COLstring>& Fields);
COLvar CFGconfigSerializeArray(const COLarray<COLstring>& Array);

// Returns a COLvar compatible with saving a component.
// Notice that we are passing in the Config object as a const reference
// indicated that the object is not altered by this function.
COLvar CFGconfigSerialize(const CFGconfig& Config);

#endif // end of defensive include
