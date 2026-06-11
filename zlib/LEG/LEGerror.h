#ifndef __LEG_ERROR_H__
#define __LEG_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEGerror
//
// Description:
//
// A subclass of COLerror that has the parameter list functions that used to
// be in COLerror. This functionality is needed for a fair number of legacy
// projects used in Iguana and Chameleon components.
//
// Author: Luke Walker
// Date:   Monday, March 18th, 2013 @ 03:49:13 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLerror.h>
#include <COL/COLstring.h>
#include <COL/COLmap.h>

class LEGerror : public COLerror
{
public:
   LEGerror(const COLstring& Message="", COLuint32 Code=COLerror::PreCondition);
   LEGerror(const LEGerror& Error);
   LEGerror(const COLerror& Error);
   LEGerror(const LEGerror& Error, COLuint32 Code);
   LEGerror(const COLerror& Error, COLuint32 Code);

   // Legacy interface - Eliot
   void setParameter( const COLstring& Key, const COLstring& Value );
   void setParameter( const COLstring& Key, COLuint32 Value );
   bool hasParameter( const COLstring& ParameterName ) const;

   // WARNING - really inefficient interface for iteration - here for backward compatibility
   const COLstring& key(int KeyIndex) const;
   const COLstring& parameter( const COLstring& Key );
   int CountOfParameter() const;

   // Newer more efficient interface for iterating through parameters
   COLindex firstParameter() const;
   COLindex nextParameter(COLindex Place) const;
   const COLstring& keyInPlace(COLindex Place) const;
   const COLstring& parameterInPlace(COLindex Place) const;

   virtual LEGerror& operator=(const LEGerror& Error);
   // Used in SGCexample to validate the parameter lists of exceptions thrown
   // from SGC.
   virtual bool operator==(const LEGerror& Rhs) const;

private:
   COLmap<COLstring, COLstring> Parameters;
};

COLostream& operator<<(COLostream& Stream, const LEGerror& Error);

#endif // end of defensive include
