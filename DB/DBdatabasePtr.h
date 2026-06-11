#ifndef __DB_DATABASE_PTR_H__
#define __DB_DATABASE_PTR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabasePtr
//
// Description:
//
// Reference counted database smart pointer class.  This is used in DB2
// to handle ownership issues.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------

#include <COL/COLref.h>
#include "DBdatabase.h"

class DBdatabasePtr : public COLref< DBdatabase >
{
public:
   DBdatabasePtr(DBdatabase* pReference=NULL) : COLref<DBdatabase>(pReference) {}
};

#endif // end of defensive include
