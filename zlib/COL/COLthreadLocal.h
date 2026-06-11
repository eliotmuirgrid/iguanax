#ifndef __COL_THREAD_LOCAL_H__
#define __COL_THREAD_LOCAL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLthreadLocal
//
// Description:
//
//   Thread local storage class.
//
// Author: Andrew Vajoczki
// Date:   Wednesday, March 18th, 2009 @ 06:49:30 PM
//---------------------------------------------------------------------------

#include <new>
#include <memory.h>
#include <stdint.h>

class COLthreadLocalBase{
public:
   typedef void (*COLthreadLocalBase_destroy_fn)(void*);
   COLthreadLocalBase(COLthreadLocalBase_destroy_fn = 0, int DeleteKeyOnExit = 1);
   ~COLthreadLocalBase();
   const void* getSpecific() const;
   void* getSpecific();
   void setSpecific(void* value);
private:
   void* m_ThreadKey;
   int m_DeleteKeyOnExit;
};

// An optimization to use the thread specific pointer itself
// as the integer in order to avoid an extra memory allocation
// to store the value.
//
class COLthreadLocalLong{
public:
   typedef intptr_t Type;
   COLthreadLocalLong(int DeleteKeyOnExit = 1);
   Type get() const           { return (Type)m_tls.getSpecific(); }
   operator Type() const      { return (Type)m_tls.getSpecific(); }
   void set(Type value)       { m_tls.setSpecific((void*)value); }
   void operator=(Type value) { m_tls.setSpecific((void*)value); }
private:
   COLthreadLocalLong(const COLthreadLocalLong&);
   COLthreadLocalLong& operator=(const COLthreadLocalLong&);
private:
   mutable COLthreadLocalBase m_tls;
};

template <typename Type>
class COLthreadLocal : public COLthreadLocalBase{
public:
   COLthreadLocal() : COLthreadLocalBase(&destroy) { }
   ~COLthreadLocal() { }

   // Does this thread local object exist?
   // Do not create one if it does not.
   bool exists() const{
      const Type* value = (const Type*)getSpecific();
      return value != 0;
   }

   Type* getPointer()             { return getPointerPrivate(); }
   const Type* getPointer() const { return getPointerPrivate(); }

   Type& get()                    { return *getPointerPrivate(); }
   const Type& get() const        { return *getPointerPrivate(); }
   operator Type&()               { return *getPointerPrivate(); }

   Type& operator=(const Type& value) { return *set(value); }

   Type* set(const Type& value){
      Type* p = getPointerPrivate();
      *p = value;
      return p;
   }

private:
   COLthreadLocal(const COLthreadLocal&); // disallow
   COLthreadLocal& operator=(const COLthreadLocal&); // disallow

   Type* getPointerPrivate(){
      Type* value = (Type*)getSpecific();
      if (value == 0){
         // memory must be zeroed and strictly aligned.
         char* mem = (char*)new double[(sizeof(Type) + sizeof(double) - 1) / sizeof(double)];
         memset(mem, 0, sizeof(Type));
         value = new (mem) Type;
         setSpecific(value);
      }
      return value;
   }

   static void destroy(void* value){
      // Because the Type used the placement new operator we must
      // invoke the destructor directly and then free it manually.
      ((Type*)value)->~Type();
      delete [] (double*)value;
   }
};

#endif // end of defensive include
