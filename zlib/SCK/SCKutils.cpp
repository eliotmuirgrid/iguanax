//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKutils
//
// Author: Kiril Zorin
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "SCKutils.h"
#include "SCKaddress.h"
#include "SCKerror.h"

#ifndef _WIN32
#  include <signal.h>
#  include <netinet/tcp.h>   // For TCP_NODELAY
#endif

#include <errno.h>  // For windows getting EINTR and EAGAIN

#include <COL/COLerror.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define SCK_IPV6_SUPPORTED 1

#ifndef NI_MAXHOST
#define NI_MAXHOST 1025
#endif

#ifdef SCK_IPV6_SUPPORTED
static void SCKhostInfoToIpAddresses(addrinfo* pCurrentHost, COLarray<SCKaddress>& Out){
   COL_FUNCTION(SCKhostInfoToIpAddresses);
   while (pCurrentHost){
      SCKaddress IpAddress;
      IpAddress.copy(pCurrentHost->ai_addr, pCurrentHost->ai_addrlen);  // see nice Ipv4 Ipv6 abstraction :-)
      Out.push_back(IpAddress);
      pCurrentHost = pCurrentHost->ai_next;
   }
}
#endif

void SCKhostResolveAllIps(const COLstring& HostName, COLarray<SCKaddress>* pOut){
   COL_FUNCTION(SCKhostResolveAllIps);
   COLarray<SCKaddress>& Out = *pOut;
   //prefer getaddrinfo when available
#if !defined(SCK_IPV6_SUPPORTED)
   COL_TRC("Invoking old yucky gethostbyname");
   // threadsafe on HPUX and Windows
   hostent* pHostEnt = gethostbyname( HostName.c_str() );
   if (pHostEnt){
      char** ppCurrentAddress = pHostEnt->h_addr_list;
      while(*ppCurrentAddress){
         SCKaddress IpAddress;
         sockaddr_in Address = {0};
         Address.sin_family = AF_INET;
         Address.sin_addr.saddr = *ppCurrentAddress;
         IpAddress.copy((sockaddr*)&Address, sizeof(sockaddr_in));
         Out.push_back(IpAddress);
         ppCurrentAddress++;
      }
   } else {
      throw COLerror(COLstring("Could not resolve hostname ") + HostName, COLerror::PreCondition);
   }
#else
   //try SCKv4 before SCKv6
   struct addrinfo Hints;
   memset(&Hints, 0, sizeof(Hints));
   Hints.ai_family = AF_INET;
   Hints.ai_socktype = SOCK_STREAM;
   struct addrinfo* pHostInfo;
   int Result;
   if ((Result = getaddrinfo(HostName.c_str(), NULL, &Hints, &pHostInfo)) || !pHostInfo){
      //try ipv6 if available
      memset(&Hints, 0, sizeof(Hints));
      Hints.ai_family = AF_INET6;
      Hints.ai_socktype = SOCK_STREAM;
      if ( !SCKsocketIpV6Supported() || ( Result = getaddrinfo(HostName.c_str(), NULL, &Hints, &pHostInfo)) ||  !pHostInfo ) {
         COLstring ErrorDescription = gai_strerror(Result); // since we need to shut down winsock before throwing
         COL_VAR(ErrorDescription);
         return;
      } else {
         SCKhostInfoToIpAddresses(pHostInfo, Out);
         freeaddrinfo(pHostInfo);
      }
   } else{
      SCKhostInfoToIpAddresses(pHostInfo, Out);
      freeaddrinfo(pHostInfo);
   }
#endif
}

// For non blocking version of this function look at the use function with COLthreadPool in SCKnameResolver - blocking function.
bool SCKhostResolve(const COLstring& HostName, SCKaddress* pAddress){
   COL_FUNCTION(SCKhostResolve);
   COL_TRC("Attempting to resolve " << HostName);
   if (pAddress->fromString(HostName)){
      return true;
   }
   COLarray<SCKaddress> AllIps;
   SCKhostResolveAllIps(HostName, &AllIps);
   if (AllIps.size() > 0){
      *pAddress = AllIps[0];
      return true;
   }
   return false;
}

COLstring SCKipResolve(const SCKaddress& IpAddress){
   COL_FUNCTION(SCKipResolve);
   COL_VAR(IpAddress);
   char NameBuffer[NI_MAXHOST] = {0};
   if (!getnameinfo(IpAddress.addressPtr(), IpAddress.addressSize(), NameBuffer, sizeof(NameBuffer), NULL, 0, NI_NAMEREQD)) {
      return SCKsocketError(SCKlastSocketError());
   } else {
      return NameBuffer;
   }
}

