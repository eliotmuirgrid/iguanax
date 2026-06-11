#ifndef __TRK_TICKET_LINKER_H__
#define __TRK_TICKET_LINKER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRKticketLinker
//
// Description:
//
// TRKticketLinker class.
//
// The idea of this class is to encapsulate the logic that
// many of our internal applications need - namely to hyper
// link tickets which are mentioned in the subject lines of emails
// and CVS commit lines.
//
// This initial version is intended to do a simple hyperlink only.  One
// day however it would be nice to have little tool-tips pop up over the
// hyperlinked tickets since this would give a lot more context.  This
// class would make implementing that logic easier and more efficient
// since multiple SQLite select queries would need to be implemented 
// to obtain the description of each ticket.  Hopefully this interface
// will allow this logic to be implemented sufficiently efficiently.
// (i.e. we probably want to cache the result of each select - if we
// were fanatical about efficiency we would probably need to do something
// like pre-parsing all the lines we want to escape to get a select statement
// for all the tickets we want etc.  - but it's probably not necessary.
//
// Note to add the above functionality more properties would be required
// like giving the path to the Trak database etc.a
//
// With respect to hard coded URLs like http://ifware.dynip.com/cgi-bin/trac.cgi/ticket/
// this can be changed inside the implementation of the class.  For the very long term
// we might want to want to soft code this as a property and pass it in but not
// important right now.
//
// Author: Eliot Muir
// Date:   Monday, January 23rd, 2006 @ 08:11:05 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

 /* Constant strings used to generate the html link */
const COLstring TRK_BEGINING_OF_LINK = "<a href=\"http://fogbugz.ifware.dynip.com/default.asp?";
const COLstring TRK_SLASH_AND_HASH = "\" target=\"trak\">#";
const COLstring TRK_END_OF_TAG = "</a>";

class TRKticketLinkerPrivate;

class TRKticketLinker 
{
public:
   TRKticketLinker();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TRKticketLinker();

   // This method will find all the tickets referenced - i.e. with #<ticket no>
   // and translate them to hyperlinks to the ticket in Trak.  Note that this
   // method is *not* const - this is because later implementations of TRKticketLinker
   // could in fact have their state modified by this method if it is caching say
   // description information for each ticket.
   COLstring linkString(const COLstring& Line);

   // This method converts characters such as >, <, and & into their HTML safe equivalents such
   // as &gt;, &lt;, and &amp;.  Modified slightly from CStemplate.cpp.
   COLstring htmlEscape(const COLstring& pInput);

private:
   TRKticketLinkerPrivate* pMember;
   TRKticketLinker(const TRKticketLinker& Orig); // not allowed
   TRKticketLinker& operator=(const TRKticketLinker& Orig); // not allowed
   
};

#endif // end of defensive include
