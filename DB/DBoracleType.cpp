//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBoracleType
//
// Description: Implementation
//
// Author: Julian Muir
// Date:   Tue Jun 2 2021
//
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// JULIAN It seems that many of the SQLT_XXX types are OCI specific types that programmers can use - may or 
//        even most of these will never be returned by Oracle as a filed type. So it seems that we only need
//        the internal types for those type output by Oracle. All the other can simply return the SQLT_XXX in 
//        the case statement and they should never be returned. If any of the SQLT_XXX are returned and 
//        displayed in the front end we can then simply update the type mapping to match... 
//
// If this is unclear please speak to JULIAN & NIC as we understand this is about...
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// JULIAN notes for "tricky/subtle" things that need to be addressed - I tried to make these as clear as 
//        possible but as I do not understand everything about this the notes may be "fuzzy" or incomplete
//------------------------------------------------------------------------------------------------------------
// Oracle types mappings:
//  1:1 = maps one internal data type to one external data type
//  M:1 = maps many internal data types to one external data type
//  1:M = maps one internal data types to several external data type
//  M:M = maps several internal data types to several external data type
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
//  M:M = most difficult - need to investigate the rules    = most investigation
//        NOTE: I did not find any M:M mappings (yet)
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
//
// It appears that Oracle keeps extra type information as metadata attributes to each field in a table see
// paragraph immediately after the query below :)
//
// https://docs.oracle.com/en/database/oracle/oracle-database/21/lnoci/data-types.html#GUID-65066795-6F95-44AE-A454-31CF70A46A1F
//
// Typecodes
//
// A unique typecode is associated with each Oracle Database type, whether scalar, collection, reference, or 
// object type.
//
// This typecode identifies the type, and is used by Oracle Database to manage information about object type 
// attributes. This typecode system is designed to be generic and extensible. It is not tied to a direct 
// one-to-one mapping to Oracle data types. Consider the following SQL statements:
//
// CREATE TYPE my_type AS OBJECT
// ( attr1    NUMBER,
//   attr2    INTEGER,
//   attr3    SMALLINT);
//
// CREATE TABLE my_table AS TABLE OF my_type;
//
// These statements create an object type and an object table. When it is created, my_table has three columns, 
// all of which are of Oracle NUMBER type, because SMALLINT and INTEGER map internally to NUMBER. The internal 
// representation of the attributes of my_type, however, maintains the distinction between the data types of 
// the three attributes: attr1 is OCI_TYPECODE_NUMBER, attr2 is OCI_TYPECODE_INTEGER, and attr3 is 
// OCI_TYPECODE_SMALLINT. If an application describes my_type, these typecodes are returned.
//
// OCITypeCode is the C data type of the typecode. The typecode is used by some OCI functions, like 
// OCIObjectNew(), where it helps determine what type of object is created. It is also returned as the value 
// of some attributes when an object is described; for example, querying the OCI_ATTR_TYPECODE attribute of a 
// type returns an OCITypeCode value. 
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// There are some subtleties with using the Oracle NUMBER internal type - some of the OCI funtions take
// parameters that can set the precision of the number. I am not sure (yet) if the affects/changes the type
// mapping it depends on how NUMBER works - it may be that NUMBER is a ***superset*** of the possibilities
// if this is the case then it is n problem - however it may be that you can set the precision of NUMBER in
// which case it may affect the mappings 
//
// From: https://docs.oracle.com/en/database/oracle/oracle-database/21/lnoci/data-types.html#GUID-7283C7EC-E768-44AD-991A-7E28AB49E7D3
// Footnote  28 
// p is the precision of the NUMBER in decimal digits; s is the scale of the NUMBER in decimal digits.
// Footnote 26
// This is equivalent to a NUMBER with no decimal places.
// Footnote 27
// i is the size of the NUMBER in bytes, set as part of an OCI call.
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// QUESTIONS:
// It seems that Oracle MAY have a M:M type relationship??? Need to investigate and document (MYSQL = 1:M = easier)
// Basically there is (ultimately) a 1:1 relatinship - so I need to figure out the rules
//
//  1:M = difficult - need to investigate the rules         = more investigation
//      - VARCHAR2 -> SQLT_VCS & SQLT_CHR
//      - NUMBER   -> SQLT_NUM, SQLT_INT, SQLT_FLT, SQLT_UIN & SQLT_VNU
//  M:1 
//      - NUMBER & FLOAT <- SQL_NUM 
//      - VARCHAR2 & NVARCHAR2 <- SQLT_CHR
//      - CHAR & NCHAR <- SQLT_AFC
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// Testing Oracle types by creating a table and looking at the types in an query - DBexample/testOracleOci.cpp
// Need to test the types and theIr various options, types tested already
// - NUMBER
//   - FLOAT (NUMBER with char insstead of byte precision - implemented as NUMBER)
// - VARCHAR2
//   - NVARCHAR2 ("Unicode" varchar using 2 bytes per character implemented as VARCHAR2)
//   - CHARACTER VARYING -> VARCHAR2 (alias)
// - LONG
// - LONG RAW
// - RAW
// - CHAR & NCHAR
//   - CHARACTER -> CHAR (alias) 
// - DATE
// - BLOB
// - CLOB
// - BFILE
// - TIMESTAMP
// - TIMESTAMP WITH TIME ZONE
// - TIMESTAMP WITH LOCAL TIME ZONE
// - INTERVAL YEAR TO MONTH
// - INTERVAL DAY TO SECOND
// - BINARY_DOUBLE
// - BINARY_FLOAT
// - ROWID
//------------------------------------------------------------------------------------------------------------

