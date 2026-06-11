#ifndef __TST_TEST_SPECIFICATION_H__
#define __TST_TEST_SPECIFICATION_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSpecification
//
// Description: Describes a path in the test tree to test. 
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>


class TSTtestableOptions;
class COLstring;
class TSTtestSpecificationPrivate;

class TSTtestSpecification
{
public:
   TSTtestSpecification();
   virtual ~TSTtestSpecification();

   //clears path and options
   void clear();

   //pushes back a test name
   void pushBackPath(const COLstring& TestableName);

   //the number of pushbacks called
   COLuint32 pathDepth() const;
   const COLstring& pathItem(COLuint32 Depth) const;
   
   //allows modification of how to treat the test that the path
   //resolves to
   TSTtestableOptions& testOptions();
   const TSTtestableOptions& testOptions() const;


   bool operator==(TSTtestSpecification&);
   bool operator!=(TSTtestSpecification& Rhs)
   {
      return !(this->operator==(Rhs));
   }


private:
   TSTtestSpecificationPrivate* pMember;
   TSTtestSpecification(const TSTtestSpecification& Orig);
   TSTtestSpecification& operator=(const TSTtestSpecification& Orig);
};

#endif   // end of defensive include


