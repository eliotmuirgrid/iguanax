#ifndef __SCK_POLLER_H__
#define __SCK_POLLER_H__
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

#include "SCKloop.h"
#include "SCKutils.h"
#include "SCKerror.h"

#include <COL/COLlist.h>

#ifdef SCK_SELECT
class SCKpoller{
public:
   SCKpoller(){};
   virtual ~SCKpoller() {};

   // TODO one day might want to revisit this interface having addRead and addWrite as two different calls mean two epoll kernel calls.  It might
   // or might not make a difference to performance.
   virtual void addWrite(int Socket)=0;
   virtual void addRead (int Socket)=0;
   virtual void removeWrite(int Socket)=0;
   virtual void removeAll(int Socket)=0;
   virtual int maxFd() const=0;
   virtual void poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks)=0;
};

class SCKselectPoller : public SCKpoller {
public:
   SCKselectPoller();
   ~SCKselectPoller() {}
   void addWrite(int Socket);
   void addRead (int Socket);
   void removeWrite(int Socket);
   void removeAll(int Socket);
   int maxFd() const;
   void poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks);

   fd_set m_ReadSet;
   fd_set m_WriteSet;
   int m_MaxFd;
};
#endif

#ifdef SCK_EPOLL
class SCKepollPoller : public SCKpoller{
public:
   SCKepollPoller();
   ~SCKepollPoller();

   void addWrite(int Socket);
   void addRead (int Socket);
   void removeWrite(int Socket);
   void removeAll(int Socket);
   int maxFd() const;
   void poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks);

   int m_EpollFd;
   static const int m_MaxEpollEvents = 100; //Maximum events epoll returns.
};
#endif

#ifdef SCK_KQUEUE
class SCKkqueuePoller : public SCKpoller{
public:
   SCKkqueuePoller();
   ~SCKkqueuePoller();

   void addWrite(int Socket);
   void addRead (int Socket);
   void removeWrite(int Socket);
   void removeAll(int Socket);
   int maxFd() const;
   void poll(int Milliseconds, SCKcallbacks& WriteCallbacks, SCKcallbacks& ReadCallbacks, SCKcallbacks& ErrorCallbacks);
   
   static const int m_MaxKqueueEvents = 100;
   int m_KqueueFd;

};
#endif

#endif // end of defensive include
