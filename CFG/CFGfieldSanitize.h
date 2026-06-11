#ifndef __CFG_FIELDS_SANITIZE_H__
#define __CFG_FIELDS_SANITIZE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGfieldsSanitize
//
// Description:
//
// Field sanitization function.
//
// Author: John Qi originally - refactored by Matt and moved by Eliot
// Date:   Monday 24 April 2023 - 03:30PM
// ---------------------------------------------------------------------------
#include <CFG/CFGfieldGrammar.h>

bool CFGfieldsSanitizeValue(const COLstring& Name, CFGfieldType Type, COLstring& Value, COLstring* pError);

#endif // end of defensive include
