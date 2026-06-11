#ifndef __SCK_LOOP_H__
#define __SCK_LOOP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKloop
//
// Author: Eliot Muir
//
// Description:
//
// Thin wrapper around the watching mechanism - i.e. a ::select call, IOCP, kqueue, epoll etc.
//---------------------------------------------------------------------------

#include "SCKsystem.h"  // important to get FD_SET macros consistent.
#include "SCKschedule.h"
#include "SCKnameResolver.h"

#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLauto.h>
#include <COL/COLthread.h>
#include <COL/COLthreadQueue.h>

class COLthreadPool;
class SCKconnection;

#ifdef SCK_SELECT


class SCKpoller;
typedef COLhashmap<int, COLauto<COLclosure0> > SCKcallbacks;
#endif // SCK_SELECT

class SCKloop{
public:
   // Since SCKloop has SCKnameResolver as a member in order to resolve hostnames to IP addresses
   // since there are only blocking APIs to do name lookup.
   SCKloop(COLthreadPool* pPool);
   ~SCKloop();

   COLthreadPool* threadPool() { return m_pThreadPool; }
   SCKnameResolver& nameResolver() { return m_NameResolver; }

   // This is the public API of the class
   // Run message loop - this starts the event loop.
   void start();

   // End message loop - typically used as the application shuts down.
   void shutdown();

   // Create a timer event.  SCKloop will invoke the COLclosure0 given for the interval given in milliseconds.
   // The timer event can optionally repeat.  The method returns the ID of the timer as an integer.  Applications
   // should gracefully call cancelTimer before they shut down to avoid issues with SCKlook calling the COLclosure0 on
   // objects which have gone out of scope.
   // IMPORTANT: Not thread safe. Must be called from the main thread. If you need to add a timer from a worker thread,
   // put it in a wrapper function and post the function to run on the main thread.
   int addTimer(COLauto<COLclosure0> pClosure, int TimeInMilliseconds=0, bool Repeating=false, const char* TimerName="");
   void cancelTimer(int TimerId);
   bool isSet(int TimerId) const;

   // This will wake up the loop and cause this closure to run on the thread that this SCKloop
   // is running on. This is a very important mechanism of how a thread can 'post' back it's results
   // to a thread which is running a message loop powered by SCKloop.
   void post(COLclosure0* pClosure);

   enum SCKimpl{
      DEFAULT = 0,
      KQUEUE = 1,
      EPOLL = 2,
      SELECT = 3
   };
   static COLuint64 m_NextSocketId;
   COLhashmap<COLuint64, SCKconnection*> m_ConnectionMap;
private:

   SCKschedule m_Schedule;
   COLthreadID m_StartThread;
   SCKnameResolver m_NameResolver;
   bool m_EndLoop;
   COLthreadPool* m_pThreadPool;

// SCK_SELECT should really say POSIX - it's Select,Kqueue, Epoll
#ifdef SCK_SELECT
public:
   int maxFd() const;
   int countOfFd() const;

   // TODO switch to COLclosure0
   void registerWriteCallback(int Socket, COLclosure0* pWriteCallback);
   void registerReadCallback(int Socket, COLclosure0* pWriteCallback);

   void registerCallbacks(int Socket, COLclosure0* pWriteCallback, COLclosure0* pReadCallback, COLclosure0* pErrorCallback);

   // This is part of the implementation used by SCKconnection and SCKlistener

   void addWriteCallback(int Socket);
   void addReadCallback (int Socket);
   void removeWriteCallback(int Socket);
   void removeAllCallback(int Socket);

   void onWakeupPipe();
private:
   SCKpoller* m_pPoller;

   int m_FdCount;

   // This part of the mechanics required to post events back into the queue.
   COLthreadQueue<COLauto<COLclosure0> > m_ThreadQueue;
   int m_Pipe[2];  // pipe used to wake up loop.

   SCKcallbacks m_WriteCallback;
   SCKcallbacks m_ReadCallback;

   // Only used for Windoze - but not much more overhead having it here.
   SCKcallbacks m_ErrorCallback;

#endif // SCK_SELECT
public:

#ifdef SCK_IOCP
public:
   // Below this is IOCP implementation stuff.
   LPFN_CONNECTEX            connectEx()            { return m_pConnectExFunc;        }
   LPFN_ACCEPTEX             acceptEx()             { return m_pAcceptExFunc;         }
   LPFN_DISCONNECTEX         disconnectEx()         { return m_pDisconnectExFunc;     }
   LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockAddrs() { return m_pGetAcceptExSockAddrs; }

   void addCallback   (int SocketHandle, COLclosure3<const WSAOVERLAPPED&, DWORD, bool>* pRunnable);
   void removeCallback(int SocketHandle);

   int lastError() { return m_LastError; }
private:
   void init();
   void loadApi();
   void createIoPort();

   // We have to dynamically look these functions up from the operating system at runtime.
   LPFN_ACCEPTEX     m_pAcceptExFunc;
   LPFN_CONNECTEX    m_pConnectExFunc;
   LPFN_DISCONNECTEX m_pDisconnectExFunc;
   LPFN_GETACCEPTEXSOCKADDRS m_pGetAcceptExSockAddrs;

   HANDLE m_CompletionPort;

   int m_LastError;

   void recordLastError();
   void dispatchToReceiver(ULONG_PTR pCompletionKey, const WSAOVERLAPPED& pIsCurrentOverLapped, DWORD CurrentSizeOfData, bool IsLastOpSuccessful);

   // Common
   COLhashmap<int, COLauto<COLclosure3<const WSAOVERLAPPED&, DWORD, bool>>> m_CallbackMap;
#endif // SCK_IOCP
   SCKloop(const SCKloop& Orig);  // not allowed
   SCKloop& operator=(const SCKloop& Orig); // not allowed
};

#endif // end of defensive include
