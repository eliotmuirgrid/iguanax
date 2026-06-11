#ifndef __DB_ESCAPE_H__
#define __DB_ESCAPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBescape
//
// Description:
//
// DBescape - all the SQL string escaping logic is factored out in to this
// part of the library.  This makes a little easier to follow and easier
// to use for other things
//
// Author: Eliot Muir
// Date:   Thursday, September 9th, 2010 @ 05:18:20 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

typedef COLostream& (*DBescapeFunction)(COLostream& Stream, const COLstring& UnescapedString);

// This one works for some databases - probably Oracle and I am not sure which other ones.
COLostream& DBescapeSql(COLostream& Stream, const COLstring& UnescapedString);

// SQL SERVER needs an N in front of string for unicode mode as does Oracle
COLostream& DBescapeSqlWithUnicode(COLostream& Stream, const COLstring& UnescapedString);

// Postgres is special too!
COLostream& DBescapePostgresSql(COLostream& Stream, const COLstring& UnescapedString);

// MYSQL has it's own escaping routine.
COLostream& DBescapeMySql(COLostream& Stream, const COLstring& UnescapedString);

// MARIADB is initially a copy on MYSQL – but we want to separate concerns.
COLostream& DBescapeMariaDb(COLostream& Stream, const COLstring& UnescapedString);



#endif // end of defensive include
