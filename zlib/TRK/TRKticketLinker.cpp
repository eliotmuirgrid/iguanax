//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRKticketLinker
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, January 23rd, 2006 @ 08:11:05 PM
//
//---------------------------------------------------------------------------
#include <TRK/TRKprecomp.h>
#pragma hdrstop

#include <TRK/TRKticketLinker.h>
#include <TRK/TRKticketIterator.h>

class TRKticketLinkerPrivate
{
public:
};

// Constructor
TRKticketLinker::TRKticketLinker()
{
   pMember = new TRKticketLinkerPrivate;
}

// Destructor
TRKticketLinker::~TRKticketLinker()
{
   delete pMember;
}

// Main function -- Given a string with ticket numbers in it
// returns the string with ticket numbers as hyper links
// (actual hyperlink code put into the string)
COLstring TRKticketLinker::linkString(const COLstring& Line)
{
   // we need the ticket iterator to tell us the positions of the tickets
   TRKticketIterator TicketIterator;
   TicketIterator.setLine(Line);
   
   // Create and instantiate the required variables
   COLstring TicketNumber, TemporaryString;
   COLostream TemporaryStringStream(TemporaryString);
   COLuint32 StartOfTicket = 0, EndOfTicket = 0, IndexOfLastUpdate = 0;

   if (TicketIterator.nextTicket(StartOfTicket,EndOfTicket))
   {
      do 
      {
         // Get the ticket #
         TicketNumber = Line.substr(StartOfTicket, EndOfTicket - StartOfTicket+1);
        
         // Replace the TicketNumber with the LinkedTicketNumber in a separate temporary string
         // This string is appended to whenever a valid ticket is found
         // Although this means the copy constructor is called multiple times
         // it is unavoidable
         TemporaryStringStream << Line.substr(IndexOfLastUpdate, StartOfTicket - IndexOfLastUpdate - 1)
            << TRK_BEGINING_OF_LINK << TicketNumber << TRK_SLASH_AND_HASH + TicketNumber << TRK_END_OF_TAG;
        
         // we have to keep an index of the last time data was dumped into the temporary string
         // that way, the next time there is a data dump, it encompasses everything in between the
         // ticket numbers as well
         IndexOfLastUpdate = EndOfTicket+1;
         
      } while (TicketIterator.nextTicket(StartOfTicket,EndOfTicket) == true);
      
      // Finally copy the last remaining subsection (if any exists)
      TemporaryString = TemporaryString + htmlEscape(Line.substr(EndOfTicket + 1));

      return TemporaryString;
   }
   else
   {
      return Line; 
   }
}

// Helper function for linkString -- converts HTML-unsafe characters into their safe equivalents.
COLstring TRKticketLinker::htmlEscape(const COLstring& pInput)
{
   COLstring EscapedString(pInput);
   EscapedString.replace("&", "&amp;");
   EscapedString.replace("<", "&lt;");
   EscapedString.replace(">", "&gt;");
   EscapedString.replace("\"", "&quot;");
   return EscapedString;
}
