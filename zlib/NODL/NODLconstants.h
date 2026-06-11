#ifndef __NODL_CONSTANTS_H__
#define __NODL_CONSTANTS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLconstants
//
// Description:
//
// A series of strings and arrays to be used throughout NODL
//
// Author: Vismay Shah 
// Date:   Thursday 1 December 2022
// ---------------------------------------------------------------------------

struct lua_State;

//Names for db/connection method parameters.
#define NODL_DB_PARAM_API            "api"
#define NODL_DB_PARAM_NAME           "name"
#define NODL_DB_PARAM_USER           "user"
#define NODL_DB_PARAM_PASSWORD       "password"
#define NODL_DB_PARAM_ENCRYPTION_KEY "encryption_key"
#define NODL_DB_PARAM_UNICODE        "use_unicode"
#define NODL_DB_PARAM_TIMEOUT        "timeout"
#define NODL_DB_PARAM_LIVE           "live"

// Exposing some Lua table names
#define NODL_DB_RECORDSET_ARRAY_METATABLE "db_recordset_array_metatable"
#define NODL_DB_ERROR_TABLE_TO_STRING     "db_error_table_to_string"
#define NODL_DB_CONNECTION_METATABLE      "dbconnection"

// For identifying a non-live connection
#define NODL_DB_NON_LIVE_WARNING          "db_connection_non_live"

// For NODLpreview
#define NODL_PREVIEW_LISTENER "PreviewListener"

//params possible when getting new connection
const char* const NODL_NEW_CONNECT_PARAMS[] = {
   NODL_DB_PARAM_API,
   NODL_DB_PARAM_NAME,
   NODL_DB_PARAM_USER,
   NODL_DB_PARAM_PASSWORD,
   // NODL_DB_PARAM_ENCRYPTION_KEY, // This is for #30607
   NODL_DB_PARAM_UNICODE,
   NODL_DB_PARAM_TIMEOUT,
   NODL_DB_PARAM_LIVE,
   nullptr
};

//params possible when using existing connection
const char* const NODL_OLD_CONNECT_PARAMS[] = {
   nullptr
};
const char* const NODL_CONNECT_OPTIONS[] = {
   NODL_DB_PARAM_LIVE,
   nullptr
};
// Params for closing a connection
const char* const NODL_DB_CLOSE_OPTIONS[] = {
   nullptr
}; 
// Params for doing a query
const char *const NODL_EXECUTE_OPTIONS[] = {
   "sql",
   NODL_DB_PARAM_LIVE,
   nullptr
};
// Transaction constants - begin, commit, merge and rollback
const char *const NODL_DB_TRANSACTION_OPTIONS[] = {
   NODL_DB_PARAM_LIVE, 
   nullptr
};
const char *const NODL_MERGE_OPTIONS[] = {
   "data",
   "bulk_insert",
   "transaction",
   NODL_DB_PARAM_LIVE,
   nullptr
};

void NODLaddDbApiConstants(lua_State* L);

#endif