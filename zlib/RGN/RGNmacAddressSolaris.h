#ifndef __RGN_MAC_ADDRESS_SOLARIS_H__
#define __RGN_MAC_ADDRESS_SOLARIS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: RGNMACAddressBSD
//
// Description:
//
// Returns machine ID
//
// Author: George Ma
// Date:   03/17/2004
//
//---------------------------------------------------------------------------

#include "RGNobfuscate.h"

#include <sys/sockio.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <strings.h>
#include <sys/errno.h>

COLstring RGNmacAddress()
{
   COLstring MACAddress(RGN_MACHINE_ID_MAC_ADDRESS_LENGTH,'\0');

   COLstring HostBuffer;
   HostBuffer.setCapacity(4096);
   struct hostent HostEntry;
   struct utsname SystemName;

   if (uname(&SystemName) < 0)
   {
      // Throughout this function we are returning an empty string if a
      // system call fails.  We don't want a user to know the details
      // of how the machine id is generated.
      //COL_THROW_STRERROR("uname call failed");
      return "";
   }

   for(int ErrorCode; !gethostbyname_r(SystemName.nodename, &HostEntry,
                         (char*)HostBuffer.c_str(), HostBuffer.capacity(), &ErrorCode); )
   {
      if( ErrorCode == ERANGE )
      {
         HostBuffer.resize( HostBuffer.size() * 2 );
         continue;
      }

      //COL_ERROR_STREAM("gethostbyname call failed",COLerror::SystemError);
      return "";
   }

   COLint32 Family = HostEntry.h_addrtype;

   for (char** ppAddressList = HostEntry.h_addr_list;
        *ppAddressList != NULL;
        ppAddressList++)
   {
      char Address[INET6_ADDRSTRLEN];

      if ( inet_ntop(Family,*ppAddressList,Address,sizeof(Address)) == NULL )
      {
         //COL_THROW_STRERROR("inet_ntop call failed");
         return "";
      }

      if (Family == AF_INET)
      {
         COLint32 Socket = socket(AF_INET, SOCK_DGRAM, 0);

         if (Socket < 0)
         {
            //COL_ERROR_STREAM("socket creation failed",COLerror::PostCondition);
            return "";
         }

         struct arpreq ARPRequest;
         memset(&ARPRequest,0,sizeof(ARPRequest));
         struct sockaddr_in* pSocketIn = (struct sockaddr_in*) &ARPRequest.arp_pa;
         bzero(pSocketIn, sizeof(struct sockaddr_in));
         pSocketIn->sin_family = AF_INET;
         memcpy(&pSocketIn->sin_addr, *ppAddressList, sizeof(struct in_addr));
         ioctl(Socket,SIOCGARP,&ARPRequest);
         close(Socket);

         RGNprintMACAddress(MACAddress.get_buffer(),
                            MACaddress.capacity(),
                            (COLuint8*) &ARPRequest.arp_ha.sa_data);
         break;
      }
   }

   if (MACAddress[0] = 0)
   {
      return "";
   }

   return MACAddress;
}

#endif // end of defensive include
