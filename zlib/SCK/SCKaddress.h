#ifndef __SCK_ADDRESS_H__
#define __SCK_ADDRESS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKaddress
//
// Description:
//
// This is a union around the IP4 and IP6 address data-structures that
// makes it a bit easier to use.  It replaces a less useful abstraction
// written earlier.
//
// Author: Eliot Muir
// Date:   Tue 25 Nov 2014 21:01:08 EST
//---------------------------------------------------------------------------

#include "SCKsystem.h"
#include <COL/COLstring.h>

// POD - allow default copy constructor and assignment operator.
class SCKaddress{
public:   
   SCKaddress();
   SCKaddress(const COLstring& IpAddress, int Port);
   SCKaddress(int Port);

   ~SCKaddress();

   bool set() const;
   
   // Returns AF_INET for IPV4 (default) or AF_INET6. If isSet() is false, then ipv6 is returned
   int ipFamily() const;

   void setIpv6(bool Value);
   bool isIpv6() const;

   void setAny();  // Set's to any host - takes into account IPv4 and IPv6.

   void setPort(int Port);
   int port() const;

   bool operator!=(const SCKaddress& Rhs) const;
   bool operator==(const SCKaddress& Rhs) const;

   // Converts the address into a string.
   COLstring toString() const;

   // This will attempt to convert a string into a IP address
   // The call is non-blocking and will only work on strings
   // in the format NNN.NNN.NNN.NNN (ipv4), and on some platforms
   // will work with IPv6 addresses.  It returns false if the string cannot be formed into an IPaddress.
   // IT DOES NOT resolve hostnames to IP addresses with a DNS lookup - see SCKnameResolver.
   bool fromString(const COLstring& HostString);

   void clear();

   // SCKaddress does keep on to the original human readable name which is quite handy for name resolution issues when you
   // still want to have the original name.
   void setHostName(const COLstring& HostName);
   const COLstring& hostName() const  { return m_HostName; }

   // These methods are used to feed IPaddress into operating system calls like connect, bind, accept etc. 
   sockaddr* addressPtr() const;
   SCKsocketLength addressSize() const;
   // And this method can take a sockaddr(_in|_in6) and copy it into this class - typically it would be coming from a system call.
   void copy(const sockaddr* pAddress, SCKsocketLength Size);
private:
   union {
      sockaddr_in  m_Ip4Address;
      sockaddr_in6 m_Ip6Address;
   };
   COLstring m_HostName;
};

COLostream& operator<<(COLostream& Stream, const SCKaddress& IpAddress);

#endif // end of defensive include
