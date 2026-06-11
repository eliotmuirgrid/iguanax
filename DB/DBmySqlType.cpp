//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBmySqlType
//
// Description:
//
// Function(s) to return MySql datatype names
//
//
// Author: Nicolas Lehman
// Date:   Tue Jun 1 2021
//
//---------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// JULIAN OBDC types are different mappings (different standard...)
// https://docs.oracle.com/cd/E17952_01/connector-odbc-en/connector-odbc-reference-datatypes.html
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// JULIAN notes for "tricky/subtle" things that need to be addressed - I tried to make these as clear as 
//        possible but as I do not understand everything about this the notes may be "fuzzy" or incomplete
//------------------------------------------------------------------------------------------------------------
// MySQL types mappings:
//  1:1 = maps one internal data type to one external data type
//  M:1 = maps many internal data types to one external data type
//  1:M = maps one internal data types to several external data type
//  M:M = MySQL does not do this as far as I can tell
//
// Ideally we want to resolve all type relationsips to 1:1 - if we can do this then we will
// know the exact inernal type for every field in a table. 
//
// Alternatively resolving to M:1 may be enough as we can use the most general type. Often
// with M:1 mappings one of the internal types is a superset of all the others - in this
// case you can choose the "superset type". The supersets are usually based on size as the 
// type must be the same (or comaptible). For byte sizes (i.e., 8 vs 16 vs 32 bit) or max 
// length of the field (i.e., char = 255 vs varchar = 65,535)
//
// As we (ideally) want to present the *exact* INTERNAL data type (a 1:1 mappimg) of the field to the user we 
// need to understand (or "reverse engineer") the data mapping rules that are used so we can identify the 
// correct internal data type for each field
// 
//  1:1 = resolved                                          = no further investigation
//  M:1 = moderate  - can use the most general type usually = some investigation
//  1:M = difficult - need to investigate the rules         = more investigation
//
//------------------------------------------------------------------------------------------------------------
// (1) INT: NULL-type - QUESTION this is simply a NULL value???
//     NOT available as a choice for a field type when defining a table ... hmmm...
//     not sure what/why/when this is used or why it is defined
//
//------------------------------------------------------------------------------------------------------------
// (2) Binary string type
//     - MySQL treats binary data as character data with a "binary" character set (like UTF8 unicode character set) 
//     - Because of this MySQL uses the same external datatype for CHAR and BINARY data, for example
//       - MYSQL_TYPE_BLOB maps to both BLOB (binary) and TEXT (char) internal types
//         the same applies 
//       - MYSQL_TYPE_STRING maps to both CHAR and BINARY internal type
//       - MYSQL_TYPE_BLOB maps to both BLOB (binary) and TEXT (char) internal types
//       - MYSQL_TYPE_TINY_BLOB (probably) maps to both TINYBLOB (binary) and TINYTEXT (char) internal types
//       - MYSQL_TYPE_MEDIUM_BLOB (probably) maps to both MEDIUMBLOB (binary) and MEDIUMTEXT (char) internal types
//       - MYSQL_TYPE_LONG_BLOB (probably) maps to both LONGBLOB (binary) and LONGTEXT (char) internal types
//       - MYSQL_TYPE_VAR_STRING (probably) maps CHAR, BINARY & VARBINARY internal type
//         NOTE: You might expect it to also map to VARRCHAR (it did before V5) but MYSQL_TYPE_VARCHAR was 
//               introduced for V5+ just to ensure that it is even less consistent & more confusing
//     - Oracle supports conversion to "CHAR, VARCHAR2, LONG, RAW, and LONG RAW: Conversion is valid for output."
//       which effectively means IF you KNOW it is not binary you can convert to char (CHAR, VARCHAR2, LONG)
//       otherwise you must convert to a binary format (RAW, and LONG RAW) = THIS ALWAYS WORKS
//       https://docs.oracle.com/en/database/oracle/oracle-database/12.2/drdaa/API-ref-for-oracle-MySQL-client-library.html#GUID-7B8AD58E-3950-493F-96E8-96C597F4BE62
//     - It seems like we can use the mysql_fetch_fields() function to identify which CHAR types are BINARY 
//       mysql_fetch_fields() returns an array of MYSQL_FIELD structures for all columns of a result set.
//       see https://dev.mysql.com/doc/c-api/8.0/en/mysql-fetch-fields.html
//     - MYSQL_FIELD structures contain flags including "BINARY_FLAG = Field has the BINARY attribute" which
//       appears to indicate whether it is BINARY (true) data or CHAR (false) data 
//       see https://dev.mysql.com/doc/c-api/5.7/en/c-api-data-structures.html (scroll about half way down the page)
//       see https://www.php.net/manual/en/function.mysql-field-flags.php (same info for php - nicer visually)
//       see https://dev.mysql.com/doc/dev/mysql-server/8.0.12/structMYSQL__FIELD.html#a99a3cdadd0b01a750f203fac9250dba2 
//           struct reference generated from mysql.h - no explanatory info...
//------------------------------------------------------------------------------------------------------------
// (2) NON-BINARY Strings
//     - can have a binary collation (_bin) = confusing terminology
//------------------------------------------------------------------------------------------------------------
// (3) BLOBs are padded with binary 0 (\0) so they take up a lot of space
// (4) It seems likely that the TINY/MEDIUM/LONG BLOBs are shorter than BLOB so you can reduce storage
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------
// JULIAN Less useful (?) should be mostly covered above - but I kept it anyway...
// -----------------------------------------------------------------------------------------------------------
// (1) ORACLE DOCS For instance, MYSQL_TYPE_VAR_STRING is mapped to VARCHAR in the server. 
//     https://docs.oracle.com/en/database/oracle/oracle-database/12.2/drdaa/API-ref-for-oracle-MySQL-client-library.html#GUID-7B8AD58E-3950-493F-96E8-96C597F4BE62
// (2) MySQL treats/stores BINARY data as "BINARY STRINGS" and maps them to MYSQL_TYPE_VAR_STRING = they are a "binary type" of string ***UGLYUGLYUGLY***
// (3) MYSQL Also maps character strings to MYSQL_TYPE_VAR_STRING
// (4) CONCLUSION: This maps to CHAR/VARCHAR and BINARY/VARBINARY types ***UGLYUGLYUGLY***
// (5) So this case needs to return TWO internal types - that are incompatible/exclusive = the user must then work which it is
// (6) We should be able to query the metatdata to identify if the field is binary
// (7) What we need to return...
//   return "VARCHAR/VARBINARY";
// -----------------------------------------------------------------------------------------------------------