COLstring SCKlocalHostName(){
   COL_FUNCTION(SCKlocalHostName);
   char HostName[256] = "";
   if (0 != ::gethostname(HostName, sizeof(HostName))){   // gethostname() function is same in unistd.h and winsock2.h
      int LastSocketError = SCKlastSocketError();
      throw COLerror(SCKsocketError(LastSocketError), LastSocketError);
   }
   COL_TRC("Successfully retrieved hostname: " << HostName);
   return HostName;
}

bool SCKwasErrorAddressInUse(int ErrorCode){
   return ErrorCode == SCK_EADDRINUSE;
}

COLstring SCKlocalIpAsString(){
   COL_FUNCTION(SCKlocalIpAsString);
   COLstring HostSCKstring;
   SCKaddress HostSCK;
   bool Result = SCKhostResolve(SCKlocalHostName(), &HostSCK);
   HostSCKstring = HostSCK.toString();
   COL_VAR(HostSCKstring);
   return HostSCKstring;
}

bool s_IpV6Supported = false;

void SCKcheckIp6Support(){
   COL_FUNCTION(SCKcheckIp6Support);
   //a bit of a hacky way to detect SCKv6 at runtime. This is because
   //we could have compiled on a platform/compiler with SCKv6 support
   //but running on one without it. e.g, win2k
#ifdef SCK_IPV6_SUPPORTED
   int Handle = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
   if (SCK_INVALID_SOCKET != Handle){
      COL_TRC("SCKV6 Support detected.");
      s_IpV6Supported = true;
      ::closesocket(Handle);
   } else {
      COL_TRC("No SCKV6 support detected.");
   }
#endif
}

void SCKinitWinsock(){
   COL_FUNCTION(SCKinitWinsock);
#ifdef _WIN32
   WORD VersionRequested = MAKEWORD(1,1);
   WSADATA WsaData;
   int ErrorCode;
   ErrorCode = ::WSAStartup(VersionRequested, &WsaData);

   if (0 != ErrorCode) {
      COL_ERROR_STREAM("Couldn't find a suitable WinSocket DLL", (COLerror::COLerrorCode)ErrorCode);
   }
   COL_TRC("Set up Winsock " << (int)HIBYTE(WsaData.wVersion) << '.' << (int)LOBYTE(WsaData.wVersion));
#endif
   SCKmaskSignals();
   SCKcheckIp6Support();
}

bool SCKsocketIpV6Supported(){
   COL_FUNCTION(SCKsocketIpV6Supported);
#ifdef SCK_IPV6_SUPPORTED
   COL_VAR(s_IpV6Supported);
   return s_IpV6Supported;
#else
   COL_TRC("SCKV6 support not compiled in.");
   return false;
#endif
}

void SCKmaskSignals(){
   COL_FUNCTION(SCKmaskSignals);
#ifndef _WIN32
   // If you attempt to write to a broken pipe or a disconnected socket then the signal SIGPSCKE will be raised,
   // and its default behaviour is to terminate the process responsible. This behavior is appropriate for programs
   // that are intended for use in a pipeline, but in most other circumstances it is undesirable and should be prevented.
   struct sigaction HandlerInfo = {0};
   HandlerInfo.sa_handler = SIG_IGN;
   if (-1 == ::sigaction(SIGPIPE, &HandlerInfo, NULL)){
      COL_THROW_STRERROR("sigaction() failed.");
   }
#else
   COL_TRC("This is a no op under Windows.");
#endif
}

const char* SCKfamilyAsString(int Family){
   switch(Family){
   case AF_INET : return "AF_INET";
   case AF_INET6: return "AF_INET6";
   }
   COL_ERR("Unknown SCK family type!");
   return "UNKNOWN!!";
}

void SCKdisableNagleAlgorithm(int Handle, int* pErrorCode){
   COL_FUNCTION(SCKdisableNagleAlgorithm);
   int LastError = 0;
   int TcpNoDelay = 1;
   if (-1 == ::setsockopt(Handle, IPPROTO_TCP, TCP_NODELAY, (char*)&TcpNoDelay, sizeof(TcpNoDelay))){
      *pErrorCode = SCKlastSocketError();
      COL_ERR("We have a socket error: " << *pErrorCode);
   }
}

