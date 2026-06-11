//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKpoller
//
// Author: Kevin Cai
//
// Description:
//
// Implementation 
//---------------------------------------------------------------------------

#include "SCKpoller.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef SCK_SELECT

SCKselectPoller::SCKselectPoller() : m_MaxFd(0){
   COL_METHOD(SCKselectPoller::SCKselectPoller);
   FD_ZERO(&m_WriteSet);
   FD_ZERO(&m_ReadSet);
}

void SCKselectPoller::addWrite(int Socket){
   COL_METHOD(SCKselectPoller::addWrite);
   FD_SET(Socket, &m_WriteSet);
   m_MaxFd = COL_MAX(Socket, m_MaxFd);
   COL_VAR(m_MaxFd);
}

void SCKselectPoller::addRead(int Socket){
   COL_METHOD(SCKselectPoller::addRead);
   FD_SET(Socket, &m_ReadSet);
   m_MaxFd = COL_MAX(Socket, m_MaxFd);
   COL_VAR(m_MaxFd);
}

void SCKselectPoller::removeWrite(int Socket){
   COL_METHOD(SCKselectPoller::removeWrite);
   COL_VAR(Socket);
   FD_CLR(Socket, &m_WriteSet);
   // TODO - do I need to lower the m_MaxFd variable.  Only if we really care about optimizing the ::select implementation
}

void SCKselectPoller::removeAll(int Socket){
   COL_METHOD(SCKselectPoller::removeAll);
   COL_VAR(Socket);
   FD_CLR(Socket, &m_WriteSet);
   FD_CLR(Socket, &m_ReadSet);
}

int SCKselectPoller::maxFd() const{
   COL_METHOD(SCKselectPoller::maxFd);
   return FD_SETSIZE - 10;
}

void SCKselectPoller::poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks){
   COL_METHOD(SCKselectPoller::poll);
   COLASSERT(m_MaxFd + 10 < FD_SETSIZE); //Last line of defense against reaching fd set limit
   // We really need to make sure that shipping products don't reach that limit.  Hopefully we won't use the select poller in production.
   int Seconds = Milliseconds / 1000;
   int Microseconds = (Milliseconds % 1000) * 1000;

   timeval Timeout = { Seconds, Microseconds };  // Seconds, microseconds.
   COL_TRC("Starting select()");
   fd_set SelectReadSet = m_ReadSet;
   fd_set SelectWriteSet = m_WriteSet;
#ifdef _WIN32  // notice that m_MaxFd isn't needed by windows - really should not bother with it on windows.
   fd_set SelectErrorSet = m_ReadSet;  // Can I get away with just copying the m_ReadSet?
   int SelectResult = ::select(0, &SelectReadSet, &SelectWriteSet, &SelectErrorSet, &Timeout);
#else
   // only seem to need the error set for _WIN32.
   int SelectResult = ::select(m_MaxFd+1, &SelectReadSet, &SelectWriteSet, NULL, &Timeout);
   //SCKshowFdSet(m_MaxFd+1, SelectReadSet);  // UNCOMMENT for diagnostics.
   //SCKshowFdSet(m_MaxFd+1, SelectWriteSet);
#endif
   
   if (SelectResult == 0){
      COL_VAR(SelectResult);
      COL_TRC("There is no action on any socket. So let's return!");
      return;
   }
   if (SelectResult == -1){
      int ErrorCode = SCKlastSocketError();
      COL_VAR(SelectResult);
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
      return;
   }
   COL_VAR(SelectResult);
#ifdef _WIN32 // The error list only matters for Windoze.  It seems to be the way we get the error notification back for a failed connect for non blocking sockets using ::select
   COLlist<COLclosure0*> ErrorList;
   for (SCKcallbacks::iterator i = ErrorCallbacks.begin(); i != ErrorCallbacks.end(); i++){
      if (FD_ISSET(i->first, &SelectErrorSet)) { ErrorList.add(i->second.get()); } 
   }
   COL_VAR(ErrorList.size());
   // Important to have read notifications first so we can get the connection refused errors.
   for (COLlist<COLclosure0*>::iterator i = ErrorList.begin(); i != ErrorList.end(); i++){
      COL_TRC("Calling error notification back on socket.");
      (*i)->run();
   }
#endif
   COLlist<COLclosure0*> ReadList;
   // We can't safely iterate through the dictionary while we dispatch events because it might be changed, so we make two to-do list.
   for (SCKcallbacks::iterator i = ReadCallbacks.begin(); i != ReadCallbacks.end(); i++){
      COL_VAR(i->first);
      if (FD_ISSET(i->first, &SelectReadSet))  {  ReadList.add(i->second.get()); }
   }
   COL_VAR(ReadList.size());
   // Important to have read notifications first so we can get the connection refused errors.
   for (COLlist<COLclosure0*>::iterator i = ReadList.begin(); i != ReadList.end(); i++){
      (*i)->run();
   }
   COLlist<COLclosure0*> WriteList;
   // Important to do this after running the read callbacks since a read callback could have removed a socket from the write callback list.
   for (SCKcallbacks::iterator i = WriteCallbacks.begin(); i != WriteCallbacks.end(); i++){
      COL_VAR(i->first);
      if (FD_ISSET(i->first, &SelectWriteSet)) { WriteList.add(i->second.get()); }
   }
   COL_VAR(WriteList.size());
   for (COLlist<COLclosure0*>::iterator i = WriteList.begin(); i != WriteList.end(); i++){
      (*i)->run();
   }
}
#endif
