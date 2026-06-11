#ifndef __SCK_SYSTEM_H__
#define __SCK_SYSTEM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// This file has all the system headers for sockets.
//
// Author:   Eliot Muir
// Date:     Monday, June 9th, 2008 @ 01:35:52 PM
//---------------------------------------------------------------------------

#define SCK_INVALID_SOCKET -1

#ifdef _WIN32
#  define SCK_IOCP
//#  define SCK_SELECT  // ::select can work on windows.
#else
#  define SCK_SELECT  // :for now we use ::select on all platforms.
#  ifdef __APPLE__
#     define SCK_KQUEUE
#  endif
#  ifdef __linux__
#     define SCK_EPOLL // Epoll is only defined on Linux
#  endif
#endif

#ifdef _WIN32
   #include <COL/COLwindows.h>
   #ifdef FD_SETSIZE
      #undef FD_SETSIZE
   #endif 
   #define FD_SETSIZE 2048  // Default is 64.
   #include <winsock2.h>
   #include <objbase.h>
   #include <ws2tcpip.h>
   #include <mswsock.h>

   // #13988, #13942
   // This is a very important include, because of getaddrinfo and freeaddrinfo on older 
   // versions of windows. Without this, you can get dll load errors on older versions of windows like win2k
   // See http://msdn.microsoft.com/en-us/library/ms737931(VS.85).aspx and read the "Support for earlier windows" section
   #ifdef _WIN32
      #include <wspiapi.h>
   #endif

   typedef int SCKsocketLength;
#else  // we have POSIX
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <sys/time.h>
   #include <time.h>
   #include <netinet/in.h>
   #include <sys/un.h>
   #include <unistd.h>
   #include <arpa/inet.h>
   #include <errno.h>
   #include <fcntl.h>
   #include <netdb.h>

   typedef socklen_t SCKsocketLength;

   #define closesocket close
#endif


#endif // end of defensive include
