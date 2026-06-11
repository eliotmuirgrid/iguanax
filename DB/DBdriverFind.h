#ifndef __DB_DRIVER_FIND_H__
#define __DB_DRIVER_FIND_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdriverFind
//
// Description:
//
// Routines that locate the location of DLL/SO drivers
//
// Author: Eliot Muir
// Date:   Wed 14 Apr 2021 16:27:25 EDT
//---------------------------------------------------------------------------

template<typename COLstring> class COLlist; 

class COLstring;
class COLostream;

// This function will attempt to find the MySQL dll/so using various file system heuristics 
bool DBfindMySql(COLstring* pDriverPathResult, COLostream& LogStream);

// This function will attempt to find the MariaDB dll/so using various file system heuristics 
bool DBfindMariaDb(COLstring* pDriverPathResult, COLostream& LogStream);

// This function will attempt to find the ODCB dll/so using various file system heuristics 
bool DBfindOdbc(COLstring* pDriverPathResult, COLostream& LogStream);

// This function will attempt to find the Oracle OCI dll/so using various file system heuristics 
bool DBfindOracleOci(COLstring* pDriverPathResult, COLostream& LogStream);

#endif // end of defensive include
