//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLthreadLocal
//
// Description:
//
// Implementation of thread local storage class.
//
// Author: Andrew Vajoczki
// Date:   Wednesday, March 18th, 2009 @ 06:49:30 PM
//---------------------------------------------------------------------------
#include "COLthreadLocal.h"
#include <memory.h>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#  include <stdlib.h>
#endif

COLthreadLocalBase::COLthreadLocalBase(void (*destroyFunction)(void*), int DeleteKeyOnExit)
: m_DeleteKeyOnExit(DeleteKeyOnExit)
{
#ifndef _WIN32
   m_ThreadKey = malloc(sizeof(pthread_key_t));
   memset(m_ThreadKey, 0, sizeof(pthread_key_t));
   pthread_key_create((pthread_key_t*)m_ThreadKey, destroyFunction);
#else
   m_ThreadKey = (void*)TlsAlloc();
#endif
}

COLthreadLocalBase::~COLthreadLocalBase()
{
   if (m_DeleteKeyOnExit)
   {
#ifndef _WIN32
      pthread_key_delete(*(pthread_key_t*)m_ThreadKey); // XXX
      free(m_ThreadKey);
#else
      TlsFree((DWORD_PTR)m_ThreadKey);
#endif
   }
}

void* COLthreadLocalBase::getSpecific(){
#ifndef _WIN32
   return pthread_getspecific(*(pthread_key_t*)m_ThreadKey);
#else
   return TlsGetValue((DWORD_PTR)m_ThreadKey);
#endif
}

const void* COLthreadLocalBase::getSpecific() const{
#ifndef _WIN32
   return pthread_getspecific(*(pthread_key_t*)m_ThreadKey);
#else
   return TlsGetValue((DWORD_PTR)m_ThreadKey);
#endif
}

void COLthreadLocalBase::setSpecific(void* value)
{
#ifndef _WIN32
   pthread_setspecific(*(pthread_key_t*)m_ThreadKey, value);
#else
   TlsSetValue((DWORD_PTR)m_ThreadKey, value);
#endif
}

COLthreadLocalLong::COLthreadLocalLong(int DeleteKeyOnExit)
: m_tls(0, DeleteKeyOnExit)
{
}

