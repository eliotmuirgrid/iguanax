//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SCKloop
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

//TODO: Consider possible errors of all epoll calls - Kevin

#include "SCKloop.h"
#include "SCKutils.h"
#include "SCKpoller.h"

#include <COL/COLlist.h>
#include <COL/COLlog.h>
#include <COL/COLhashmap.h>
COL_LOG_MODULE;

void SCKshowFdSet(int MaxFd, const fd_set& Set){
   COL_FUNCTION(SCKshowFdSet);
   for (int fd=0; fd < MaxFd; fd++){
      COL_VAR2(fd, FD_ISSET(fd, &Set));
   }
}

#ifdef SCK_SELECT
SCKloop::SCKimpl SCKdefaultImpl(SCKloop::SCKimpl Impl){ //If Impl == DEFAULT, replaces DEFAULT with the implementation.
   COL_FUNCTION(SCKdefaultImpl);
   if (Impl == SCKloop::DEFAULT){
      if      (SCKsupportEpoll())  { Impl = SCKloop::EPOLL; }
      else if (SCKsupportKqueue()) { Impl = SCKloop::KQUEUE;}
      else                         { Impl = SCKloop::SELECT;}
   }
   COL_VAR(Impl);
   return Impl;
}

SCKpoller * SCKpollerFactory(SCKloop::SCKimpl Impl){
   COL_FUNCTION(SCKpollerFactory);
   SCKpoller* pPoller;
   switch (Impl){
#ifdef SCK_EPOLL
      case SCKloop::EPOLL:
         COL_TRC("Using epoll implementation");
         pPoller = new SCKepollPoller;
         break;
#elif defined SCK_KQUEUE
      case SCKloop::KQUEUE:
         COL_TRC("Using kqueue implementation");
         pPoller = new SCKkqueuePoller;
         break;
#endif
      default:
         COL_TRC("Using select implementation");
         pPoller = new SCKselectPoller;
         break;
   }
   return pPoller;
}

SCKloop::SCKloop(COLthreadPool* pThreadPool)
 : m_pThreadPool(pThreadPool), m_EndLoop(false), m_NameResolver(pThreadPool, this), m_StartThread(0) {
   COL_METHOD(SCKloop::SCKloop);
   m_pPoller = SCKpollerFactory(SCKdefaultImpl(DEFAULT));
   ::pipe(m_Pipe);  // assume it won't fail...
   COL_VAR2(m_Pipe[0], m_Pipe[1]);
   m_ReadCallback.add(m_Pipe[0], COLnewClosure0(this, &SCKloop::onWakeupPipe));
   m_pPoller->addRead(m_Pipe[0]);
   m_FdCount = 0;
}

SCKloop::~SCKloop(){
   COL_METHOD(SCKloop::~SCKloop);
   m_pPoller->removeAll(m_Pipe[0]);
   m_ReadCallback.removeKey(m_Pipe[0]);
   COL_VAR2(m_WriteCallback.size(),m_ReadCallback.size());
   if (m_ReadCallback.size() > 0){
      COL_WRN("Sockets not deregistered from ReadCallbacks: " << m_ReadCallback.size());
      SCKcallbacks::iterator it = m_ReadCallback.begin(), E = m_ReadCallback.end();
      for (; it != E; ++it) {
         COL_WRN(it->first);
      }
   }
   if (m_WriteCallback.size() > 0){
      COL_WRN("Sockets not deregistered from WriteCallbacks: " << m_WriteCallback.size());
      SCKcallbacks::iterator it = m_WriteCallback.begin(), E = m_WriteCallback.end();
      for (; it != E; ++it) {
         COL_WRN(it->first);
      }
   }
   close(m_Pipe[0]);
   close(m_Pipe[1]);
   delete m_pPoller;
}

int SCKloop::maxFd() const{
   int MaxFd = m_pPoller->maxFd();
   COL_VAR(MaxFd);
   return MaxFd;
}

int SCKloop::countOfFd() const{
   COL_METHOD(SCKloop::countOfFd);
   COL_VAR(m_FdCount);
   return m_FdCount;
}

// TODO change over to using COLclosure0
void SCKloop::registerWriteCallback(int Fd, COLclosure0* pWriteCallback){
   COL_METHOD(SCKloop::registerWriteCallback);
   if (pWriteCallback) { m_WriteCallback.add(Fd, pWriteCallback); }
}

