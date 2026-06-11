#ifndef __DB_SQL_COMMAND_SEQUENCE_H__
#define __DB_SQL_COMMAND_SEQUENCE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCommandSequence
//
// Description:
//
// Simple collection that allows for a sequence of commands to be executed.
// Note that this is NOT TRANSACTIONAL. The commands are executed one after another, 
// and an exception is raised from the first command to fail
//
// Author: Nasron Cheong
// Date:   May 17, 2004
//
//
//---------------------------------------------------------------------------
class DBsqlCommand;
class DBsqlCommandSequencePrivate;

template <class TItem>
class COLref;

class DBsqlCommandSequence : public COLrefCounted
{
public:
   DBsqlCommandSequence();
   virtual ~DBsqlCommandSequence();

   // This takes ownership
   DBsqlCommand* push_back(DBsqlCommand* pNewCommand);

   //returns count of result set
   COLuint32 countOfSqlCommand() const;

   //returns a command
   const DBsqlCommand& sqlCommand(COLuint32 CommandIndex) const;
   DBsqlCommand& sqlCommand(COLuint32 CommandIndex);

   //removes all commands
   void clear();

   //appends another command sequence to the this one
   //void append(const DBsqlCommandSequence& Next);

private:
   DBsqlCommandSequencePrivate* pMember;

   DBsqlCommandSequence( DBsqlCommandSequence& Orig ); // not allowed
   DBsqlCommandSequence& operator=( DBsqlCommandSequence& Orig ); // not allowed
};

// A smart pointer type to handle result set ownership issues through
// reference counting.

// TODO: Can this be moved into the cpp file - then the above includes can be replaced
//       with pre-decl's to improve compile time      
typedef COLref< DBsqlCommandSequence > DBsqlCommandSequencePtr;

#endif // end of defensive include
