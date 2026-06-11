#ifndef __TST_HELPERS_H__ 
#define __TST_HELPERS_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSThelpers
//
// Description:
//
// Helper macros that make using the framework a bit easier.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLostream.h>
#include <TST/TSTexception.h>

template <class TValue>
class COLarray;

// since COLerror does not implement ==
bool TSTcompareException(const COLerror& Lhs, const COLerror& Rhs);

//adds a test collection to the existing collection/fixture.
#define TST_ADD_COLLECTION(_TESTNAME) \
   { \
      TSTtestCollection* __pSubCollection = new TSTtestCollection(_TESTNAME); \
      __pCollection->addTest(__pSubCollection); \
      TSTtestCollection* __pCollection = __pSubCollection;

//adds a test fixture to the existing test fixture, and inherits the fixtures
//instance instead of constructing a new one.
#define TST_ADD_FIXTURE_INHERIT(_TESTNAME) \
   { \
      TSTtestFixture<TCurrentClass>* __pSubCollection = new TSTtestFixture<TCurrentClass>(_TESTNAME,__pCollection); \
      __pCollection->addTest(__pSubCollection); \
      TSTtestFixture<TCurrentClass>* __pCollection = __pSubCollection; \

#define TST_END_ADD_COLLECTION \
   }

#define TST_START_FIXTURE(_CLASS) \
   TSTtestFixture<_CLASS>* __pCollection = new TSTtestFixture<_CLASS>(#_CLASS, &_CLASS::create); \
   typedef _CLASS TCurrentClass;

#define TST_START_FIXTURE_WITH_INSTANCE(_CLASS, pGivenInstance) \
   TSTtestFixture<_CLASS>* __pCollection = new TSTtestFixture<_CLASS>(#_CLASS, pGivenInstance); \
   typedef _CLASS TCurrentClass;

#define TST_START_NAMED_FIXTURE_WITH_INSTANCE(Name, _CLASS, pGivenInstance) \
   TSTtestFixture<_CLASS>* __pCollection = new TSTtestFixture<_CLASS>(Name, pGivenInstance); \
   typedef _CLASS TCurrentClass;

#define TST_ADD_TEST_METHOD(_METHOD) \
   __pCollection->addTest(new TSTtestMethod<TCurrentClass>(#_METHOD,&TCurrentClass::_METHOD));

#define TST_ADD_SET_UP_METHOD(_METHOD) \
   __pCollection->addSetUp(new TSTtestSetUpMethod<TCurrentClass>(#_METHOD,&TCurrentClass::_METHOD));

#define TST_ADD_TEAR_DOWN_METHOD(_METHOD) \
   __pCollection->addTearDown(new TSTtestTearDownMethod<TCurrentClass>(#_METHOD,&TCurrentClass::_METHOD));

#define TST_END_FIXTURE \
   return __pCollection;

#define TST_START_COLLECTION(_NAME) \
   TSTtestCollection* __pCollection = new TSTtestCollection(_NAME);

#define TST_ADD_TEST_FUNCTION(_FUNCTION) \
   __pCollection->addTest(new TSTtestFunction(#_FUNCTION,_FUNCTION));

#define TST_ADD_TEST_COLLECTION(_COLLECTION) \
   __pCollection->addTest(_COLLECTION);

// ^ really it's just adding a test, whatever it is..
#define TST_ADD_TEST(_TEST) \
   __pCollection->addTest(_TEST);

#define TST_ADD_TEST_FUNCTION_WITH_FLAGS(_FUNCTION) \
   __pCollection->addTest(new TSTtestFunctionWithFlags(#_FUNCTION,_FUNCTION));

#define TST_ADD_SET_UP_FUNCTION(_FUNCTION) \
   __pCollection->addSetUp(new TSTtestSetUpFunction(#_FUNCTION,_FUNCTION));

#define TST_ADD_TEAR_DOWN_FUNCTION(_FUNCTION) \
   __pCollection->addTearDown(new TSTtestTearDownFunction(#_FUNCTION,_FUNCTION));

#define TST_END_COLLECTION \
   return __pCollection;

#define TST_REGISTER_TEST(_TESTCLASS_INSTANCE) \
   TSTtestCollection::globalInstance().addTest(_TESTCLASS_INSTANCE);

#define TST_ASSERT(_EXPRESSION) \
   if (!(_EXPRESSION))  \
   { \
      COLstring __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << "Assert:      [" << #_EXPRESSION << "]" << newline; \
      TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure); \
      throw __Error; \
   }
 
#define TST_ASSERT_MESSAGE(_EXPRESSION,_MESSAGE_ON_FAIL) \
   if (!(_EXPRESSION))  \
   { \
      COLstring  __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << _MESSAGE_ON_FAIL << newline; \
      __Stream << "Assert:      [" << #_EXPRESSION << "]" << newline; \
      TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure); \
      throw __Error; \
   }

#define TST_ASSERT_EQUALS_MESSAGE(_EXPECTED, _ACTUAL, _MESSAGE_ON_FAIL) \
   if (_EXPECTED!=(_ACTUAL))  \
   { \
      COLstring  __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << _MESSAGE_ON_FAIL << newline; \
      __Stream << "Assert:      [" << #_EXPECTED  << " == " << #_ACTUAL << "]" << newline; \
      __Stream << "Expected:    [" << _EXPECTED << "]" << newline; \
      __Stream << "Actual:      [" << (_ACTUAL) << "]";  \
      TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure); \
      throw __Error; \
   } 

#define TST_ASSERT_EQUALS(_EXPECTED, _ACTUAL) \
   if (_EXPECTED!=(_ACTUAL))  \
   { \
      COLstring  __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << "Expected should be equal to actual." << newline; \
      __Stream << "Assert:      [" << #_EXPECTED  << " == " << #_ACTUAL << "]" << newline; \
      __Stream << "Expected:    [" << _EXPECTED << "]" << newline; \
      __Stream << "Actual:      [" << (_ACTUAL) << "]";  \
      TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure); \
      throw __Error; \
   } 

// This helper will try and ignore newline differences one often sees between platforms.
void TSTequalStrings(const COLstring& Expected, const COLstring& Actual);

#define TST_ASSERT_EXCEPTION_MESSAGE(_EVALUATE, _EXPECTED_EXCEPTION, _MESSAGE )\
   {\
      bool __ExceptionOccurred = false;\
      COLerror __ThrownError("",0);\
      const COLerror __ExpectedError = _EXPECTED_EXCEPTION;\
      try\
      {\
         _EVALUATE;\
      }\
      catch( const COLerror& __Error)\
      {\
         __ThrownError = __Error;\
         __ExceptionOccurred = true;\
      }\
      if (!__ExceptionOccurred)\
      {\
         COLstring __ErrorString;\
         COLostream __Stream(__ErrorString);\
         __Stream << "'" << #_EVALUATE << "': " << _MESSAGE;\
         TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure);\
         throw __Error; \
      }\
      else if (!TSTcompareException(__ThrownError,__ExpectedError))\
      {\
         COLstring __ErrorString;\
         COLostream __Stream(__ErrorString);\
         __Stream << "'" << #_EVALUATE << "': " << _MESSAGE << newline\
                  << "Expected: " << newline << __ExpectedError << newline\
                  << "Received: " << newline << __ThrownError << newline;\
         TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure);\
         throw __Error; \
      }\
   }

#define TST_ASSERT_EXCEPTION(_EVALUATE, _EXPECTED_EXCEPTION) \
      TST_ASSERT_EXCEPTION_MESSAGE(_EVALUATE,_EXPECTED_EXCEPTION, "did not throw as expected.")

#define TST_ASSERT_NO_EXCEPTION_MESSAGE(_EVALUATE, _MESSAGE_ON_EXCEPTION) \
   try \
   { \
      _EVALUATE; \
   } \
   catch (const COLerror& __UnexpectedException) \
   { \
      COLstring __ErrorString; \
      COLostream __Stream(__ErrorString); \
      __Stream << _MESSAGE_ON_EXCEPTION << newline; \
      __Stream << "Assert:      [" << #_EVALUATE << "]" << newline; \
      __Stream << "Expected:    [No Exception]" << newline; \
      __Stream << "Actual:      [Exception: " << __UnexpectedException.description() << "]"; \
      TSTexception __Error(__ErrorString,__LINE__,__FILE__,TSTexception::eAssertionFailure); \
      throw __Error; \
   }

#define TST_ASSERT_NO_EXCEPTION(_EVALUATE) \
   TST_ASSERT_NO_EXCEPTION_MESSAGE(_EVALUATE, "Exception unexpected.")

//This is all to avoid FIL dependencies so most unittests won't need it
bool TSTfileExists(const COLstring& FileName);
void TSTfileReadLines(const COLstring& FileName, COLarray<COLstring>& Lines);
void TSTfileWrite(const COLstring& FileName, const COLstring& Data);

#endif  // end of defensive include
