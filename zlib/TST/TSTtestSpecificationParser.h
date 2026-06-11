#ifndef __TST_TEST_SPECIFICATION_PARSER_H__
#define __TST_TEST_SPECIFICATION_PARSER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSpecificationParser
//
// Description: Parses a string into a testSpecification using the following grammar
//
// name::= any ascii character that isn't /,[,],*
// path ::= step ('/' step)*
//
// all ::= '*'
// andexpresion ::= name ('and' name)*
// expression ::= '[' name | all | andexpression ']'
// 
// spec ::= path / expression 
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

class TSTtestSpecification;
class TSTtestSpecificationParserPrivate;

class TSTtestSpecificationParser
{
public:
   TSTtestSpecificationParser();
   virtual ~TSTtestSpecificationParser();

   void createSpecification(const COLstring& String, TSTtestSpecification& ReturnSpec);

private:
   TSTtestSpecificationParserPrivate* pMember;
   TSTtestSpecificationParser(const TSTtestSpecificationParser& Orig);
   TSTtestSpecificationParser& operator=(const TSTtestSpecificationParser& Orig);
};

#endif // end of defensive include


