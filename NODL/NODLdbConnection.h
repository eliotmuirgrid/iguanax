#ifndef __NODL_DB_CONNECTION_H__
#define __NODL_DB_CONNECTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLdbConnection
//
// Description:
//
// Class for representing a database connection in Lua
//
// Author: Vismay Shah
// Date:   Thursday 24 November 2022
// ---------------------------------------------------------------------------
#include <COL/COLrefCounted.h>
#include <DB/DBdatabasePtr.h>

struct lua_State;
class NODLdbConnection : public COLrefCounted {
public:
   NODLdbConnection();
   ~NODLdbConnection();
   //Gets connection parameters from parameter table for current method call.
   void getParamsFromCall(lua_State* L);

   void open(lua_State* L);
   void close(lua_State* L);

   bool isConnectionLive(lua_State* L);
   bool isOperationLive(lua_State* L, bool LiveDefault);
   //If operation isn't live, push a warning as an extra return value.
   //Returns number of additional return values (either 1 or 0).
   int nonLiveConnectionWarning(lua_State* L);
   int pushInfoTable(lua_State* L);

   lua_State* m_L; //needed in destructor
   DBdatabasePtr m_pDb;
   bool m_ConnectionCaching; //Whether connection is cached based on below parameters (deprecated use).
   bool m_Live;   //Whether connection is "live". See isOperationLive() and isConnectionLive().
   bool m_Closed; //Whether connection has been closed by the user.

   int m_DatabaseApiIndex;
   COLstring m_DatabaseApi;
   COLstring m_DataSource;
   COLstring m_UserName;
   COLstring m_Password;
   COLstring m_EncryptionKey;
   bool m_UseUnicode;
   int m_TimeoutSeconds;
   bool m_LastOperationLive;
private:
   NODLdbConnection(const NODLdbConnection& another); //copies not allowed
   NODLdbConnection& operator=(const NODLdbConnection& another); //copies not allowed
};

COLref<NODLdbConnection> NODLdbLoadConnectionHandle(lua_State* L, bool CheckClosed);
COLref<NODLdbConnection> NODLdbGetConnection(lua_State* L, const char* const MethodParams[]);

int NODLdbConnectionDestructor(lua_State* L);
int NODLdbConnectionToString  (lua_State* L);

#endif // end of defensive include
