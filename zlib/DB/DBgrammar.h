#ifndef __DB_GRAMMAR_H__
#define __DB_GRAMMAR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBgrammar
//
// Description:
//
// Simple POD model for database grammar.
//
// Author: Eliot Muir 
// Date:   Monday 07 November 2022 - 12:57PM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <COL/COLmap.h>
#include <COL/COLstring.h>


enum DBsimpleType{
   DBS_STRING=0,
   DBS_INTEGER=1,
   DBS_DOUBLE=2,
   DBS_DATETIME=3
};

class DBcolumn{
public:
   DBcolumn(const COLstring& Name, const COLstring& Description, DBsimpleType Type, bool IsKey);
   COLstring Name;
   COLstring Description;
   bool IsKey;
   DBsimpleType Type;
};


class DBtable{
public:
   COLstring Description;
   COLarray<DBcolumn> Columns;
};


class DBcollection{
public:
   void clear();
   COLmap<COLstring, DBtable> Tables;
   COLmap<COLstring, COLstring> Export;
};


#endif // end of defensive include
