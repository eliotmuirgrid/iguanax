#ifndef __TRK_TICKET_ITERATOR_H__
#define __TRK_TICKET_ITERATOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRKticketIterator
//
// Description:
//
// TRKticketIterator class.
//
// This class is used to iterate through a string that has tickets
// in the form #<ticket no>.  The class is used to implement TRKticketLinker
// but also for applications like mail_filter for parsing ticket numbers out
// of email subject lines. 
//
// Usage:
//   // the nextTicket function returns whether or not a ticket has been found
//   // past the currently stored EndOfTicket index.  If a ticket has been found
//   // it changes the values of StartOfTicket and EndOfTicket to reflect the right index
//
// TRKticketIterator TicketIterator("#123, #455 Fixed terrible bug");
//
// COLint32 TicketNumber;
// COLuint32 StartOfTicket, EndOfTicket;
// while (TicketIterator.nextTicket(StartOfTicket, EndOfTicket))
// {

//   // We get two iterations of the loop with true and true returned.
//   // After that, the next time it runs we get false.
//   // StartOfTicket and EndOfTicket will point to the index that is the begining
//   // of the ticket number 1 (Value: 1), and the end of the ticket 3 (Value: 3) for the first case.
//   // For the second case they shall contain 7 (Value: 4) and 9 (Value: 5).
//   //   ...
//   // We keep calling nextTicket so long as there is another ticket in the string,
//   // that is to say, until "TicketIterator.nextTicket(StartOfTicket, EndOfTicket) = false".
// }
//
// Author: Eliot Muir
// Date:   Tuesday, January 24th, 2006 @ 08:28:52 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// This constant is used by the mail_indexer and mail_filter programs to distinguish between
// spam and our emails that actually correspond to tickets 
#define TRK_MAX_TICKET_LENGTH 5


class TRKticketIteratorPrivate;

class TRKticketIterator 
{
public:
   TRKticketIterator();   
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TRKticketIterator();

   // This resets the iterator to parse the string passed in this method
   void setLine(const COLstring& Line);

   // see above for usage
   bool nextTicket(COLuint32& StartOfTicket, COLuint32& EndOfTicket);
private:
   TRKticketIteratorPrivate* pMember;
   TRKticketIterator(const TRKticketIterator& Orig); // not allowed
   TRKticketIterator& operator=(const TRKticketIterator& Orig); // not allowed
};

#endif // end of defensive include
