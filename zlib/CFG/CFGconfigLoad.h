#ifndef __CFG_CONFIG_LOAD_H_
#define __CFG_CONFIG_LOAD_H_
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfigLoad
//
// Description:
//
// CFGconfigLoad is VERY DELIBERATELY in it's own file.  The purpose of this
// logic is to load a configuration for a component using a type safe interface
// in a way which is backwardly compatible.
//
// THe code should be able to load older versions of component configurations.
//
// Author: Eliot Muir
// Date:   Mon 11 Jul 2022 14:34:23 EDT
//---------------------------------------------------------------------------
struct CFGconfig;
class COLstring;

const unsigned long long CFGcompJsonLimit = 100 * 1024 * 1024; // 100mb

// THIS API is BLOCKING.  DO NOT call it on the main event loop.
bool CFGconfigLoad(const COLstring& ComponentId, CFGconfig* pResult);

#endif // end of defensive include
