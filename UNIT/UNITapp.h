#ifndef __UNIT_APP_H__
#define __UNIT_APP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITapp
//
// Author: Eliot Muir
//
// Date: Mon 15 Mar 2021 08:40:28 EDT
//
// Description:
//
// This is the public interface to the UNIT test library.
//---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLclosure.h>
#include <COL/COLauto.h>

typedef void (*UNITtest)();

class UNITapp{
public:
   UNITapp() {}
   ~UNITapp() {}

   void add       (const COLstring& Name, UNITtest     pTestFunc   );
   void addClosure(const COLstring& Name, COLclosure0* pTestClosure); // Takes ownership

   int run(int argc,const char** argv);
   int runSingleThreaded(int argc,const char** argv);

   COLmap<COLstring, COLauto<COLclosure0> >& map() { return m_TestCollection; }
private:
   COLmap<COLstring, COLauto<COLclosure0> > m_TestCollection;
};

#define UNIT_ASSERT_EQUALS(_EXPECTED, _ACTUAL) \
   if (_EXPECTED!=(_ACTUAL))  { \
      COLstring  __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << "Expected should be equal to actual." << newline; \
      __Stream << "Assert:      [" << #_EXPECTED  << " == " << #_ACTUAL << "]" << newline; \
      __Stream << "Expected:    [" << _EXPECTED << "]" << newline; \
      __Stream << "Actual:      [" << (_ACTUAL) << "]";  \
      COLerror __Error(__ErrorString,__LINE__,__FILE__,0); \
      throw __Error; \
   } 

#define UNIT_ASSERT_MESSAGE(_EXPRESSION,_MESSAGE_ON_FAIL) \
   if (!(_EXPRESSION))  {\
      COLstring  __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << _MESSAGE_ON_FAIL << newline; \
      __Stream << "Assert:      [" << #_EXPRESSION << "]" << newline; \
      COLerror __Error(__ErrorString,__LINE__,__FILE__,0); \
      throw __Error; \
   }

#define UNIT_ASSERT(_EXPRESSION) \
   if (!(_EXPRESSION))  {\
      COLstring __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << "Assert:      [" << #_EXPRESSION << "]" << newline; \
      COLerror __Error(__ErrorString,__LINE__,__FILE__,0); \
      throw __Error; \
   }

#endif // end of defensive include