#include "DBmySqlType.h"
#include "DBdriverMySql.h"

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBMySqlTypeAsString (mysql_field_types MySqlType) {
   COL_FUNCTION(DBMySqlTypeAsString);

// "INT: " represents the internal MySQL data type represented by the MySQL TYPE CONSTANT
   switch(MySqlType) {
      case MYSQL_TYPE_DECIMAL:            // INT: DECIMAL 
         return "MYSQL_TYPE_DECIMAL";     // NUMERIC is an accepted alias for DECIMAL - NUMERIC is implemented as DECIMAL = they are the same
         break;
      case MYSQL_TYPE_TINY:               // INT: TINYINT
         return "MYSQL_TYPE_TINY";    
         break;
      case MYSQL_TYPE_SHORT:              // INT: SMALLINT  
         return "MYSQL_TYPE_SHORT";
         break;
      case MYSQL_TYPE_LONG:               // INT: INTEGER
         return "MYSQL_TYPE_LONG";
         break;
      case MYSQL_TYPE_FLOAT:              // INT: FLOAT - four bytes single-precision value
         return "MYSQL_TYPE_FLOAT";       // no alias for FLOAT was indentified (yet)
         break;
      case MYSQL_TYPE_DOUBLE:             // INT: DOUBLE - eight bytes double-precision value
         return "MYSQL_TYPE_DOUBLE";      // REAL is an accepted alias for DOUBLE - REAL is implemented as DOUBLE = they are the same
         break;
      case MYSQL_TYPE_NULL:               // INT: NULL-type - QUESTION this is simply a NULL value???
         return "MYSQL_TYPE_NULL";        // NOT available as a choice for a field type when defining a table ... hmmm...
         break;
      case MYSQL_TYPE_TIMESTAMP:          // INT: TIMESTAMP
         return "MYSQL_TYPE_TIMESTAMP";
         break;
      case MYSQL_TYPE_LONGLONG:           // INT: BIGINT
         return "MYSQL_TYPE_LONGLONG";
         break;
      case MYSQL_TYPE_INT24:              // INT: MEDIUMINT
         return "MYSQL_TYPE_INT24";
         break;
      case MYSQL_TYPE_DATE:               // INT: DATE
         return "MYSQL_TYPE_DATE";
         break;
      case MYSQL_TYPE_TIME:               // INT: TIME
         return "MYSQL_TYPE_TIME";
         break;
      case MYSQL_TYPE_DATETIME:           // INT: DATETIME
         return "MYSQL_TYPE_DATETIME";
         break;
      case MYSQL_TYPE_YEAR:               // INT: YEAR
         return "MYSQL_TYPE_YEAR";
         break;
      case MYSQL_TYPE_NEWDATE:            // INT: DATE - functionally equivalent to MYSQL_TYPE_DATE from pre version 5 format
         return "MYSQL_TYPE_NEWDATE";     // but takes 3 bytes storage instead of 4 bytes (both versions accepted but stored as 3 bytes)
         break;
      case MYSQL_TYPE_VARCHAR:            // INT: VARCHAR 
         return "MYSQL_TYPE_VARCHAR";
         break;
      case MYSQL_TYPE_BIT:                // INT: BIT
         return "MYSQL_TYPE_BIT";
         break;
      case MYSQL_TYPE_NEWDECIMAL:         // INT: DECIMAL ("alias" for NUMERIC) - NUMERIC is implemented as DECIMAL = they are the same
         return "MYSQL_TYPE_NEWDECIMAL";  // NEW probably indicates changed storage design??? - like NEWDATE
         break;
      case MYSQL_TYPE_ENUM:               // INT: ENUM
         return "MYSQL_TYPE_ENUM";
         break;
      case MYSQL_TYPE_SET:                // INT: SET
         return "MYSQL_TYPE_SET";
         break;
      case MYSQL_TYPE_TINY_BLOB:          // INT: TINYBLOB or TINYTEXT ???
         return "MYSQL_TYPE_TINY_BLOB";   // (probably) maps to both TINYBLOB (binary) and TINYTEXT (char) internal types
         break;
      case MYSQL_TYPE_MEDIUM_BLOB:        // INT: MEDIUMBLOB or MEDIUMTEXT ???
         return "MYSQL_TYPE_MEDIUM_BLOB"; // (probably) maps to both MEDIUMBLOB (binary) and MEDIUMTEXT (char) internal types
         break;
      case MYSQL_TYPE_LONG_BLOB:          // INT: LONGBLOB or LONGTEXT ???
         return "MYSQL_TYPE_LONG_BLOB";   // (probably) maps to both LONGBLOB (binary) and LONGTEXT (char) internal types
         break;
      case MYSQL_TYPE_BLOB:               // INT: BLOB or TEXT
         return "MYSQL_TYPE_BLOB";        // // NOTE: Binary data is treated as character data where character set = "binary"
         break;
      case MYSQL_TYPE_VAR_STRING:         // INT: VARCHAR or VARBINARY 
         return "MYSQL_TYPE_VAR_STRING";  // NOTE: Binary data is treated as character data where character set = "binary"
         break;
      case MYSQL_TYPE_STRING:             // INT: CHAR or BINARY
         return "MYSQL_TYPE_STRING";      // NOTE: Binary data is treated as character data where character set = "binary"
         break;
      case MYSQL_TYPE_GEOMETRY:           // INT: Spatial  
         return "MYSQL_TYPE_GEOMETRY";
         break;
      default:
         return "UNKNOWN";
         break;
   }
}