#include "DBoracleType.h"

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBoracleOciTypeAsString(oci_field_types OciType, COLstring &OciConstant) {
    COL_FUNCTION(DBoracleOciTypeAsString);

// JULIAN do we want to use EXTERNAL names instead from this page 
    switch(OciType) {
        case _SQLT_CHR:                  // EXT: VARCHAR2 INT: VARCHAR2 & NVARCHAR2 (VARCHAR = alias for VARCHAR2)
            OciConstant = "SQLT_CHR";    // SQLT_VCS & SQLT_CHR both map to VARCHAR2
            return "VARCHAR2 & NVARCHAR2 = SQLT_CHR"; // yes
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_CHR for a VARCHAR2 & NVARCHAR2
        case _SQLT_VCS:                  // EXT: VARCHAR LONG INT: VARCHAR2
            OciConstant = "SQLT_VCS";    // SQLT_VCS & SQLT_CHR both map to VARCHAR2
            return "VARCHAR2 = SQLT_VCS";
            break;
        case _SQLT_AFC:                  // EXT: CHAR LONG INT: CHAR & NCHAR
            OciConstant = "SQLT_AFC";
            return "CHAR & NCHAR = SQLT_AFC";
            break;
        case _SQLT_NUM:                  // EXT: NUMBER INT: NUMBER
            OciConstant = "SQLT_NUM";    // oci_define_by_pos+SQLT_NUM produces garbage - oci_define_by_pos+SQLT_LNG/SQLT_CHR works (Oracle docs wrong???)
            return "NUMBER = SQLT_NUM";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_NUM for a NUMBER
        case _SQLT_INT:                  // EXT: INTEGER  INT: NUMBER
            OciConstant = "SQLT_INT";    // return = SQLT_NUM instead
            return "NUMBER = SQLT_INT";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_NUM for a FLOAT (because it is implemented as a NUMBER)
        case _SQLT_FLT:                  // EXT: FLOAT INT: NUMBER - FLOAT is as alias using precision in binary bits (NUMBER is precision in decimal digits) 
            OciConstant = "SQLT_FLT";    // return = SQLT_NUM instead
            return "FLOAT = SQLT_FLT";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer 
        //        as far as I can tell Oracle will always return SQLT_NUM for a FLOAT (because it is implemented as a NUMBER)
        case _SQLT_VNU:                  // EXT: VARNUM INT: NUMBER??? - format seems to be length of NUMBER (22 BYTES) + 1 for null terminator
            OciConstant = "SQLT_VNU";    // return = SQLT_NUM instead
            return "NUMBER = SQLT_VNU";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_NUM for a UNSIGNED INT (because it is implemented as a NUMBER)
        case _SQLT_UIN:                  // EXT: UNSIGNED INT LONG INT: NUMBER
            OciConstant = "SQLT_UIN";    // return = SQLT_NUM instead
            return "NUMBER = SQLT_UIN";
            break;
        case _SQLT_LNG:                  // EXT: LONG INT: LONG - this is a max 2GB character string type
            OciConstant = "SQLT_LNG";
            return "LONG = SQLT_LNG";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_LNG for a LONG 
        case _SQLT_LVC:                  // EXT: LONG VARCHAR LONG INT: LONG
            OciConstant = "SQLT_LVC";    // return = SQLT_LNG instead
            return "SQLT_LVC";
            break;
        case _SQLT_BIN:                  // EXT: RAW LONG INT: RAW 
            OciConstant = "SQLT_BIN";
            return "RAW = SQLT_BIN";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_BIN for a RAW 
        case _SQLT_LVB:                  // EXT: LONG VARRAW  INT: RAW
            OciConstant = "SQLT_LVB";    // return = SQLT_BIN instead
            return "SQLT_LVB";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_BIN for a RAW 
        case _SQLT_VBI:                  // EXT: VARRAW LONG INT: RAW
            OciConstant = "SQLT_VBI";    // return = SQLT_BIN instead
            return "SQLT_VBI";
            break;
        case _SQLT_LBI:                  // EXT: LONG RAW LONG INT: LONG RAW - this is a max 2GB binary data type
            OciConstant = "SQLT_LBI";
            return "LONG RAW = SQLT_LBI";
            break;
        case _SQLT_DAT:                  // EXT: DATE LONG INT: DATE
            OciConstant = "SQLT_DAT";
            return "DATE = SQLT_DAT";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_DAT for a DATE 
        //        ---- not 100% sure ----
        case _SQLT_DATE:                 // EXT: ANSI DATE LONG INT: DATE
            OciConstant = "SQLT_DATE";
            return "DATE = SQLT_DATE";
            break;
        case _SQLT_CLOB:                 // EXT: Character LOB descriptor LONG INT: CLOB
            OciConstant = "SQLT_CLOB";
            return "CLOB = SQLT_CLOB";
            break;
        case _SQLT_BLOB:                 // EXT: Binary LOB descriptor LONG INT: BLOB
            OciConstant = "SQLT_BLOB";
            return "BLOB = SQLT_BLOB";
            break;
        case _SQLT_BFILEE:               // EXT: Binary FILE descriptor LONG INT: BFILE
            OciConstant = "SQLT_BFILEE";
            return "BFILE = SQLT_BFILEE";
            break;
        case _SQLT_TIMESTAMP:            // EXT: TIMESTAMP LONG INT: TIMESTAMP
            OciConstant = "SQLT_TIMESTAMP";
            return "TIMESTAMP = SQLT_TIMESTAMP";
            break;
        case _SQLT_TIMESTAMP_TZ:         // EXT: TIMESTAMP WITH TIME ZONE LONG INT: TIMESTAMP WITH TIME ZONE
            OciConstant = "SQLT_TIMESTAMP_TZ";
            return "TIMESTAMP WITH TIME ZONE = SQLT_TIMESTAMP_TZ";
            break;
         case _SQLT_TIMESTAMP_LTZ:        // EXT: TIMESTAMP WITH LOCAL TIME ZONE LONG INT: TIMESTAMP WITH LOCAL TIME ZONE
            OciConstant = "SQLT_TIMESTAMP_LTZ";
            return "TIMESTAMP WITH LOCAL TIME ZONE = SQLT_TIMESTAMP_LTZ";
            break;
       case _SQLT_INTERVAL_YM:          // EXT: INTERVAL YEAR TO MONTH LONG INT: INTERVAL YEAR TO MONTH
            OciConstant = "SQLT_INTERVAL_YM";
            return "INTERVAL YEAR TO MONTH = SQLT_INTERVAL_YM";
            break;
        case _SQLT_INTERVAL_DS:          // EXT: INTERVAL DAY TO SECOND LONG INT: INTERVAL DAY TO SECOND
            OciConstant = "SQLT_INTERVAL_DS";
            return "INTERVAL DAY TO SECOND = SQLT_INTERVAL_DS";
            break;
        case _SQLT_BFLOAT:               // EXT: native float ??? LONG INT: BINARY_FLOAT
            OciConstant = "SQLT_BFLOAT"; // on Oracle web page but not in the ocidfn.h header file :(
            return "BINARY_FLOAT = SQLT_BFLOAT";
            break;
        case _SQLT_BDOUBLE:              // EXT: native double ??? LONG INT: BINARY_DOUBLE
            OciConstant = "SQLT_BDOUBLE";
            return "BINARY_DOUBLE = SQLT_BDOUBLE";
            break;
        case _SQLT_RDD:                  // no mapping found (ROWID descriptor /* rowid descriptor */)
            OciConstant = "SQLT_RDD";
            return "SQLT_RDD";
            break;
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        //        as far as I can tell Oracle will always return SQLT_RDD for a ROWID 
        case _SQLT_RID:                  // no mapping found (/* rowid */)
            OciConstant = "SQLT_RID";    // return = SQLT_RDD instead
            return "SQLT_RID";
            break;
        //------------------------------------------------------------------------------------------------------------
        // JULIAN the OCI Types below look like they do not need to be mapped as they will never be returned 
        // as a fiels type from Oracle
        //------------------------------------------------------------------------------------------------------------

// JULIAN this one is a mystery I cannot find anything
        case _SQLT_NON:                  // no mapping found (/* Null/empty PCC Descriptor entry */)
            OciConstant = "SQLT_NON";
            return "SQLT_NON";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// found this page which suggests using it to prevent truncation of spaces 
// https://comp.databases.oracle.misc.narkive.com/fNqkBEqv/sqlt-chr-and-null-values-in-oci
// sounds like it is a null terminated string type like SQLT_STR - but SQLT_AVC will keep spaces when you BIND with it
// unlike SQLT_STR which will truncate spaces
// 99.999% sure Oracle will never return this as a field type 
        case _SQLT_AVC:                  // no mapping found (CHARZ type can be used for UROWID /* Ansi Var char */)
            OciConstant = "SQLT_AVC";
            return "SQLT_AVC";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// https://community.oracle.com/tech/developers/discussion/153585/does-oci-support-cursor-actions
// appears to be a type that allows you to BIND to an Oracle cursor 
// 99.999% sure Oracle will never return this as a field type 
        case _SQLT_CUR:                  // no mapping found (/* cursor  type */)
            OciConstant = "SQLT_CUR";
            return "SQLT_CUR";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// https://docs.oracle.com/cd/B19306_01/network.102/b14267/intro.htm
// Oracle Label Security obviates the need for such measures by enabling row-level access control
// So I am guessing this is something to do with the metadata/labels used to enact row level security
// 99.999% sure Oracle will never return this as a field type 
        case _SQLT_LAB:                  // no mapping found (/* label type */)
            OciConstant = "SQLT_LAB";
            return "SQLT_LAB";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// https://docs.oracle.com/cd/B19306_01/network.102/b14267/intro.htm
// This one also seems to be about labels again - perhaps OS specific
// 99.999% sure Oracle will never return this as a field type 
        case _SQLT_OSL:                  // no mapping found (/* oslabel type */)
            OciConstant = "SQLT_OSL";
            return "SQLT_OSL";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// looks like it is PL/SQL related so it is not for field types (PL/SQL is the Oracle specific language for stored procedures)
// 99.999% sure Oracle will never return this as a field type 
        case _SQLT_PNTY:                 // no mapping found (/* pl/sql representation of named types */)
            OciConstant = "SQLT_PNTY";
            return "SQLT_PNTY";
            break;

// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// the best clue I found was this page 
// https://docs.oracle.com/cd/B10501_01/appdev.920/a96584/oci15r30.htm
// OCIBindByPos()
// valuep (IN/OUT)
// An address of a data value or an array of data values of the type specified in the dty parameter. An array of data 
// values can be specified for mapping into a PL/SQL table or for providing data for SQL multiple-row operations. 
// When an array of bind values is provided, this is called an array bind in OCI terms.
// For SQLT_NTY or SQLT_REF binds, the valuep parameter is ignored. The pointers to OUT buffers are set in the pgvpp 
// parameter initialized by OCIBindObject().
// If the OCI_ATTR_CHARSET_ID attribute is set to OCI_UTF16ID (replaces the deprecated OCI_UCS2ID, which is retained 
// for backward compatibility), all data passed to and received with the corresponding bind call is assumed to be in UTF-16 encoding.
// See Also:
// OCI_ATTR_CHARSET_ID.
        case _SQLT_REF:                  // EXT: REF LONG INT: 
            OciConstant = "SQLT_REF";
            return "SQLT_REF";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// the only clues I found was these 2 pages about using SQLT_NTY for binding
// https://docs.oracle.com/cd/A64702_01/doc/server.805/a58234/obj_bind.htm
// Named Data Type Binds
// For a named data type (object type or collection) bind, a second bind call is necessary (following OCIBindByName() 
// or OCIBindByPos() ). The OCI Bind Object Type call, OCIBindObject(), sets up additional attributes specific to the 
// object type bind. An OCI application uses this call when fetching data from a table which has a column with an object datatype.
// The OCIBindObject() call takes, among other parameters, a Type Descriptor Object (TDO) for the named data type. 
// The TDO, of datatype OCIType is created and stored in the database when a named data type is created. It contains 
// information about the type and its attributes. An application can obtain a TDO by calling OCITypeByName().
// The OCIBindObject() call also sets up the indicator variable or structure for the named data type bind.
// When binding a named data type, use the SQLT_NTY datatype constant to indicate the datatype of program variable 
// being bound. SQLT_NTY indicates that a C struct representing the named data type is being bound. A pointer to this 
// structure is passed to the bind call.
// It is possible that working with named data types may require the use of three bind calls in some circumstances. 
// For example, to bind a static array of named data types to a PL/SQL table, three calls must be invoked: OCIBindByName(), 
// OCIBindArrayOfStruct(), and OCIBindObject(). 
// https://docs.oracle.com/cd/E11882_01/appdev.112/e10646/oci03typ.htm#LNOCI16300
// Named Data Types: Object, VARRAY, Nested Table
// Named data types are user-defined types that are specified with the CREATE TYPE command in SQL. Examples include 
// object types, varrays, and nested tables. In OCI, named data type refers to a host language representation of the type. 
// The SQLT_NTY data type code is used when binding or defining named data types.
// In a C application, named data types are represented as C structs. These structs can be generated from types stored in 
// the database by using the Object Type Translator. These types correspond to OCI_TYPECODE_OBJECT.
// See Also:
//     "Object Type Information Storage and Access" for more information about working with named data types in OCI
//     Chapter 15 for information about how named data types are represented as C structs
        case _SQLT_NTY:                  // EXT: NAMED DATATYPE LONG INT: 
            OciConstant = "SQLT_NTY";
            return "SQLT_NTY";
            break;

// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// the only clue I found was this page where someone is having trouble reading a NUMBER into this field
// also he refers to SQLT_STR as a char* variable in c/c++ 
// https://community.oracle.com/tech/developers/discussion/646205/problem-converting-number-to-sqlt-str-via-database-link
// if this is anything to go by this just represents a char* null perminated string and it can (probably) be used for various
// Oracle internal types that are (or can be) exported as characters
// So basically an OCI programmers type that you can represent any character data you can export...
// 99.999% certain you will not see this type returned from Oracle as a field type
         case _SQLT_STR:                 // EXT: STRING INT: ????? - this is a null terminated string which does not exist in Oracle
            OciConstant = "SQLT_STR";    // INT possibilities CHAR/NCHAR/LONG ??????? 
            return "SQLT_STR";
            break;
// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// https://docs.oracle.com/cd/B10501_01/appdev.920/a96584/oci11oty.htm// 
// Date (OCIDate)
// The Oracle date format is mapped in C by the OCIDate type, which is an opaque C struct. Elements of the struct 
// represent the year, month, day, hour, minute, and second of the date. The specific elements can be set and retrieved 
// using the appropriate OCI functions.
// The OCIDate datatype can be bound or defined directly using the external typecode SQLT_ODT in the bind or define call.
// The OCI date manipulation functions are listed in the following tables, which are organized according to functionality. 
// Unless otherwise specified, the term date in these tables refers to a value of type OCIDate.
// See Also:
// The prototypes and descriptions for all the functions are provided in Chapter 18, "OCI Datatype Mapping and Manipulation Functions".        
        case _SQLT_ODT:                  // EXT: OCI DATE LONG INT: 
            OciConstant = "SQLT_ODT";    // guess for mappiing - DATE
            return "SQLT_ODT";
            break;

// JULIAN not much useful info online but this seems to be a clue - looks to be another OCI programmers type
// https://docs.oracle.com/cd/B10501_01/appdev.920/a96584/oci20a19.htm
// OCIAnyDataTypeCodeToSqlt()
// This function converts OCI_TYPECODE_CHAR as well as OCI_TYPECODE_VARCHAR2 to SQLT_VST (which corresponds to the 
// OCIString mapping) with a charset form of SQLCS_IMPLICIT. OCI_TYPECODE_NVARCHAR2 will also return SQLT_VST 
// (OCIString mapping is used by the OCIAnyData API) with a charset form of SQLCS_NCHAR.
// For more information see "NCHAR Typecodes for OCIAnyData Functions"
        case _SQLT_VST:                  // EXT: OCI STRING LONG INT: 
            OciConstant = "SQLT_VST";
            return "SQLT_VST";
            break;
// JULIAN some info I found for SQLT_PDN & SQLT_SLS
// https://docs.oracle.com/cd/B10501_01/appdev.920/a96584/ociaahan.htm
// Attribute Datatype: sb4 *
// OCI_ATTR_PDPRC
// Specifies packed decimal precision. For SQLT_PDN values, the precision should be equal to 2*(value_sz-1). For SQLT_SLS values, 
// the precision should be equal to (value_sz-1).
//
// After a bind or define, this value is initialized to zero. The OCI_ATTR_PDPRC attribute should be set first, 
// followed by OCI_ATTR_PDSCL. If either of these values needs to be changed, a rebind/redefine should be done first, 
// and then the two attributes should be reset in order.
// JULIAN it looks like these are OCI types for programmers again... probably both map to NUMBER
       case _SQLT_PDN:                  // no mapping found (/* (ORANET TYPE) Packed Decimal Numeric */)
            OciConstant = "SQLT_PDN";   // best guess for mappiing - NUMBER
            return "SQLT_PDN";          // return = SQLT_NUM instead
            break;
        case _SQLT_SLS:                 // no mapping found (/* Display sign leading separate */)
            OciConstant = "SQLT_SLS";   // best guess for mappiing - NUMBER
            return "SQLT_SLS";          // return = SQLT_NUM instead
            break;
// JULIAN looks like a user defined type so more work on this later
        case _SQLT_NCO:                  // no mapping found (COLLECTION = OCI_TYPECODE_NAMEDCOLLECTION)
            OciConstant = "SQLT_NCO";
            return "SQLT_NCO";           //                  (/* named collection type (varray or nested table) */)
            break;
// REF CURSOR variables
// SQLT_RSET
// Allocate a statement handle, OCIStmt, and then bind its address, OCIStmt **, using the SQLT_RSET datatype.
// https://docs.oracle.com/cd/B19306_01/appdev.102/b14250/oci05bnd.htm
        case _SQLT_RSET:
            OciConstant = "SQLT_RSET";
            return "SQLT_RSET";          // no mapping found (/* result set type */)
            break;

        //------------------------------------------------------------------------------------------------------------
        // JULIAN TODO the OCI Types below unable to find obvious mapping (I tried)
        // I thnk these are Oracle "external types" that are supplied as a kind of "pattern" for OCI programmers to 
        // "help/guide"(?) them on how to map to Oracle internal types???? (= help by making things harder very Oracle :)
        //------------------------------------------------------------------------------------------------------------
        // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
        case _SQLT_CFILEE:               // no mapping found (/* character file lob */)
            OciConstant = "SQLT_CFILEE"; // guess for mappiing - BFILE (no CFILE type in Oracle = means treat BFILE as CHAR)
            return "SQLT_CFILEE";
            break;
         // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
         //        it may be possible that Oracle used to have a TIME type that is now obsolete???
        case _SQLT_TIME:                   // no mapping found (/* TIME */)
            OciConstant = "SQLT_TIME";     // guess for mappiing - TIMESTAMP
            return "SQLT_TIME";
            break;
         // JULIAN I think will never be returned from Oracle - it is an external type  that can be used by an OCI programmer
         //        it may be possible that Oracle used to have a TIME type that is now obsolete???
       case _SQLT_TIME_TZ:                // no mapping found (/* TIME WITH TIME ZONE */)
            OciConstant = "SQLT_TIME_TZ"; // guess for mappiing - TIMESTAMP WITH TIME ZONE
            return "SQLT_TIME_TZ";
            break;
        default:
            OciConstant = "UNKNOWN";
            return "UNKNOWN";
            break;
    }      
}

// JULIAN TODO ODBC types as well
COLstring DBoracleOdbcTypeAsString (oracle_odbc_field_types OracleOdbcType, COLstring &OracleOdbcConstant) {
    COL_FUNCTION(DBoracleOdbcTypeAsString);

    switch(OracleOdbcType) {          
   
        default:
            OracleOdbcConstant = "UNKNOWN";
            return "UNKNOWN";
            break;
    }
}
