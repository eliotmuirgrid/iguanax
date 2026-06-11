//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved 
//
// Module: COLevent
//
// Description:
//
// Implementation - _WIN32
//
// Author: Eliot Muir
// Date:   Wed 09/23/1998 
//---------------------------------------------------------------------------
#include "COLevent.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "COLwindows.h"
#include "COLerror.h"

class COLeventPrivate 
{
public:
   HANDLE EventHandle;
};

void COLevent::createEvent( bool Sticky ) {
   pMember = new COLeventPrivate();
   pMember->EventHandle = ::CreateEvent(NULL, Sticky ? TRUE : FALSE, FALSE, NULL);
   if (pMember->EventHandle == NULL)
   {
      // we should delete pMember here since we are going to throw an exception
      // which means the destructor will not be called.
      delete pMember;
      pMember = NULL;
      COL_THROW_WIN32_ERROR("::CreateEvent failed");
   }
   {  //disable inheritance
      DWORD Flags;
      if (!GetHandleInformation(pMember->EventHandle, &Flags)) {
         ::CloseHandle(pMember->EventHandle); //nothrow
         delete pMember;
         COL_THROW_WIN32_ERROR("GetHandleInformation failed");
      }
      Flags &= (~HANDLE_FLAG_INHERIT);
      if (!SetHandleInformation(pMember->EventHandle, HANDLE_FLAG_INHERIT, Flags)) {
         ::CloseHandle(pMember->EventHandle); //nothrow
         delete pMember;
         COL_THROW_WIN32_ERROR("SetHandleInformation failed");
      }
   }
}

COLevent::~COLevent() {
   try {
      if (pMember != NULL) {
         CloseHandle(pMember->EventHandle);
         delete pMember;
      }   
   } catch(const COLerror& Error) {
      COL_TRC(Error);
   }
}

void COLevent::wait() {
   DWORD ReturnCode = WaitForSingleObject(pMember->EventHandle, INFINITE);
   switch(ReturnCode) {
      case WAIT_OBJECT_0: return;
      case WAIT_TIMEOUT: return;
      default: COL_THROW_WIN32_ERROR("wait");
   }
}

void COLevent::signal() { 
   BOOL Result = ::SetEvent(pMember->EventHandle);
   if (!Result){
      COLstring LastError = COLlastErrorAsString();
      COL_ABORT("SetEvent failed " << LastError);
   }
}

void COLevent::reset() { 
   BOOL Result = ::ResetEvent(pMember->EventHandle); 
   if (!Result){
      COLstring LastError = COLlastErrorAsString();
      COL_ABORT("ResetEvent failed " << LastError);
   }
}