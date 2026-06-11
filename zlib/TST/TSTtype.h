#ifndef __TST_TYPE_H__
#define __TST_TYPE_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtype
//
// Description: The type instances for the TST framework. This allows for dynamic 
// typechecking without the overhead of a larger framework, like the TRE library.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

// TODO - question could either break this into smaller files or hide it so that
// users of the library do no see this file.

#include<COL/COLerror.h>
#ifdef _WIN32
   #include <string.h>
#endif
#define TST_TYPE_INSTANCE(_CLASS) \
   static TSTtype& instance() \
   { \
      static _CLASS Instance; \
      return Instance; \
   }

#define TST_TYPE_PARENT_TYPE(_TYPE) \
   virtual TSTtype* parentType() \
   { \
      return &_TYPE::instance(); \
   }

class TSTtype
{
public:
   TSTtype(){};
   virtual ~TSTtype(){};
   
   virtual const char* name() = 0;
   
   //if no parent type, returns null
   virtual TSTtype* parentType()
   {
      return NULL;
   }

   bool isDerivedFrom(TSTtype& ParentType)
   {
    //this is changed temporarily since DB2example on AIX seems to return 2 pointers to parentType and we dont know why yet 
      TSTtype* pType = this;
      while (pType &&
             /*pType != &ParentType*/ strcmp(pType->name(),ParentType.name())!=0)
      {
         pType = pType->parentType();
      }
      return pType != NULL;
   }

};

class TSTtypeTestable : public TSTtype
{
public:
   TSTtypeTestable(){};
   virtual ~TSTtypeTestable(){};
   
   virtual const char* name(){ return "TSTtypeTestable"; }

   TST_TYPE_INSTANCE(TSTtypeTestable)
};

class TSTtypeTestCollection : public TSTtypeTestable
{
public:
   TSTtypeTestCollection(){};
   virtual ~TSTtypeTestCollection(){};
   
   virtual const char* name(){ return "TSTtypeTestCollection"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestCollection)
};

class TSTtypeTestFunction : public TSTtypeTestable
{
public:
   TSTtypeTestFunction(){};
   virtual ~TSTtypeTestFunction(){};
   
   virtual const char* name(){ return "TSTtypeTestFunction"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestFunction)
};

class TSTtypeTestFunctionWithFlags : public TSTtypeTestable
{
public:
   TSTtypeTestFunctionWithFlags(){};
   virtual ~TSTtypeTestFunctionWithFlags(){};
   
   virtual const char* name(){ return "TSTtypeTestFunctionWithFlags"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestFunction)
};

class TSTtypeTestSetUpFunction : public TSTtypeTestable
{
public:
   TSTtypeTestSetUpFunction(){};
   virtual ~TSTtypeTestSetUpFunction(){};
   
   virtual const char* name(){ return "TSTtypeTestSetUpFunction"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestSetUpFunction)
};

class TSTtypeTestTearDownFunction : public TSTtypeTestable
{
public:
   TSTtypeTestTearDownFunction(){};
   virtual ~TSTtypeTestTearDownFunction(){};
   
   virtual const char* name(){ return "TSTtypeTestTearDownFunction"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestTearDownFunction)
};

class TSTtypeTestFixture : public TSTtypeTestCollection
{
public:
   TSTtypeTestFixture(){};
   virtual ~TSTtypeTestFixture(){};
   
   virtual const char* name(){ return "TSTtypeTestFixture"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestCollection)

   TST_TYPE_INSTANCE(TSTtypeTestFixture)
};

template <class _T>
class TSTtypeTestMethod : public TSTtype
{
public:
   TSTtypeTestMethod(){};
   virtual ~TSTtypeTestMethod(){};
   
   virtual const char* name(){ return "TSTtypeTestMethod<_T>"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestMethod<_T>)
};

template <class _T>
class TSTtypeTestSetUpMethod : public TSTtype
{
public:
   TSTtypeTestSetUpMethod(){};
   virtual ~TSTtypeTestSetUpMethod(){};
   
   virtual const char* name(){ return "TSTtypeTestSetUpMethod<_T>"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestSetUpMethod<_T>)
};

template <class _T>
class TSTtypeTestTearDownMethod : public TSTtype
{
public:
   TSTtypeTestTearDownMethod(){};
   virtual ~TSTtypeTestTearDownMethod(){};
   
   virtual const char* name(){ return "TSTtypeTestTearDownMethod<_T>"; }

   TST_TYPE_PARENT_TYPE(TSTtypeTestable)

   TST_TYPE_INSTANCE(TSTtypeTestTearDownMethod<_T>)
};

#endif   // end of defensive include



