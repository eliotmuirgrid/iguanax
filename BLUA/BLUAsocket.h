#ifndef __BLUA_SOCKET_H__
#define __BLUA_SOCKET_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAsocket
//
// Description:
//    BLUAinstance socket functions
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include <BLUA/BLUAtask.h>
#include <COL/COLauto.h>
#include <COL/COLstring.h>
#include <SCKS/SCKSsslContext.h>

class  BLUAinstance2;
class  SCKaddress;
class  SCKconnection;
class  COLvar;
struct lua_State;

class BLUAsocketSslParams{ // POD class for ssl parameters. Used when passing params into a closure to save closure arguments
public:
   BLUAsocketSslParams(const COLstring& PrivateKey, const COLstring& Certificate, bool VerifyPeer, const COLstring& CAfile);
   COLstring m_PrivateKey;
   COLstring m_Cert;
   COLstring m_CAfile;
   bool m_VerifyPeer;
};

// BLUAtask BLUAsocketOnAcceptTask (int ConnectionId, const COLstring& Address);
// BLUAtask BLUAsocketOnConnectTask(int ConnectionId);
// BLUAtask BLUAsocketOnDataTask   (int ConnectionId, const COLstring& Data);
// BLUAtask BLUAsocketOnWriteTask  (int ConnectionId);
// BLUAtask BLUAsocketOnCloseTask  (int ConnectionId, const COLstring& Data);
// BLUAtask BLUAsocketOnErrorTask  (int ConnectionId, const COLstring& Error);
void BLUAhandleSocketOnAccept (lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAhandleSocketOnConnect(lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAhandleSocketOnData   (lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAhandleSocketOnWrite  (lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAhandleSocketOnClose  (lua_State*& L, const COLvar& Payload, int* pResult);
void BLUAhandleSocketOnError  (lua_State*& L, const COLvar& Payload, int* pResult);

void BLUAhandleHttpOnData     (lua_State*& L, const COLvar& Payload, int* pResult);

void BLUAinstanceTearDownSocket(BLUAinstance2* pInstance);
void BLUAinstancePostSocketEvent(BLUAinstance2* pInstance, BLUAtaskId TaskId);

// must be called from Lua worker thread
bool BLUAinstanceListenAsync (BLUAinstance2* pInstance, int Port, const COLstring& SSLcert, const COLstring& SSLkey, bool VerifyPeer, const COLstring& CAfile, COLstring* pError);
bool BLUAinstanceConnectAsyncNew(BLUAinstance2* pInstance, COLstring Host, int Port, bool Secure, int* pConnId);

// must be called from main thread
void BLUAinstanceConnectAsync(BLUAinstance2* pInstance, COLstring Host, int Port, bool Secure = false);
void BLUAinstanceSendAsync   (BLUAinstance2* pInstance, COLauto<COLstring> Data, int ConnId);
void BLUAinstanceCloseAsync  (BLUAinstance2* pInstance, int ConnId);

BLUAtask BLUAhttpOnDataTask(int ConnectionId, const COLstring& Data, int StreamId = 0);

// Exposed for use in LNEThttpListen to prevent copied code.
void BLUAsetupListener(BLUAinstance2* pInstance, int Port, const BLUAsocketSslParams& SslParams, COLclosure0* pOnAccept, COLstring* pError, SCKSsslAPLNselectCallback pAlpnSelector = nullptr);
void BLUAaddConnectionToMaps(BLUAinstance2* pInstance, SCKconnection* pConnection, int ConnId);
SCKconnection* BLUAsetupRegularOrSslConnection(BLUAinstance2* pInstance, SCKaddress* pClientAddress);

#endif // end of defensive include
