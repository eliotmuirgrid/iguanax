//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGerror
//
// Description:
//
// Implementation
//
// Author: Luke Walker
// Date:   Monday, March 18th, 2013 @ 03:49:13 PM
//
//---------------------------------------------------------------------------
#include "LEGprecomp.h"
#pragma hdrstop

#include "LEGerror.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

LEGerror::LEGerror( const COLstring& Message/*=""*/, COLuint32 Code/*=COLerror::PreCondition*/ ) :
   COLerror(Message, Code)
{}

LEGerror::LEGerror( const LEGerror& Error ) :
   COLerror(Error)
{
   for (COLmapPlace ListPlace = Error.Parameters.first();
      ListPlace != NULL;
      ListPlace = Error.Parameters.next(ListPlace))
   {
      Parameters.add(Error.Parameters.key(ListPlace), Error.Parameters[ListPlace]);         
   }
}

LEGerror::LEGerror( const COLerror& Error ) :
   COLerror(Error)
{}

LEGerror::LEGerror( const LEGerror& Error, COLuint32 Code ) :
   COLerror(Error, Code)
{
   for (COLmapPlace ListPlace = Error.Parameters.first();
      ListPlace != NULL;
      ListPlace = Error.Parameters.next(ListPlace))
   {
      Parameters.add(Error.Parameters.key(ListPlace), Error.Parameters[ListPlace]);         
   }
}

LEGerror::LEGerror( const COLerror& Error, COLuint32 Code ) :
   COLerror(Error, Code)
{}

void LEGerror::setParameter( const COLstring& Key, const COLstring& Value )
{
   COLmapPlace ListPlace = Parameters.find(Key);
   if (ListPlace)
   {
      Parameters[ListPlace] = Value;
   }
   else
   {
      Parameters.add(Key,Value);
   }
}

void LEGerror::setParameter( const COLstring& Key, COLuint32 Value )
{
   // Not so efficient but this doesn't get called much? Eliot
   COLstring StringValue;
   COLostream Stream(StringValue);
   Stream << Value;
   setParameter(Key, StringValue);
}

bool LEGerror::hasParameter( const COLstring& ParameterName ) const
{
   return Parameters.find(ParameterName) != NULL;
}

// Legacy interface so we have to have this interface - it really isn't efficient
// but it's only used by some customers in some error situations and therefore seems
// an okay hack - Eliot
const COLstring& LEGerror::key(int KeyIndex) const
{
   if (KeyIndex <0 || KeyIndex >= CountOfParameter())
   {
      COL_ERROR_STREAM_PLAIN(COL_T("Invalid parameter index ") << KeyIndex << COL_T(".  Should be no greater than ") << CountOfParameter(), 
         COLerror::PreCondition);
   }
   COLmapPlace ListPlace = Parameters.first();
   for (;KeyIndex !=0; KeyIndex--) 
   {
      ListPlace = Parameters.next(ListPlace);       
   }
   return Parameters.key(ListPlace);
}

const COLstring& LEGerror::parameter(const COLstring& Key)
{
   COLmapPlace ListPlace = Parameters.find(Key);
   if (!ListPlace)
   {
      // Legacy interface so we have to do this stuff
      ListPlace = Parameters.add(Key, COLstring());
   }
   return Parameters[ListPlace];
}

int LEGerror::CountOfParameter() const
{
   return Parameters.size();
}

COLindex LEGerror::firstParameter() const
{
   return (COLindex)Parameters.first();
}

COLindex LEGerror::nextParameter(COLindex Place) const
{
   return (COLindex)Parameters.next((COLmapPlace)Place);
}

const COLstring& LEGerror::keyInPlace(COLindex Place) const
{
   return Parameters.key((COLmapPlace)Place);
}

const COLstring& LEGerror::parameterInPlace(COLindex Place) const
{
   return Parameters[(COLmapPlace)Place];
}

LEGerror& LEGerror::operator=( const LEGerror& Error )
{
   COLerror::operator=(Error);

   Parameters.clear();
   for (COLmapPlace ListPlace = Error.Parameters.first();
      ListPlace != NULL;
      ListPlace = Error.Parameters.next(ListPlace))
   {
      Parameters.add(Error.Parameters.key(ListPlace), Error.Parameters[ListPlace]);         
   }

   return *this;
}

bool LEGerror::operator==( const LEGerror& Rhs ) const
{
   if (!COLerror::operator==(Rhs))
   {
      return false;
   }

   // Parameters
   if (Parameters.size() != Rhs.Parameters.size())
   {
      return false;
   }

   COLmapPlace RhsListPlace = Rhs.Parameters.first();
   for (COLmapPlace LhsListPlace = Parameters.first(); 
      LhsListPlace != NULL; LhsListPlace = Parameters.next(LhsListPlace))
   {
      if (Parameters.key(LhsListPlace) != Rhs.Parameters.key(RhsListPlace)
         || Parameters[LhsListPlace] != Rhs.Parameters[RhsListPlace])
      {
         return false;
      }

      RhsListPlace = Rhs.Parameters.next(RhsListPlace);
   }

   return true;
}

COLostream& operator<<(COLostream& Stream, const LEGerror& Error)
{
   Stream << static_cast<const COLerror&>(Error); // call the COLerror insertion operator

   if (Error.CountOfParameter())
   {
      for (COLindex ListPlace = Error.firstParameter();
         ListPlace != 0;
         ListPlace = Error.nextParameter(ListPlace))
      {
         Stream << Error.keyInPlace(ListPlace) << COL_T(": ") << Error.parameterInPlace(ListPlace) << newline;         
      }
   }

   return Stream;
}
