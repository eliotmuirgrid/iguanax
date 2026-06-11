#ifndef __PGS_CONNECTION_POOL_H__
#define __PGS_CONNECTION_POOL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSconnectionPool
//
// Author: John Qi and Eliot Muir
//
// Description:
//
// Native postgres database connection pool
//---------------------------------------------------------------------------
#include "PGSconnection.h"

#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLmutex.h>

typedef void* PQconn;
class COLvar;

class PGSconnectionPool{
public:
   PGSconnectionPool();
   ~PGSconnectionPool();
 
   PQconn* connect(const COLvar& ConnectInfo);  // will return a PQconn if it has one.
   // Force will force close connection, even if pooling is enabled.
   void disconnect(PQconn* pConnection=NULL, bool Force=false);

   COLstring report();

   // This will iterate through all the connections and close them.
   void shutdown();

private:
   COLmutex m_PoolLock;
   int m_ConnectionCount;
   int m_MaxCount;
   int m_ConnectionsMade;

   PQconn* reuseConnection();

   COLlist<PQconn*> m_Connections;
};

#endif // end of defensive include
