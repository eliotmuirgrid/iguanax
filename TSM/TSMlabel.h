#ifndef __TSM_LABEL_H__
#define __TSM_LABEL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMlabel
//
// Description:
//
// TSMlabel class.  Utility class to store what are effectively const
// char* names but cache the length to avoid the need to call strlen
// repeatedly during serialization.  It's kind of COLstring light...
//
// It also has a numerical ID which is intended for use in serialization
// to binary formats where unique numerical IDs are used instead of strings. 
//
// NOTE: TSMlabel DOES NOT TAKE OWNERSHIP OF THE STRING USED FOR THE LABEL.
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 09:33:05 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <string.h>

class TSMlabel 
{
public:
   TSMlabel();
   // Lazy man's constructor - warning!!! COLstring must exceed lifetime of TSMlabel
   TSMlabel(const COLstring& String);   
   // Lazy man's constructor
   TSMlabel(const char* pString);  
   TSMlabel(int iId, const char* pString);
   TSMlabel(int iId, const char* pString, COLuint32 Size);
   // POD so assignment operator and copy constructor work

   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSMlabel();

   void setId(int iId);   
   void setString(const char* pString);
   void setStringWithSize(const char* pString, COLuint32 Size);   

   const int& id() const { return Id; }
   const char* label() const { return pLabel; }
   COLuint32 size() const { return SizeOfLabel; }
   
   bool operator==(const TSMlabel& Rhs) const;
   bool operator==(const char* pRhs) const;
   bool operator!=(const TSMlabel& Rhs) const;
   bool operator!=(const char* pRhs) const;   

private:
   COLuint32 SizeOfLabel;
   const char* pLabel;
   int Id;
};

COLostream& operator<<(COLostream& Stream, const TSMlabel& Label);

#endif // end of defensive include
