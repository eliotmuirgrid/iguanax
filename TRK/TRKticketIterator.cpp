//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRKticketIterator
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, January 24th, 2006 @ 08:28:52 AM
//
//---------------------------------------------------------------------------
#include <TRK/TRKprecomp.h>
#pragma hdrstop

#include <TRK/TRKticketIterator.h>
#include <ctype.h>    // required so that we can use the isDigit function
#include <string.h>   // we use the strlen function

class TRKticketIteratorPrivate
{
public:
   TRKticketIteratorPrivate() 
    : CharIndex(-1) {}

   bool nextTicket(COLuint32& StartOfTicket, COLuint32& EndOfTicket);

   void detectTicket(COLuint32& StartOfTicket, COLuint32& EndOfTicket);

   const char* pTicketString;  // To hold the string you are iterating through
   COLuint32 CharIndex;

   void setCharIndex(COLuint32 EndOfTicketIndex);
};

void TRKticketIteratorPrivate::detectTicket(COLuint32& StartOfTicket, COLuint32& EndOfTicket)
{
   // CharIndex should be pointing at the first digit of the ticket
   // record start
   StartOfTicket = CharIndex;
   // iterate forward until we don't have a digit - this is the end of ticket
   do 
   {
      CharIndex++;   
   } while(isdigit(pTicketString[CharIndex]));
   // record end 
   EndOfTicket = CharIndex-1;
   // and return
}

// Constructor
TRKticketIterator::TRKticketIterator()
{
   pMember = new TRKticketIteratorPrivate;
}

// Destructor
TRKticketIterator::~TRKticketIterator()
{
   delete pMember;
}

// Accessors Function for creating a new line in the object
void TRKticketIterator::setLine(const COLstring& Line)
{
   pMember->pTicketString = Line.c_str();
   pMember->CharIndex = 0;
}

// Main logical function
bool TRKticketIteratorPrivate::nextTicket(COLuint32& StartOfTicket, COLuint32& EndOfTicket)
{
   
   COLPRECONDITION(-1 != CharIndex);
   
   // checking for empty string or a null string
   if ( (pTicketString == 0) || (::strlen(pTicketString) ==0))
   {
      return false;
   }

   if (*pTicketString != '\0')
   {
      while (pTicketString[CharIndex] != '\0') 
      {
         if (pTicketString[CharIndex] == '#')
         {
            // character array boundary check
            if (CharIndex < unsigned(::strlen(pTicketString)) )
            {
               if (isdigit(pTicketString[CharIndex+1]))
               {
                  CharIndex++;
                  detectTicket(StartOfTicket, EndOfTicket);
                  return true;
               }
               else
               {
                  // we have some garbage so we ignore it
               }
            }
         }
         CharIndex++;
      }
   }
   return false;   
}

bool TRKticketIterator::nextTicket(COLuint32& StartOfTicket, COLuint32& EndOfTicket)
{
   return pMember->nextTicket(StartOfTicket, EndOfTicket);
}

