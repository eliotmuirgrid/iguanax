// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMcolumnType
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 23 February 2023 - 06:22PM
// ---------------------------------------------------------------------------
#include <DBIM/DBIMcolumnType.h>

#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <DB/DBvariant.h>
COL_LOG_MODULE;

static COLhashmap<COLstring, DBsimpleType> ColumnTypeLookup;

// This populates the static member above with typical column types
// (so far dealt with Sql Server, MariaDb and MySql)
// Anything not listed here is interpreted as string, 
// This is done intentionally as there are a lot of sub-categories which we can interpret as string data
void DBIMinitTypeLookup(){
   COL_FUNCTION(DBIMinitTypeLookup);
   if (ColumnTypeLookup.size() != 0) { return; }
   // Integer column types
   ColumnTypeLookup["tinyint"]   = DBS_INTEGER;
   ColumnTypeLookup["smallint"]  = DBS_INTEGER;
   ColumnTypeLookup["mediumint"] = DBS_INTEGER;
   ColumnTypeLookup["int"]       = DBS_INTEGER;
   ColumnTypeLookup["integer"]   = DBS_INTEGER;
   ColumnTypeLookup["bigint"]    = DBS_INTEGER;
   // Double column types
   ColumnTypeLookup["decimal"]          = DBS_DOUBLE;
   ColumnTypeLookup["double"]           = DBS_DOUBLE;
   ColumnTypeLookup["double precision"] = DBS_DOUBLE; // Postgresql
   ColumnTypeLookup["float"]            = DBS_DOUBLE;
   ColumnTypeLookup["number"]           = DBS_DOUBLE;
   ColumnTypeLookup["numeric"]          = DBS_DOUBLE;
   ColumnTypeLookup["real"]             = DBS_DOUBLE;
   // TODO - ignored "money" columns from sql server
   // Datetime column types
   ColumnTypeLookup["date"]           = DBS_DATETIME;
   ColumnTypeLookup["datetime"]       = DBS_DATETIME;
   ColumnTypeLookup["datetime2"]      = DBS_DATETIME; // sql server specific
   ColumnTypeLookup["datetimeoffset"] = DBS_DATETIME; // sql server specific
   ColumnTypeLookup["smalldatetime"]  = DBS_DATETIME; // sql server specific
   ColumnTypeLookup["time"]           = DBS_DATETIME;
   ColumnTypeLookup["timestamp"]      = DBS_DATETIME;
   ColumnTypeLookup["year"]           = DBS_DATETIME;
}

static COLstring DBIMgetTypeString(const DBvariant& Value){
   COL_FUNCTION(DBIMgetTypeString);
   COLstring Type = Value.toString().toLowerCase();
   COLstring Extra;
   Type.split(Type, Extra, "(");
   return Type;
}

DBsimpleType DBIMcolumnType(const DBvariant& Value){
   COL_FUNCTION(DBIMcolumnType);
   COLstring Type = DBIMgetTypeString(Value);
   COL_VAR2(Type, ColumnTypeLookup.count(Type));
   // if the type isn't found, interpret as string
   return ColumnTypeLookup.count(Type) ? ColumnTypeLookup[Type] : DBS_STRING;
}