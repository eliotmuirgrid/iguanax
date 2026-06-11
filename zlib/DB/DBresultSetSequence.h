#ifndef __DB_RESULT_SET_SEQUENCE_H__
#define __DB_RESULT_SET_SEQUENCE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSetSequence
//
// Description:
//
// Wrapper for a vector of DBresultSets. 
//
// Author: Nasron Cheong
// Date:   May 17, 2003 
//
//
//---------------------------------------------------------------------------

#include <COL/COLrefCounted.h>       
#include <COL/COLref.h>
#include "DBresultSetPtr.h"

class DBresultSetSequencePrivate;

class DBresultSetSequence : public COLrefCounted
{
public:
   DBresultSetSequence(); 
   virtual ~DBresultSetSequence();

   DBresultSetPtr push_back(DBresultSetPtr pNewResultSet);

   //returns count of result set
   COLuint32 countOfResultSet() const;

   //returns a result set
   DBresultSet& resultSet(COLuint32 SetIndex);

   //removes all results
   void clear();

private:
   DBresultSetSequencePrivate* pMember;

   DBresultSetSequence( DBresultSetSequence& Orig ); // not allowed
   DBresultSetSequence& operator=( DBresultSetSequence& Orig ); // not allowed
};

/**
 * A smart pointer type to handle result set ownership issues through
 * reference counting.
 */

// TODO: Can this be moved into the cpp file - then the above includes can be replaced
//       with pre-decl's to improve compile time      
typedef COLref< DBresultSetSequence > DBresultSetSequencePtr;

#endif // end of defensive include
