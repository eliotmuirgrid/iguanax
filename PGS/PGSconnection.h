#ifndef __PGS_CONNECTION_H__
#define __PGS_CONNECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSconnection
//
// Author: John Qi and Eliot Muir
//
// Description:
//
// Native postgres database connection pool
//---------------------------------------------------------------------------

#include <COL/COLdateTime.h>

typedef void* PQconn;

class PGSconnection{
public:
   PGSconnection(COLuint64 Id, PQconn* pConnection) : m_Id(Id), m_Connection(pConnection), m_LastAccessed(COLdateTime::currentTime()) {};
   ~PGSconnection() {};

   COLuint64 id() const { return m_Id; }
   void accessed() { m_LastAccessed = COLdateTime::currentTime(); }
   const COLdateTime& lastAccessTime() const { return m_LastAccessed; }

private:
   COLuint64 m_Id;
   PQconn* m_Connection;
   COLdateTime m_LastAccessed;
};

#endif // end of defensive include
