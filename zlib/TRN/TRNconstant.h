#ifndef __TRN_CONSTANT_H__
#define __TRN_CONSTANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNconstant
//
// Description:
//
// TRNconstant class.
//
// Author: Kevin Senn
// Date:   Monday, April 18th, 2011 @ 02:18:45 PM
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

#define TRN_REQUIRE_SENTINEL "<<>><$#$$REQUIREPROBLEM&*^*&^"

// Protocol id for Displaying help params
extern const int TRN_LUA_HELP_PARAM_PROTOCOL;
extern const int TRN_LUA_HELP_PARAM_TYPE_REQUIRED;
extern const int TRN_LUA_HELP_PARAM_TYPE_OPTIONAL;

extern const COLstring TRN_MAIN_FILE_NAME;
extern const COLstring TRN_FILE_STATUS_UNCHANGED;
extern const COLstring TRN_FILE_STATUS_CHANGED;

extern const char* TRN_LUA_FILE_EXTENSION;

extern const COLstring TRN_ANNOTATION_STATE;

extern const COLstring TRN_BEGIN_TREE_VALUE;
extern const COLstring TRN_END_TREE_VALUE;

extern const char* TRN_BEGIN_HIGHLIGHT_SEARCH_MATCH;
extern const char* TRN_END_HIGHLIGHT_SEARCH_MATCH;
extern const char* TRN_HIGHLIGHT_SEARCH_MATCH;
extern const char* TRN_LEVEL_DIVIDER;

extern const unsigned int TRN_INTELLISENSE_MAX_LENGTH;
extern const COLstring TRN_INTELLISENSE_TRUNCATED;
extern const char* TRN_ALPHANUMERIC_CHARACTERS;

extern const char* TRN_GLOBAL_INTELLISENSE_VARIABLE;

const int TRN_MAX_RECURSION_LEVEL = 30;

#endif // end of defensive include
