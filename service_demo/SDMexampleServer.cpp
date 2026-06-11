//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMexampleServer
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "SDMexamplePrecomp.h"
#pragma hdrstop

#include "SDMexampleServer.h"
#include <MT/COLevent.h>
#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <COL/COLsinkNull.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLuint32 SDMexampleServer::SDM_TEST_PERIOD = 1000;

SDMexampleServer::SDMexampleServer() : m_LogStream(new COLsinkNull(), true) {
   COL_METHOD(SDMexampleServer::SDMexampleServer);
}

SDMexampleServer::~SDMexampleServer() {
   COL_METHOD(SDMexampleServer::~SDMexampleServer);
}

void SDMexampleServer::onRun(){
   // One thing to notice here: Under Posix, when the process forks, the working directory is
   // changed to '/' (the root directory). As such, if you want to create a file, you must create it
   // in a directory in which you have the permission to do so. In this case the file goes into the 
   // temp directory.
   COLstring LogFileName = FILtempDir();
   LogFileName += "service.log";
   m_LogStream.setSink(new FILfile(LogFileName, FILfile::Rewrite), true);
   COLmath::resetRandomGenerator();
   COLuint32 Dice = COLmath::randomNumber(3);

   m_LogStream << "Dice == " << Dice << newline;
   if (0 == Dice) {
      m_LogStream << "We have hit the one in three chance of having an exception during start up." << newline;
      m_LogStream << "The service should gracefully shutdown." << newline;
      throw COLerror("Test exception!", COLerror::PreCondition);
   } else {
      m_LogStream << "Server Initiated!" << newline << flush;
   }
   
   do {      
      m_LogStream << "Server Running!" << newline << flush;      
   } while(!m_StopEvent.wait(SDM_TEST_PERIOD));
   m_LogStream << "Server stopped!" << newline << flush;
}


void SDMexampleServer::onStopRequest(){
   COL_METHOD(SDMexampleServer::onStopRequest);
   m_StopEvent.signal();
}
