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
#ifdef SCK_KQUEUE

#include <sys/event.h>
#include <sys/resource.h>  // needed for finding the maximum file descriptors
#include <COL/COLlog.h>

COL_LOG_MODULE;

SCKkqueuePoller::SCKkqueuePoller(){
   COL_METHOD(SCKkqueuePoller::SCKkqueuePoller);
   m_KqueueFd = kqueue();
   COL_VAR(m_KqueueFd);
   if (m_KqueueFd == -1) { // TODO Is this error handling enough?
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << " : " << ErrorCode);
   }
}

SCKkqueuePoller::~SCKkqueuePoller(){
   COL_METHOD(SCKkqueuePoller::~SCKkqueuePoller);
   close(m_KqueueFd);
}

void SCKkqueuePoller::addRead(int Socket){
   COL_METHOD(SCKkqueuePoller::addRead);
   int KqueueResult;
   struct kevent ev;
   EV_SET(&ev, Socket, EVFILT_READ, EV_ADD, 0, 0, NULL); //EN_ENABLE is implied.
   KqueueResult = kevent(m_KqueueFd, &ev, 1, NULL, 0, NULL);
   COL_VAR(KqueueResult);
   if (KqueueResult == -1){
      int ErrorCode = SCKlastSocketError();
      COL_TRC("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
   }
}

void SCKkqueuePoller::addWrite(int Socket){
   COL_METHOD(SCKkqueuePoller::addWrite);
   COL_VAR(Socket);
   int KqueueResult;
   struct kevent ev;
   EV_SET(&ev, Socket, EVFILT_WRITE, EV_ADD, 0, 0, NULL); 
   KqueueResult = kevent(m_KqueueFd, &ev, 1, NULL, 0, NULL);
   COL_VAR(KqueueResult);
   if (KqueueResult == -1){
      int ErrorCode = SCKlastSocketError();
      COL_TRC("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
   }
}

void SCKkqueuePoller::removeWrite(int Socket){
   COL_METHOD(SCKkqueuePoller::removeWrite);
   COL_VAR(Socket);
   int KqueueResult;
   struct kevent ev;
   EV_SET(&ev, Socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
   KqueueResult = kevent(m_KqueueFd, &ev, 1, NULL, 0, NULL);
   COL_VAR(KqueueResult);
   if (KqueueResult == -1){
      int ErrorCode = SCKlastSocketError();
      COL_TRC("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
   }
}

void SCKkqueuePoller::removeAll(int Socket){
   COL_METHOD(SCKkqueuePoller::removeAll);
   COL_VAR(Socket);
   //Kqueue automatically deregisters FDs as they close.  So long as we don't have forked processes
}

// TODO - assign to a member to cache the value since it's an expensive operation.
int SCKkqueuePoller::maxFd() const{
   COL_METHOD(SCKepollPoller::maxFd);
   int MaxFd;
   rlimit FDlimit;
   int RtnCode = getrlimit(RLIMIT_NOFILE, &FDlimit);
   if (RtnCode == 0){
      MaxFd = FDlimit.rlim_cur;
   } else {
      COL_ERR("Retrieving file descriptor limit. " << SCKsocketError(SCKlastSocketError()) << " : " << SCKlastSocketError());
   }
   return MaxFd - 10;
}

void SCKkqueuePoller::poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks){
   COL_METHOD(SCKkqueuePoller::poll);
   int KqueueResult;
   int Seconds = Milliseconds / 1000;
   long int Nanoseconds = (Milliseconds % 1000) * 1000000;

   struct timespec Timeout = {Seconds, Nanoseconds};
   struct kevent ev[m_MaxKqueueEvents];

   KqueueResult = kevent(m_KqueueFd, NULL, 0, ev, m_MaxKqueueEvents, &Timeout);
   COL_VAR(KqueueResult);

   if (KqueueResult == 0){
      COL_TRC("No activity");
      return;
   } else if (KqueueResult == -1){
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << " : " << ErrorCode);
   }

   for (int i = 0; i < KqueueResult; i++){
      //Like Epoll, errors could occur when a socket modifies another socket that has a higher FD.
      int SocketFd = ev[i].ident;
      COL_VAR(SocketFd);
      COL_VAR2(ev[i].filter, ev[i].flags);

      // Maybe check for EV_EOF for termination of sockets? libuv doesn't.
      // the EV_ERROR flag only applies when adding a kevent - refer to man page
      if (ev[i].filter == EVFILT_READ){
         COL_TRC("Running read events");
         if (ReadCallbacks.count(SocketFd)){
            (*(ReadCallbacks[SocketFd])).run();
         } else {
            COL_TRC("No read events in ReadCallbacks, socket must have been closed.");
         }
      }

      if (ev[i].filter == EVFILT_WRITE){
         COL_TRC("Running write events");
         if (WriteCallbacks.count(SocketFd)){
            COL_TRC("Write event still exists");
            (*(WriteCallbacks[SocketFd])).run();
         } else {
            COL_TRC("No write events in WriteCallbacks anymore. Socket has been closed.");
         }
      }
   }
}
#endif  // SCK_KQUEUE