void SCKloop::registerReadCallback(int Fd, COLclosure0* pReadCallback){
   COL_METHOD(SCKloop::registerReadCallback);
   if (pReadCallback) { m_ReadCallback.add(Fd, pReadCallback); }
}

void SCKloop::registerCallbacks(int Socket, COLclosure0* pWriteCallback, COLclosure0* pReadCallback, COLclosure0* pErrorCallback){
   COL_METHOD(SCKloop::registerCallbacks);
   if (pReadCallback)  { m_ReadCallback.add(Socket, pReadCallback);   }
   if (pWriteCallback) { m_WriteCallback.add(Socket, pWriteCallback); }
   if (pErrorCallback) { m_ErrorCallback.add(Socket, pErrorCallback); }
}

void SCKloop::addWriteCallback(int Socket){
   COL_METHOD(SCKloop::addWriteCallback);
   COL_VAR(Socket);
   m_pPoller->addWrite(Socket);
}

void SCKloop::addReadCallback(int Socket){
   COL_METHOD(SCKloop::addReadCallback);
   m_FdCount++;
   COL_VAR2(Socket, m_FdCount);
   m_pPoller->addRead(Socket);
}

void SCKloop::removeWriteCallback(int Socket){
   COL_METHOD(SCKloop::removeWriteCallback);
   COL_VAR(Socket);
   m_pPoller->removeWrite(Socket);
}

void SCKloop::removeAllCallback(int Socket){
   COL_METHOD(SCKloop::removeAllCallback);
   COL_VAR(Socket);
#ifdef _WIN32
   m_ErrorCallback.removeKey(Socket);
#endif
   m_WriteCallback.removeKey(Socket);
   if (m_ReadCallback.removeKey(Socket)){ // Only decrease count if socket was bound.
      m_FdCount--;
   }
   COL_VAR(m_FdCount);
   COL_VAR2(m_WriteCallback.size(),m_ReadCallback.size());
   m_pPoller->removeAll(Socket);
}

void SCKloop::start(){
   COL_METHOD(SCKloop::start);
   m_StartThread = COLthreadGetCurrentThreadID();
   m_EndLoop = false;
   while (!m_EndLoop){
      COL_VAR(m_Schedule.timeTillNextEvent(1000));
      m_pPoller->poll(m_Schedule.timeTillNextEvent(1000), m_WriteCallback, m_ReadCallback, m_ErrorCallback);
      m_Schedule.fire();
   }
}

// Post writes a trivial amount of data into this pipe object which is being watched
// in our epoll/kqueue waiting call.
void SCKloop::post(COLclosure0* pClosure){
   COL_METHOD(SCKloop::post);
   // TODO - make sure we only call this on a thread ID different from start() thread.
   m_ThreadQueue.put(pClosure);
   COL_TRC("Put in the event, now write to the pipe");
   char Buffer='A';
   ::write(m_Pipe[1], &Buffer, 1);  // wake up the event polling loop.
   // TODO - we could optimize this with less writes to the pipe.
}

// TODO more comments explaining how this works and why.
void SCKloop::onWakeupPipe(){
   COL_METHOD(SCKloop::onWakeupPipe);
   char Buffer[100];
   ::read(m_Pipe[0], &Buffer, sizeof(Buffer));
   while (m_ThreadQueue.size() > 0){
      COLauto<COLclosure0> pClosure = m_ThreadQueue.take();
      try{
         COL_TRC("About to call closure");
         pClosure->run();
         COL_TRC("Returned from closure.");
      } catch (const COLerror& Error){
         // Eliot - IX-1857 Important that we violently stop when this occurs.
         COLabortWithMessage("Exception thrown from ::post" + Error.description());
      }
   }
}
#endif // SCK_SELECT

COLuint64 SCKloop::m_NextSocketId = 0;

void SCKloop::shutdown(){
   COL_METHOD(SCKloop::shutdown);
   m_EndLoop = true;
}

bool SCKloop::isSet(int TimerId) const {
   COLASSERT(m_StartThread == 0 || m_StartThread == COLthreadGetCurrentThreadID());  // must be called from the main thread
   return m_Schedule.isSet(TimerId);
}
