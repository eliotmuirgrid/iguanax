#ifndef __RGN_MAC_ADDRESS_LINUX_H__
#define __RGN_MAC_ADDRESS_LINUX_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNMACAddressLinux
//
// Description:
// Implementation of RGNMACAddress for Linux
//
// Author: George Ma
// Date:   03/17/2004
//
//---------------------------------------------------------------------------

#include "RGNobfuscate.h"
#include <linux/sockios.h>

const char* RGN_MACHINE_ID_INTERFACE_NAME = "eth0";

COLstring RGNmacAddress()
{
   COLstring MACAddress(RGN_MACHINE_ID_MAC_ADDRESS_LENGTH,'\0');
   COLint32 Socket = socket(PF_INET,SOCK_DGRAM,0);

   if (Socket < 0)
   {
      // Checking the return value, but not throwing an exception on failure.
      // We don't want to let a customer know how the machine id is
      // generated
      //COL_ERROR_STREAM("socket creation failed",COLerror::PostCondition);
      return "";
   }

   struct ifreq Buffer;
   strcpy(Buffer.ifr_ifrn.ifrn_name,RGN_MACHINE_ID_INTERFACE_NAME);

   if (ioctl(Socket,SIOCGIFHWADDR,&Buffer) < 0)
   {
      //COL_THROW_STRERROR("ioctl call failed");

      // close the socket and return the empty string if
      // the ioctl call has failed
      close(Socket);
      return "";
   }
   close(Socket);

   RGNprintMACAddress(MACAddress.get_buffer(),
                      MACAddress.capacity(),
                      (COLuint8*) &Buffer.ifr_ifru.ifru_hwaddr.sa_data);
   return MACAddress;
}

#endif // end of defensive include
