//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMDtestStringArray.cpp
//
// Description:
//
// Implementation
//
// Author: Roman Leifer
// Date:   Tue 08/17/2004
//
//
//---------------------------------------------------------------------------

#include <CMDexample/CMDexamplePrecomp.h>
#include <COL/COLstring.h>
#pragma hdrstop

#include <CMDexample/CMDtestStringArray.h>

CMDtestStringArray::CMDtestStringArray(const CMDtestStringArray& Rhs)
{
   Vector.clear();
   for (COLuint32 Index = 0; Index < Rhs.Vector.size(); Index++)
   {
      Vector.push_back(Rhs.Vector[Index]);
   }
}
const char** CMDtestStringArray::getArray()
{
   Array.resize(Vector.size());
   for (COLuint32 Index = 0; Index < Vector.size(); Index++)
   {
      Array[Index] = Vector[Index].c_str();
   }
   return Array.getArray();
}

void CMDtestStringArray::addCommandLine(const COLstring& CommandLine)
{

   COLstring SubCommandLine = CommandLine;
   //if there are no spaces, only one param
   if(SubCommandLine.find(' ') == npos)
   {
      Vector.push_back(SubCommandLine);
   }
   else
   {
      while(SubCommandLine.find(' ') != npos)
      {
         COLstring Param;
         SubCommandLine.split(Param, SubCommandLine, " ");
         Vector.push_back(Param);
      }

      Vector.push_back(SubCommandLine);
   }
}

bool CMDtestStringArray::has(const COLstring& String)
{
   for(COLuint32 StringIndex = 0; StringIndex < Vector.size(); StringIndex++)
   {
      if(Vector[StringIndex].find(String) != npos)
         return true;
   }
   return false;
}
