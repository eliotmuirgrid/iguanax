//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PRTutils
//
// Description:
//
// Implementation
//
// Author: Cheryl Chan
// Date:   Monday, March 12th, 2007 @ 10:36:34 AM
//
//---------------------------------------------------------------------------
#include "PRTprecomp.h"
#pragma hdrstop

#include "PRTutils.h"

#if _WIN32
#define FD_SETSIZE 256
#include <winsock2.h>
#else
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

// Apparently INADDR_NONE is not defined on Solaris?
#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

#include <COL/COLlog.h>
COL_LOG_MODULE;

PRTreturn PRTisPortInUse(COLuint16 Port, const COLstring& Interface)
{
   COL_FUNCTION(PRTisPortInUse);
   COL_VAR2(Port, Interface);

   struct sockaddr_in   ServerAddress;
   memset(&ServerAddress, 0, sizeof(ServerAddress));
   ServerAddress.sin_family = AF_INET;
   ServerAddress.sin_port = htons(Port);

   // bind to INADDR_ANY if the user does not specify the interface
   COLuint32 IpAddress;
   if(Interface == "")
   {
      IpAddress = INADDR_ANY;
   }
   else
   {
#if defined(_WIN32)
      // threadsafe on Windows
      IpAddress = inet_addr(Interface.c_str());
      if (INADDR_NONE == IpAddress)
      {
         hostent* pHostEnt = gethostbyname( Interface.c_str() );
         if (pHostEnt)
         {
            COLPRECONDITION(sizeof(COLuint32) == pHostEnt->h_length);
            IpAddress = *((COLuint32*)pHostEnt->h_addr_list[0]);         
         }
         else
         {
            COL_ERR("gethostbyname failed to find address of " << Interface);
            return PRT_SOCKET_ERROR;
         }
      }
#else
      struct addrinfo Hints;
      memset(&Hints, 0, sizeof(Hints));
      Hints.ai_family = AF_INET; // ipv4 only
      Hints.ai_socktype = SOCK_STREAM;

      struct addrinfo* pHostInfo;
      int Result;

      if ((Result = getaddrinfo(Interface.c_str(), NULL, &Hints, &pHostInfo)) || !pHostInfo) 
      {
         COL_ERR("getaddrinfo failed to find address of " << Interface);
         return PRT_SOCKET_ERROR;
      } 

      struct sockaddr_in* pAddress = (struct sockaddr_in*)pHostInfo->ai_addr;
      IpAddress = pAddress->sin_addr.s_addr;
      freeaddrinfo(pHostInfo);
#endif

   }
   ServerAddress.sin_addr.s_addr = IpAddress;

   // Create socket
   int mSocket = ::socket(PF_INET, SOCK_STREAM, 0);

   if(mSocket < 0)
   {
      COL_ERR("socket create failed for Port " << Port);
      return PRT_SOCKET_ERROR;
   }

#ifndef _WIN32
   // #11142 Set the reuse addresses socket option to true on POSIX only.
   // NEVER use SO_REUSEADDR on Windows. It is not remotely the same as POSIX.
   // http://msdn.microsoft.com/en-us/library/ms740621(VS.85).aspx
   int Reuse = 1;
   setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&Reuse, sizeof(int));
#endif

   // try binding
   if(bind(mSocket, (struct sockaddr*) &ServerAddress, sizeof(ServerAddress)) < 0)
   {
#if _WIN32
      closesocket(mSocket);
#else
      close(mSocket);
#endif
      COL_ERR("bind failed for Port " << Port);
      return PRT_PORT_USED;
   }

   // try listening
   if(listen(mSocket, SOMAXCONN) < 0)
   {
#if _WIN32
      closesocket(mSocket);
#else
      close(mSocket);
#endif
      COL_ERR("listen failed for Port " << Port);
      return PRT_PORT_USED;
   }

#if _WIN32
   closesocket(mSocket);
#else
   close(mSocket);
#endif

   COL_INF("port is free: " << Port);
   return PRT_PORT_FREE;
}


PRTreturn PRTmainCheckPort(COLostream& LogStream, COLuint16 Port) {
   COL_FUNCTION(PRTmainCheckPort);
   COL_VAR(Port);

   PRTreturn PortUsed = PRTisPortInUse(Port);

   if ( PortUsed == PRT_PORT_USED) {
      LogStream << "Warning: Port " << Port << " is already in use by another application." << newline;
      COL_WRN("Warning: Port " << Port << " is already in use by another application.");
      return PRT_PORT_USED;
   } else if (PortUsed == PRT_SOCKET_ERROR) {
      LogStream << "Socket error!" << newline;
      COL_ERR("Socket error!");
      return PRT_SOCKET_ERROR;
   } else { 
      LogStream << "There are no port conflicts on port " << Port << newline;
      COL_TRC("There are no port conflicts on port " << Port);
      return PRT_PORT_FREE;
   }
}