//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIFdiffer
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//---------------------------------------------------------------------------

#include "DIFprecomp.h"
#pragma hdrstop

#include "DIFdiffer.h"
#include <FIL/FILfile.h>
#include <FMT/FMTbinaryDump.h>

class DIFdifferPrivate
{
public:

};

DIFdiffer::DIFdiffer()
{
   pMember = new DIFdifferPrivate;
}

DIFdiffer::~DIFdiffer()
{
   delete pMember;
}


bool DIFdiffer::filesMatch
(
   const COLstring& FileName1,
   const COLstring& FileName2,
   COLostream& ErrStream
)
{
   FILfile File1;
   FILfile File2;

   File1.open(FileName1, FILfile::Read);
   File2.open(FileName2, FILfile::Read);
   if (File1.size() != File2.size())
   {
      if (File1.size() < File2.size())
      {
         File1.close();
         File2.close();
         File1.open(FileName2, FILfile::Read);
         File2.open(FileName1, FILfile::Read);
      }
   }
   COLstring Buffer1;  
   COLstring Buffer2;
   Buffer1.setCapacity(17);
   Buffer2.setCapacity(17);
   
   COLuint32 Position;

   COLuint32 AmountRead;
   do 
   {
      Position = File1.position();
      AmountRead = File1.read(&Buffer1);
      File2.read(&Buffer2);

      if (memcmp(Buffer1.c_str(), Buffer2.c_str(), AmountRead) != 0)
      {
         ErrStream << "Files '" << FileName1 << " and '" << FileName2 
         << "' differed in the range " << Position << " to " << Position + AmountRead << newline;
         ErrStream << "1: ";
         FMTbinaryDump(*ErrStream.sink(), (const COLuint8*)Buffer1.get_buffer(), AmountRead);
         ErrStream << "2: ";
         FMTbinaryDump(*ErrStream.sink(), (const COLuint8*)Buffer2.get_buffer(), AmountRead);

         return false;
      }
      
   } while (AmountRead > 0);
 
   if (File1.size() != File2.size())
   {
      ErrStream << "Files are different sizes";
      return false;
   }
   
   return true;
}

void DIFcompare(const COLstring& FileName1, const COLstring& FileName2)
{
   DIFdiffer Differ;
   if (!Differ.filesMatch(FileName1, FileName2, COLcout))
   {
      COL_ERROR_STREAM_PLAIN(FileName1 << " and " << FileName2 << " do not match.", COLerror::PreCondition);
   }
}
