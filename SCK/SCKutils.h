#ifndef __SCK_UTILS_H__
#define __SCK_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKutils
//
// Author: Eliot Muir
//
// Description:
//
// This header file has utility functions for which I have not yet thought of
// a better place to put them.
//---------------------------------------------------------------------------

class SCKaddress;
class COLstring;

template<class TValue> class COLarray;

void SCKinitWinsock();

// Returns the host name for the local machine
COLstring SCKlocalHostName();

// Returns the SCK address for the local machine
COLstring SCKlocalIpAsString();

// Warning - this is a blocking operation.
//
// This function will work out if you have supplied a host
// name or an SCK address and will then attempt to return an
// SCK address.  If it fails it will throw an exception.
//
// It only bothers to resolve the first SCK address of the given
// host name - i.e. it ignores the fact that many hosts have multiple
// SCK addresses like websites like Yahoo that have clustering.
bool SCKhostResolve(const COLstring& HostName, SCKaddress* pAddress);

//same as above, but provides all resolved addresses for the given hostname
void SCKhostResolveAllIps(const COLstring& HostName, COLarray<SCKaddress>* pOut);

// Warning - this is a blocking operation - see SCKnameResolver.h for asynchronous interface.
// This function will attempt to resolve the SCK address to
// a hostname.  It will throw an exception if it fails
// The hostname is resolved with SCKv4 before SCKv6
COLstring SCKipResolve(const SCKaddress& IpAddress);

bool SCKwasErrorAddressInUse(int ErrorCode);

//does a test based on platform, compiler if ipv6 is supported
bool SCKsocketIpV6Supported();

// Mask pipe signals.
void SCKmaskSignals();

void SCKsetReuseAddress(int Socket);

const char* SCKfamilyAsString(int Family);

int SCKsetNonblockingOptions(int Handle, bool NonBlocking = true);

void SCKdisableNagleAlgorithm(int Handle, int* pErrorCode);

// There are a variety of accept errors which could be described as harmless since the client could have timed out
// and reset the connection before the server has accepted the connection.
bool SCKisHarmlessAcceptError(int Code);
int SCKcreateSocket(int IpFamily);

//TODO - comment what this is.
void SCKsetV6DualStack(int Socket, int IpFamily);

bool SCKsupportEpoll();
bool SCKsupportIocp();
bool SCKsupportKqueue();

// Create a socket and disable SIGPIPE, Nagle and inheritance
// Return socket handle on success, SCKerror on failure.
int SCKcreateSocket  (struct addrinfo* pAddress);
// Return 0 always (for now, unless we care about close errors)
int SCKcloseSocket   (int Socket);
// Return 0 on success, SCKerror on failure.
int SCKnoInheritance (int Socket);
// Return 0 on success, SCKerror on failure.
int SCKnoNagleAlgo   (int Handle);
// Return 0 on success, SCKerror on failure.
int SCKnoSigPipe     (int Socket);
// Return 0 on success, SCKerror on failure.
int SCKsetNonblocking(int Socket, bool NonBlocking = true);
// Set socket timeout for recv and send. Direction = SO_RCVTIMEO or SO_SNDTIMEO
// Returns the old timeout value in ms on success, SCKerror on failure.
int SCKsetIoTimeout  (int Socket, int TimeoutMs, int Direction);

COLstring SCKaddressToString(struct addrinfo* pAddress);

#endif // end of defensive include
