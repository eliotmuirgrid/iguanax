//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKaddress
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, March 5th, 2010 @ 04:08:05 PM
//---------------------------------------------------------------------------

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "SCKaddress.h"
#include "SCKutils.h"
#include "SCKerror.h"

SCKaddress::SCKaddress() {
   COL_METHOD(SCKaddress::SCKaddress);
   clear();
}

SCKaddress::SCKaddress(const COLstring& IpAddress, int Port){
   COL_METHOD(SCKaddress::SCKaddress);
   clear();
   setPort(Port);
   fromString(IpAddress);
}

SCKaddress::SCKaddress(int Port){
   COL_METHOD(SCKaddress::SCKaddress);
   clear();
   setPort(Port);
}

SCKaddress::~SCKaddress(){
   COL_METHOD(SCKaddress::~SCKaddress);
}

void SCKaddress::setIpv6(bool New){
   COL_METHOD(SCKaddress::setIpv6);
   COL_VAR(New);
   bool Current = isIpv6(); 
   m_Ip4Address.sin_family = New ? AF_INET6 : AF_INET;
   if (Current != New){
      setAny();
   }
}

bool SCKaddress::isIpv6() const{
   return m_Ip4Address.sin_family != AF_INET;
}

bool SCKaddress::set() const{
   COL_METHOD(SCKaddress::set);
   bool IsSet;
   if (ipFamily() == AF_INET){
      IsSet = m_Ip4Address.sin_addr.s_addr != INADDR_ANY;
   } else {
      IsSet = memcmp(&m_Ip6Address.sin6_addr, &in6addr_any, sizeof(in6addr_any));
   }
   COL_VAR(IsSet);
   return IsSet;
}

void SCKaddress::setAny(){
   COL_METHOD(SCKaddress::setAny);
   if (isIpv6()){
      COL_TRC("Set to in6addr_any");
      m_Ip6Address.sin6_addr = in6addr_any;
   } else{
      m_Ip4Address.sin_addr.s_addr = INADDR_ANY;
   }
}


int SCKaddress::ipFamily() const{
   COL_METHOD(SCKaddress::ipFamily);
   return m_Ip4Address.sin_family;
}

void SCKaddress::clear(){
   COL_METHOD(SCKaddress::clear);
   ::memset(&m_Ip6Address, 0, sizeof(m_Ip6Address));
   m_Ip4Address.sin_family = AF_INET;
   m_Ip4Address.sin_port = 0;
   m_Ip4Address.sin_addr.s_addr = INADDR_ANY;
}

void SCKaddress::setPort(int Port){
   COL_METHOD(SCKaddress::setPort);
   COL_VAR(Port);
   m_Ip4Address.sin_port = htons(Port);
}


int SCKaddress::port() const{
   return ntohs(m_Ip4Address.sin_port);
}

sockaddr* SCKaddress::addressPtr() const{
   return (sockaddr*)&m_Ip4Address;  // address of union so that works for both Ipv4 and ipv6
}

SCKsocketLength SCKaddress::addressSize() const { 
   return ipFamily() == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);
}

bool SCKaddress::operator!=(const SCKaddress& Rhs) const{
   return !operator==(Rhs);}

bool SCKaddress::operator==(const SCKaddress& Rhs) const{
   if (Rhs.ipFamily() == AF_INET ){
      return ::memcmp(&m_Ip4Address, &Rhs.m_Ip4Address, sizeof(sockaddr_in))==0;
   }else {
      return ::memcmp(&m_Ip6Address, &Rhs.m_Ip6Address, sizeof(sockaddr_in6))==0;
   }
}

COLstring SCKaddress::toString() const{
   // Do not use COLlog trace macros in this function
   if (!set()){ return "<any>";}
#if defined(_WIN32)
   COLstring IpInString;
   IpInString.setCapacity(NI_MAXHOST);
   DWORD StringSize = IpInString.capacity();
   int SavePort = port();
   ((SCKaddress*)this)->setPort(0);
   int ReturnCode = ::WSAAddressToString(addressPtr(), addressSize(), NULL, (LPSTR)IpInString.c_str(), &StringSize);
   int LastError = SCKlastSocketError();
   ((SCKaddress*)this)->setPort(SavePort);  // crazy stuff we have to do to use Windows.
   COLstring Out;
   if (ReturnCode){
      throw COLerror(SCKsocketError(LastError), LastError);
   }
   IpInString.setSize(StringSize-1);
   COL_VAR(IpInString);
   return IpInString;
#else
   char NameBuffer[NI_MAXHOST] = {0};
   int Result;
   if ((Result = getnameinfo(addressPtr(), addressSize(), NameBuffer, sizeof(NameBuffer), NULL, 0, NI_NUMERICHOST))){
      COL_ERROR_STREAM_PLAIN(COL_T("Error while resolving SCK address to string: ") << gai_strerror(Result), COLerror::PreCondition);
   }
   return NameBuffer;
#endif
}

COLostream& operator<<(COLostream& Stream, const SCKaddress& IpAddress){
   Stream << IpAddress.toString() << ':' << IpAddress.port();
   return Stream;
}


#ifndef _WIN32  // Posix implementation of SCKaddress::fromString
bool SCKaddress::fromString(const COLstring& HostString){
   COL_FUNCTION(SCKaddress::fromString);
   m_HostName = HostString;
   COL_VAR(HostString);
   setIpv6(false);
   if  (1 == inet_pton(AF_INET, HostString.c_str(), &m_Ip4Address.sin_addr)){
      COL_TRC("Successfully converted the V4 address.");
      return true;
   }
   setIpv6(true);
   if  (1 == inet_pton(AF_INET6, HostString.c_str(), &m_Ip6Address.sin6_addr)){
      COL_TRC("Successfully converted the V6 address.");
      return true;
   }
   return false;
}
#endif

#ifdef _WIN32  // _WIN32 implementation of SCKaddress::fromString
bool SCKaddress::fromString(const COLstring& HostString){
   COL_FUNCTION(SCKaddress::fromString);
   m_HostName = HostString;
   COL_VAR(HostString);
   int Port = port();  // record port.
   setIpv6(false);
   INT AddressSize = addressSize();
   if (-1 != ::WSAStringToAddress((LPSTR)HostString.c_str(), AF_INET, 0, addressPtr(), &AddressSize)){
      COL_TRC("Successfully converted the V4 address.");
      setPort(Port);
      return true;
   }
   if (!SCKsocketIpV6Supported()){
      setPort(Port);
      return false;
   }
   setIpv6(true);
   AddressSize = addressSize();
   COL_VAR(AddressSize);
   COL_VAR(HostString);
   if (-1 != ::WSAStringToAddress((LPSTR)HostString.c_str(), AF_INET6, 0, addressPtr(), &AddressSize)){
      COL_TRC("Successfully converted the V6 address.");
      setPort(Port);
      return true;
   }
   COL_VAR(SCKsocketError(SCKlastSocketError()));
   clear();
   setPort(Port);
   return false;
}
#endif


void SCKaddress::setHostName(const COLstring& HostName){
   COL_METHOD(SCKaddress::setHostName);
   COL_VAR(HostName);
   m_HostName = HostName;
}

// beautiful copying method.
void SCKaddress::copy(const sockaddr* pAddress, SCKsocketLength Size){
   COL_METHOD(SCKaddress::copy);
   COL_VAR2(pAddress, Size);
   memcpy(&m_Ip4Address, pAddress, Size);
   COL_VAR(ipFamily());
}
