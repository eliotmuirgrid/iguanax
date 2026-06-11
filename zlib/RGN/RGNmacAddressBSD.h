#ifndef __RGN_MAC_ADDRESS_BSD_H__
#define __RGN_MAC_ADDRESS_BSD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNMACAddressBSD
//
// Description:
// Implementation of RGNMACAddress for BSD (and MAC)
//
// Author: George Ma
// Date:   03/17/2004
//
//---------------------------------------------------------------------------

#include "RGNobfuscate.h"

#include <net/if_dl.h>
#include <net/if_types.h>

#include <COL/COLmap.h>

const COLint32 RGN_MACHINE_ID_BUFFER_SIZE = 4096;

void RGNmacAddressList(COLmap<COLstring,COLstring>& AddressList)
{
   struct ifconf InterfaceConfig;
   struct ifreq* pInterfaceRequest;
   struct sockaddr_dl* pSocketAddress;
   char TempBuffer[RGN_MACHINE_ID_BUFFER_SIZE];
   char* pByte;

   COLint32 Socket = socket(AF_INET, SOCK_DGRAM, 0);

   // Check the socket call
   // In RGNMACAddress we are simply returning the empty string
   // if an API call fails.  This is because we don't want to give
   // our users information on how the machine id is generated
   if (Socket < 0)
   {
      //COL_ERROR_STREAM("socket creation failed",COLerror::PostCondition);
      return;
   }

   InterfaceConfig.ifc_len = sizeof(TempBuffer);
   InterfaceConfig.ifc_buf = TempBuffer;

   if (ioctl(Socket, SIOCGIFCONF, &InterfaceConfig) < 0)
   {
      //COL_THROW_STRERROR("ioctl call failed");
      close(Socket);
      return;
   }
   close(Socket);

   for (pByte = TempBuffer; pByte < TempBuffer + InterfaceConfig.ifc_len; )
   {
      pInterfaceRequest = (struct ifreq*) pByte;
      if (pInterfaceRequest->ifr_addr.sa_family == AF_LINK)
      {
       pSocketAddress = (struct sockaddr_dl*) &pInterfaceRequest->ifr_addr;
       if (pSocketAddress->sdl_type == IFT_ETHER)
       {
          COLstring InterfaceName = pInterfaceRequest->ifr_name;
          COLstring MACAddress(RGN_MACHINE_ID_MAC_ADDRESS_LENGTH,'\0');
          RGNprintMACAddress(MACAddress.get_buffer(),
                             MACAddress.capacity(),
                             (COLuint8*) LLADDR(pSocketAddress));
          AddressList.add(InterfaceName, MACAddress);
       }
     }
     pByte += _SIZEOF_ADDR_IFREQ(*pInterfaceRequest);
   }
}

COLstring RGNmacAddress()
{
   COLmap<COLstring,COLstring> AddressList;
   RGNmacAddressList(AddressList);
   COLmapPlace FirstPlace = AddressList.first();
   if(FirstPlace)
   {
      return AddressList[FirstPlace];
   };
   return "";
};


#endif // end of defensive include
