#ifndef __DB_RESULT_SET_PTR_H__
#define __DB_RESULT_SET_PTR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSetPtr
//
// Description:
//
// Reference counted result set smart pointer class.  This is used in DB2
// to handle ownership issues.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------

#include <COL/COLref.h>
#include "DBresultSet.h"

class DBresultSetPtr : public COLref< DBresultSet >
{
public:
   DBresultSetPtr(DBresultSet* pReference=NULL) : COLref<DBresultSet>(pReference) {}
};

#endif // end of defensive include