int SCKsetNonblockingOptions(int Handle, bool NonBlocking) {
   COL_FUNCTION(SCKsetNonblockingOptions);
   int LastError = 0;
#ifdef _WIN32
   // TODO: does inheritance belong here? Maybe SCKcreateSocket()?
   COL_TRC("SetHandleInformation HANDLE_FLAG_INHERIT | HANDLE_FLAG_PROTECT_FROM_CLOSE");
   if (0 == ::SetHandleInformation((HANDLE) Handle, HANDLE_FLAG_INHERIT | HANDLE_FLAG_PROTECT_FROM_CLOSE, 0)){
      LastError = ::GetLastError();
   }
   u_long Arg = NonBlocking ? 1 : 0;
   if( ::ioctlsocket(Handle, FIONBIO, &Arg) ){
      LastError = ::GetLastError();
   }
#else
   // TODO: does inheritance belong here? Maybe SCKcreateSocket()?
   if (-1 == ::fcntl(Handle, F_SETFD, FD_CLOEXEC)){
      LastError = errno;
   }
   int OldFlag = ::fcntl(Handle, F_GETFL);
   int NewFlag = NonBlocking ? (OldFlag | O_NONBLOCK) : (OldFlag & ~O_NONBLOCK);
   if (-1 == ::fcntl(Handle, F_SETFL, NewFlag)){
      LastError = errno;
   }

#if defined(__APPLE__)
   // TODO: Does this belong here? Maybe SCKcreateSocket()?
   // Ask macOS to not generate SIGPIPE signal and return EPIPE instead when we try to send data on a closed socket?
   // https://github.com/google/j2objc/issues/1012#issue-382354761
   // https://github.com/schveiguy/io/issues/41#issue-641254670
   int OptionValue = 1;
   if (-1 == ::setsockopt(Handle, SOL_SOCKET, SO_NOSIGPIPE, (void*)&OptionValue, sizeof(OptionValue))){
      LastError = errno;
   }
#endif
   errno = 0;
#endif
   LastError = SCKmapErrorCode(LastError);
   return LastError;
}

// TODO - do we really need this routine???

// Not all platforms define ECONNABORTED - I define it here to make for a cleaner switch statement in SCKisHarmlesAcceptError
#ifndef ECONNABORTED
#define ECONNABORTED 20000
#endif

bool SCKisHarmlessAcceptError(int Code){
   switch(Code){
   case EINTR:
   case SCK_EAGAIN:       // Resource temporarily unavailable - the call might work if you try later.
   case SCK_ECONNABORTED:
      return true;
   }
   return false;
}

int SCKcreateSocket(int IpFamily){
   COL_TRC("SCKcreateSocket");   // We cannot use COL_FUNCTION because no tracing can happen after calling ::socket in case an error has occurred that could be cleared
   COL_VAR(SCKfamilyAsString(IpFamily));
   COLASSERT(IpFamily == AF_INET6 || IpFamily == AF_INET);
   COL_TRC("Call ::socket");
   int Socket = ::socket(IpFamily, SOCK_STREAM, IPPROTO_TCP);  // SUPER IMPORTANT not to trace after this call in case we get an error.
   return Socket;
}

// This function tries to map from the corno-copia of error codes out there and standardize them.

void SCKsetV6DualStack(int Socket, int IpFamily){
   COL_FUNCTION(SCKsetV6DualStack);
   //if the option is available, and we're in ipv6, try to use dual stack mode.  Note that some platforms don't actually do anything/support this flag
#ifdef SCKV6_V6ONLY
   if (IpFamily == AF_INET6){
      int OptionValue = 0;
      if (-1 == ::setsockopt(Socket, SCKPROTO_SCKV6, SCKV6_V6ONLY, (char*)&OptionValue, sizeof(OptionValue))){
         COLstring LastError = SCKsocketError(SCKlastSocketError());
         COL_ERR("Error setting SCKV6_V6ONLY, code : " << LastError);
      } else {
         COL_TRC("Set dual stack option.");
      }
   }
#endif
}

bool SCKsupportEpoll(){
#ifdef SCK_EPOLL
   return true;
#else
   return false;
#endif
}

bool SCKsupportIocp(){
#ifdef _WIN32
   return true;
#else
   return false;
#endif
}

bool SCKsupportKqueue(){
#ifdef SCK_KQUEUE
   return true;
#else
   return false;
#endif
}

int SCKcreateSocket(struct addrinfo* pAddress) {
   COL_FUNCTION(SCKcreateSocket);
   int Handle = ::socket(pAddress->ai_family, pAddress->ai_socktype, pAddress->ai_protocol);
   if (Handle == -1) { return SCKlastSocketError(); }
   int Error = 0;
   do {
      if ((Error = SCKnoSigPipe(Handle)))     { break; }
      if ((Error = SCKnoNagleAlgo(Handle)))   { break; }
      if ((Error = SCKnoInheritance(Handle))) { break; }
      return Handle;
   } while (false);
   // failed to set socket options
   SCKcloseSocket(Handle);
   return Error; // SCKerror code
}

