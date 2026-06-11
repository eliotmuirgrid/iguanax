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
#ifdef SCK_EPOLL
#include <sys/epoll.h> 
#include <sys/resource.h>  // needed for finding the maximum file descriptors

SCKepollPoller::SCKepollPoller(){
   COL_METHOD(SCKepollPoller::SCKepollPoller);
   // TODO - we might need to revisit if we find spotty support for epoll in the current Linux eco-system
   m_EpollFd = epoll_create(1);
   COL_VAR(m_EpollFd);
   if (m_EpollFd == -1) { //Is this error handling enough?
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << " : " << ErrorCode);
   }
}

SCKepollPoller::~SCKepollPoller(){
   COL_METHOD(SCKepollPoller::~SCKepollPoller);
   if (close(m_EpollFd) != 0){
      COL_ERR("Warning " << errno << " after closing epoll file descriptor");
   }
   COL_VAR(m_EpollFd);
}

void SCKepollPoller::addWrite(int Socket){ //Assumes that socket is already hooked up.
   COL_METHOD(SCKepollPoller::addWrite);
   int EPollModResult;
   struct epoll_event ev;
   memset(&ev, 0, sizeof(ev));
   ev.data.fd = Socket;
   ev.events = EPOLLIN | EPOLLOUT;
   EPollModResult = epoll_ctl(m_EpollFd, EPOLL_CTL_MOD, Socket, &ev);
   COL_VAR(EPollModResult);
   if (EPollModResult != 0){
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
   }
}

void SCKepollPoller::addRead (int Socket){ //Assumes that socket is not yet hooked up.
   COL_METHOD(SCKepollPoller::addRead);
   int EPollAddResult;
   struct epoll_event ev;
   memset(&ev, 0, sizeof(ev));
   ev.data.fd = Socket;
   ev.events = EPOLLIN;
   EPollAddResult = epoll_ctl(m_EpollFd, EPOLL_CTL_ADD, Socket, &ev);
   COL_VAR(EPollAddResult);
   if (EPollAddResult != 0){
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
   }
}

void SCKepollPoller::removeWrite(int Socket){
   COL_METHOD(SCKepollPoller::removeWrite);
   int EPollModResult;
   struct epoll_event ev;
   memset(&ev, 0, sizeof(ev));
   ev.data.fd = Socket;
   ev.events = EPOLLIN;
   EPollModResult = epoll_ctl(m_EpollFd, EPOLL_CTL_MOD, Socket, &ev);
   COL_VAR(EPollModResult);
   if (EPollModResult != 0){
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);
   }
}

void SCKepollPoller::removeAll(int Socket){
   COL_METHOD(SCKepollPoller::removeAll);
   //Epoll automatically deregisters FDs as they close, unless the fd is still open when fork() (look at man epoll).
}

int SCKepollPoller::maxFd() const{ // Could be stored in a variable for maximum efficiency
   COL_METHOD(SCKepollPoller::maxFd);
   int MaxFd;
   rlimit FDlimit;
   int RtnCode = getrlimit(RLIMIT_NOFILE, &FDlimit);
   if (RtnCode == 0){
      MaxFd = FDlimit.rlim_cur;
   } else {
      COL_ERR("Retriving file descriptor limit. " << SCKsocketError(SCKlastSocketError()) << " : " << SCKlastSocketError());
   }
   return MaxFd - 10;
}

void SCKepollPoller::poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks){
   COL_METHOD(SCKepollPoller::poll);
   struct epoll_event events[m_MaxEpollEvents];
   int EPollResult;
   EPollResult = epoll_wait(m_EpollFd, events, m_MaxEpollEvents, Milliseconds);
   COL_VAR(EPollResult);

   if (EPollResult == 0){
      COL_TRC("No activity");
      return;
   }
   if (EPollResult == -1){
      int ErrorCode = SCKlastSocketError();
      COL_ERR("Error: " << SCKsocketError(ErrorCode) << " : " << ErrorCode);
      return;
   }
   for (int i = 0; i < EPollResult; i++){
      //Errors could occur when a socket modifies another socket that has higher FD.
      int SocketFd = events[i].data.fd;
      COL_VAR2(SocketFd, events[i].events);

      //If implementing Edge-triggered mode, might require EPOLLRDHUP.

      //Going through all possible conditions
      if (events[i].events & EPOLLERR){
         // Currently only displays error, which is handled by the read and write events. 
         // However, might be useful for when error occurs but no read or write flag are
         // triggered. (Does that ever happen?) 
         // Might need to use getsockopt to get error code off the socket.
         int ErrorCode = SCKlastSocketError();
         COL_ERR("Error: " << SCKsocketError(ErrorCode) << ":" << ErrorCode);

         if (!(events[i].events & EPOLLIN)){
            //Trigger reading from socket flag to allow for socket cleanup.
            events[i].events |= EPOLLIN;
         }
      }

      if ((events[i].events & EPOLLHUP) && !(events[i].events & EPOLLIN)){
         // Other side has hung up but read flag not set (happens with pipes),
         // trigger the read flag and let read handle it.
         // Might be a problem reading when the other side of pipe closed.
         COL_TRC("Other side hung but no read flag set. Setting read flag now."); 
         events[i].events |= EPOLLIN;
      }

      // Does not build a copy of the runnables before executing them. If a read event closes
      // another socket then that socket's read event will not run since it has been
      // deregistered from the ReadCallbacks.
      if (events[i].events & EPOLLIN){
         COL_TRC("Running read events");
         if (ReadCallbacks.count(SocketFd)){
            COL_TRC("Read event still exists");
            (*(ReadCallbacks[SocketFd])).run();
         } else {
            COL_TRC("No read events in ReadCallbacks anymore. Socket has been closed.");
         }
      }
      // Similar problem as the read events.
      if (events[i].events & EPOLLOUT) {
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
#endif // SCK_EPOLL
