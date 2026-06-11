#ifndef __DB_SQL_COMMAND_TYPE_H__
#define __DB_SQL_COMMAND_TYPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCommandType
//
// Description:
//
// Pre-compiled header file.
//
// Author: Eliot Muir
// Date:   Fri 12/28/2007 
//
//---------------------------------------------------------------------------

enum DBsqlCommandType
{
   DB_SELECT_CMD,
   DB_SELECT_UNION_CMD,
   DB_UPDATE_CMD,
   DB_INSERT_CMD,
   DB_DELETE_CMD,
   DB_DROP_CMD,
   DB_CREATE_TABLE_CMD,
   DB_DEFAULT_CMD
};

#endif // end of defensive include
