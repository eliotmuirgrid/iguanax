//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSpecificationParser
//
// Description:
//
// Description: Parses a string into a testSpecification
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestSpecificationParser.h"
#include "TSTtestSpecification.h"
#include "TSTtestableOptions.h"

class TSTtestSpecificationParserPrivate
{
public:

   void addPath(const COLstring& String, TSTtestSpecification& ReturnSpec)
   {
      ReturnSpec.pushBackPath(String.strip(COLstring::Both));
   }

   void makeExpression(const COLstring& String,TSTtestableOptions& Options)
   {
      COLstring ExpStr = String.strip(COLstring::Both);
      if (ExpStr.size() < 3 ) // min is [*]
      {
         COL_ERROR_STREAM("Invalid expression "<< ExpStr,COLerror::PreCondition);
      }
      if (ExpStr[0] != '[')
      {
         COL_ERROR_STREAM("Expected [ in "<< ExpStr,COLerror::PreCondition);
      }
      if (ExpStr[ExpStr.size()-1] != ']')
      {
         COL_ERROR_STREAM("Expected ] in "<< ExpStr,COLerror::PreCondition);
      }
      ExpStr = ExpStr.substr(1,ExpStr.size()-2);
      ExpStr = ExpStr.strip(COLstring::Both);
      if (ExpStr == "*")
      {
         Options.setRunAll(true);
      }
      else
      {
         Options.setRunAll(false);
         COLstring Lhs,Rhs;
         ExpStr.split(Lhs,Rhs," and");
         if (!Rhs.size())
         {
            COLstring CurrentItem = Lhs.strip(COLstring::Both);
            if (Lhs.find("*") != npos)
            {
               COL_ERROR_STREAM("Unexpected * in " << String,COLerror::PreCondition);
            }
            if (CurrentItem.size())
            {
               Options.addSubTest(CurrentItem);
            }
         }
         
         while (Rhs.size())
         {
            COLstring CurrentItem = Lhs.strip(COLstring::Both);
            if (Lhs.find("*") != npos)
            {
               COL_ERROR_STREAM("Unexpected * in " << String,COLerror::PreCondition);
            }
            if (CurrentItem.size())
            {
               Options.addSubTest(CurrentItem);
            }
            CurrentItem = Rhs;
            CurrentItem.split(Lhs,Rhs," and");
            if (!Rhs.size() && Lhs.size())
            {
               COLstring CurrentItem = Lhs.strip(COLstring::Both);
               if (Lhs.find("*") != npos)
               {
                  COL_ERROR_STREAM("Unexpected * in " << String,COLerror::PreCondition);
               }
               if (CurrentItem.size())
               {
                  Options.addSubTest(CurrentItem);
               }
            }
            else if (!Rhs.size() && !Lhs.size())
            {
               COL_ERROR_STREAM("Unexpected statement after 'and' in " << String,COLerror::PreCondition);
            }
         }
      }

   }
};

TSTtestSpecificationParser::TSTtestSpecificationParser()
{
   pMember = new TSTtestSpecificationParserPrivate;
}

TSTtestSpecificationParser::~TSTtestSpecificationParser()
{
   delete pMember;
}

void TSTtestSpecificationParser::createSpecification(const COLstring& String, TSTtestSpecification& ReturnSpec)
{
   try
   {
      ReturnSpec.clear();
      COLstring Lhs,Rhs;
      String.split(Lhs,Rhs,"/");
      while (Rhs.size())
      {
         if (Lhs.size())
         {
            pMember->addPath(Lhs.strip(COLstring::Both),ReturnSpec);         
         }
         COLstring Current = Rhs;
         Current.split(Lhs,Rhs,"/");
      }
      pMember->makeExpression(Lhs,ReturnSpec.testOptions());   
   }
   catch (COLerror&) 
   {
      ReturnSpec.clear();
      throw;
   }
}
