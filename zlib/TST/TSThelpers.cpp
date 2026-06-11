//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSThelpers
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Friday, September 8th, 2006 @ 11:25:34 AM
//---------------------------------------------------------------------------
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSThelpers.h"
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#   include <direct.h>
#   include <winbase.h>
#   include <io.h>
#   define TSTaccess _access
#   define FILexistsPermissions 0
#else
#   include <unistd.h>
#   include <stdlib.h>
#   include <pwd.h>
#   define TSTaccess  access
#   define FILexistsPermissions F_OK
#endif // _WIN32

bool TSTcompareException(const COLerror& Lhs, const COLerror& Rhs)
{
   //description
   return (Lhs == Rhs);
}

//To avoid FIL dependency
bool TSTfileExists(const COLstring& FileName)
{
   int Result = TSTaccess(FileName.c_str(), FILexistsPermissions);
   return Result == 0; 
}

//To avoid FIL dependency
void TSTfileReadLines(const COLstring& FileName, COLarray<COLstring>& Lines)
{
   FILE* FileHandle = ::fopen(FileName.c_str(), "rb");
   if (NULL == FileHandle)
   {
      // we have a file error
      COL_THROW_STRERROR("Open file '" << FileName << "' failed");
   }
   COLstring CurrentLine;
   int CurrentChar = ::fgetc(FileHandle);
   while (CurrentChar != EOF)
   {
      CurrentLine += (char)CurrentChar;
      if (CurrentLine[CurrentLine.size()-1] == '\n')
      {
         Lines.push_back(CurrentLine);
         CurrentLine.clear();
      }
      CurrentChar = ::fgetc(FileHandle);
   }
   //flush last line
   if (CurrentLine.size())
   {
      Lines.push_back(CurrentLine);
   }
   ::fclose(FileHandle);
}

//To avoid FIL dependency
void TSTfileWrite(const COLstring& FileName, const COLstring& Data)
{
   FILE* FileHandle = ::fopen(FileName.c_str(), "wb+");
   if (NULL == FileHandle)
   {
      // we have a file error
      COL_THROW_STRERROR("Open file '" << FileName << "' failed");
   }
   size_t AmountWritten = ::fwrite(Data.c_str(),1,Data.size(),FileHandle);
   if (AmountWritten != Data.size())
   {
      ::fclose(FileHandle);
      // we have a file error
      COL_THROW_STRERROR("::fwrite to file '" << FileName << "' failed");      
   }
   ::fflush(FileHandle);
   ::fclose(FileHandle);
}

// This compare function attempts to do a string comparison that won't be perturbed by meaningless
// \r\n \n etc. differences in line feed characters on different platforms.  Hopefully it should make
// out unit tests less brittle.
void TSTequalStrings(const COLstring& ExpectedInput, const COLstring& ActualInput){
   COL_FUNCTION(TSTequalStrings);
   COLstring Expected = ExpectedInput;
   COLstring Actual = ActualInput;
   Expected.replace("\r", "");  // This converts \r\n into just \n
   Actual.replace("\r", "");    // This converts \r\n into just \n
   Expected = Expected.strip(COLstring::Trailing, '\n');   // Strip spurious trailing newlines
   Actual = Actual.strip(COLstring::Trailing, '\n');       // Strip spurious trailing newlines.
   if (Expected != Actual){
      COL_ERR("Expected result and actual result different.");
      COL_DUMP("Expected:", Expected.c_str(), Expected.size());
      COL_DUMP("Actual:  ",   Actual.c_str(),   Actual.size());
      COLstring Output;
      COLostream Stream(Output);
      Stream << "Expected != Actual" << newline
             << "Expected:" << newline
             << Expected << newline
             << "Actual:" << newline
             << Actual << newline;
      throw COLerror(Output, COLerror::PreCondition);
   }
}