int SCKcloseSocket(int Socket) {
   COL_FUNCTION(SCKcloseSocket);
#ifdef _WIN32
   ::closesocket(Socket);
#else
   ::close(Socket);
#endif
   // don't care about close error for now
   return 0;
}

int SCKnoInheritance(int Socket) {
   COL_FUNCTION(SCKnoInheritance);
#ifdef _WIN32
   if (0 == ::SetHandleInformation((HANDLE)Socket, HANDLE_FLAG_INHERIT | HANDLE_FLAG_PROTECT_FROM_CLOSE, 0)) {
      return SCKlastSocketError();
   }
#else
   if (-1 == ::fcntl(Socket, F_SETFD, FD_CLOEXEC)){
      return SCKlastSocketError();
   }
#endif
   return 0;
}

int SCKnoNagleAlgo(int Handle) {
   COL_FUNCTION(SCKnoNagleAlgo);
   int Arg = 1;
   if (-1 == ::setsockopt(Handle, IPPROTO_TCP, TCP_NODELAY, (char*)&Arg, sizeof(Arg))) {
      return SCKlastSocketError();
   }
   return 0;
}

// Ask macOS not to generate SIGPIPE signal and return EPIPE instead
// when we try to send data on a closed socket?
// https://github.com/google/j2objc/issues/1012#issue-382354761
// https://github.com/schveiguy/io/issues/41#issue-641254670
int SCKnoSigPipe(int Socket) {
   COL_FUNCTION(SCKnoSigPipe);
#ifdef __APPLE__
   int Arg = 1;
   if (-1 == ::setsockopt(Socket, SOL_SOCKET, SO_NOSIGPIPE, (void*)&Arg, sizeof(Arg))) {
      return SCKlastSocketError();
   }
#endif
   return 0;
}

int SCKsetNonblocking(int Socket, bool NonBlocking) {
   COL_FUNCTION(SCKsetNonblocking);
#ifdef _WIN32
   u_long Arg = NonBlocking ? 1 : 0;
   if (::ioctlsocket(Socket, FIONBIO, &Arg)) {
      return SCKlastSocketError();
   }
#else
   int OldFlag = ::fcntl(Socket, F_GETFL);
   int NewFlag = NonBlocking ? (OldFlag | O_NONBLOCK) : (OldFlag & ~O_NONBLOCK);
   if (-1 == ::fcntl(Socket, F_SETFL, NewFlag)) {
      return SCKlastSocketError();
   }
#endif
   return 0;
}

int SCKsetIoTimeout(int Socket, int TimeoutMs, int Direction) {
   COL_FUNCTION(SCKsetIoTimeout);
   COLASSERT(Direction == SO_RCVTIMEO || Direction == SO_SNDTIMEO);
#ifdef _WIN32
   DWORD OldTimeout;
   int OldTimeoutLen = sizeof(DWORD);
   if (::getsockopt(Socket, SOL_SOCKET, Direction, (char*)&OldTimeout, &OldTimeoutLen) == -1) {
      return SCKlastSocketError();
   }
   if (::setsockopt(Socket, SOL_SOCKET, Direction, (char*)&TimeoutMs, sizeof(DWORD)) == -1) {
      return SCKlastSocketError();
   };
   return OldTimeout;
#else
   struct timeval OldTimeout;
   socklen_t OldTimeoutLen = sizeof(struct timeval);
   if (::getsockopt(Socket, SOL_SOCKET, Direction, (void*)&OldTimeout, &OldTimeoutLen) == -1) {
      return SCKlastSocketError();
   }
   struct timeval NewTimeout;
   NewTimeout.tv_sec  =  TimeoutMs / 1000;
   NewTimeout.tv_usec = (TimeoutMs % 1000) * 1000;
   if (::setsockopt(Socket, SOL_SOCKET, Direction, (void*)&NewTimeout, sizeof(NewTimeout)) == -1) {
      return SCKlastSocketError();
   };
   return OldTimeout.tv_sec * 1000 + OldTimeout.tv_usec / 1000;   // ms
#endif
}

COLstring SCKaddressToString(struct addrinfo* pAddress) {
   void* pInfo;
   if (pAddress->ai_family == AF_INET) { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)pAddress->ai_addr;
      pInfo = &(ipv4->sin_addr);
   } else { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)pAddress->ai_addr;
      pInfo = &(ipv6->sin6_addr);
   }
   COLstring IpString(INET6_ADDRSTRLEN, 0);  // allocate enough for IPv6
   inet_ntop(pAddress->ai_family, pInfo, IpString.get_buffer(), INET6_ADDRSTRLEN);
   IpString.setSize(strnlen(IpString.c_str(), IpString.size()));  // trim
   return IpString;
}
