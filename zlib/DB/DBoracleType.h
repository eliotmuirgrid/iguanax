//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBoracleType
//
// Description:
//
// Function(s) to return Oracle datatype names
//
//
// Author: Julian Muir
// Date:   Tue Jun 2 2021
//
//---------------------------------------------------------------------------

#include "DBdriverOracleOci.h"

class COLstring;

COLstring DBoracleOciTypeAsString (oci_field_types OciType, COLstring &OciConstant);

// JULIAN TODO - the data types are different for ODBC
COLstring DBoracleOdbcTypeAsString (oracle_odbc_field_types OracleOdbcType, COLstring &OracleOdbcConstant); 
