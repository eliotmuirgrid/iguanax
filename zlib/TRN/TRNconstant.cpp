//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNconstant
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, April 18th, 2011 @ 02:18:45 PM
//---------------------------------------------------------------------------
#include "TRNconstant.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

const int TRN_LUA_HELP_PARAM_PROTOCOL = 60;
const int TRN_LUA_HELP_PARAM_TYPE_REQUIRED = 1;
const int TRN_LUA_HELP_PARAM_TYPE_OPTIONAL = 2;

const COLstring TRN_MAIN_FILE_NAME        = "main.lua";
const COLstring TRN_FILE_STATUS_UNCHANGED = "unchanged";
const COLstring TRN_FILE_STATUS_CHANGED   = "edited";

const char* TRN_LUA_FILE_EXTENSION       = ".lua";

const COLstring TRN_ANNOTATION_STATE = "TRN_ANNOTATION_STATE";

const COLstring TRN_BEGIN_TREE_VALUE(" <span class=\"ITSintellisenseOptionValue\">(");
const COLstring TRN_END_TREE_VALUE(")</span>");

const char* TRN_BEGIN_HIGHLIGHT_SEARCH_MATCH = "<span class=\"ITSintellisenseSearchMatch\">";
const char* TRN_END_HIGHLIGHT_SEARCH_MATCH = "</span>";
const char* TRN_HIGHLIGHT_SEARCH_MATCH = "<span class=\"ITSintellisenseSearchMatch\">$1</span>";
const char* TRN_LEVEL_DIVIDER = "<span class=\"ITSintellisenseLevelDivider\"></span>";

const char* TRN_ALPHANUMERIC_CHARACTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

const unsigned int TRN_INTELLISENSE_MAX_LENGTH = 256;
const COLstring TRN_INTELLISENSE_TRUNCATED("&hellip;");

const char* TRN_GLOBAL_INTELLISENSE_VARIABLE = "_IntellisenseObject";
