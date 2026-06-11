#ifndef __SQL_ERROR_H__
#define __SQL_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLerror
//
// Description:
//
// SQLerror class.
//
// Author: Nasron Cheong
// Date:   Thursday, May 11th, 2006 @ 11:23:30 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SQLsqlite;

//thrown errors should contain a parameter in the 
//colerror containing the underlying sqlite code
COLstring SQLioErrorCode();
COLstring SQLschemaChangedErrorCode();
COLstring SQLdiskFullErrorCode();

COLstring SQLerrorCodeKey();

//note that the 4th parameter will be freed if not null
void SQLthrowSqliteError(
   void* pSqliteHandle, 
   const COLstring& FilePath,
   const COLstring& ContextString,
   const COLstring& Statement,
   char* pExecSqliteError = 0
);

void SQLthrowSqliteErrorImpl
( 
 const COLstring& FilePath, 
 const COLstring& ContextString, 
 const COLstring& Statement, 
 const COLstring& Error, 
 const COLstring& ErrorExtended, 
 COLint32 SqliteReturnCode
);   

bool SQLstatementIsCommit(const COLstring& In);

#endif // end of defensive include
