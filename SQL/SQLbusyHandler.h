#ifndef __SQL_BUSY_HANDLER_H__
#define __SQL_BUSY_HANDLER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLbusyHandler
//
// Description:
//
// SQLbusyHandler class.
//
// Author: Nasron Cheong
// Date:   Thursday, December 14th, 2006 @ 02:41:36 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SQLbusyHandler{
public:
   SQLbusyHandler();
   virtual ~SQLbusyHandler() {}

   COLuint32 sqliteBusyInterval() const;
   void setSqliteBusyInterval(COLuint32 Interval);

   // Steps are the number of times this function has been called in the
   // current operation. Returning false interrupts the operation.
   // returning true will wait for interval() ms or no longer busy
   virtual bool onSqliteBusy(COLuint32 Step, bool AutoCommit) = 0;

private:
   COLuint32 Interval;   
   SQLbusyHandler(const SQLbusyHandler&);
   SQLbusyHandler& operator=(const SQLbusyHandler&);
};

#endif // end of defensive include
