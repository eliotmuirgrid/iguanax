#ifndef __DB_SQL_COMMAND_H__
#define __DB_SQL_COMMAND_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCommand
//
// Description:
//
// Abstract base class for the supported SQL commands.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include "DBsqlCommandType.h"

class DBsqlCommand
{
public:
   DBsqlCommand() {}
   // We'll assume that DBsqlCommand is POD so we can copy them.
   virtual ~DBsqlCommand() {}

   virtual DBsqlCommandType commandType() const=0;
};

#endif // end of defensive include
