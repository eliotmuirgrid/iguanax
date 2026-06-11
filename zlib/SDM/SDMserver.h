#ifndef __SDM_SERVER_H__
#define __SDM_SERVER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMserver
//
// Description:
//
// This class is an abstraction of a windows server or unix daemon.
//
// Application written using this class will be compiled as:
// 1) A service under Windows
// 2) A daemon under Unix
//
// Author:  Eliot Muir
//---------------------------------------------------------------------------
class SDMserverPrivate;

class COLstring;
class COLostream;
class COLsink;
class COLerror;

#include <COL/COLauto.h>
#include <COL/COLclosure.h>

void SDMsetRunningAsService(bool IsService);
bool SDMrunningAsService();

class SDMserver {
 public:
   SDMserver(const COLstring& ServiceName);
   ~SDMserver();
   const COLstring& name() const; // service name

   // This command will run the server as a Service under windows or a
   // daemon under Unix.  Under Windows, if run from commandline we start as a normal process
   bool runServer();

   // This command will run the server as a normal command line process
   void runServerAsNormalProcess();

   void setOnRun(COLauto<COLclosure0> pOnRun)   { m_pOnRun = pOnRun;   }

   friend class SDMserverPrivate;

   COLauto<COLclosure0> m_pOnRun;
 private:
   SDMserverPrivate* pMember;
   SDMserver(const SDMserver& Orig); // Copy
   SDMserver& operator=(const SDMserver& Orig); // Assignment
};

#endif // end of defensive include
