#ifndef __SDM_EXAMPLE_SERVER_H__
#define __SDM_EXAMPLE_SERVER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMexampleServer
//
// Description: 
//
// This class is simple example of writing a service/daemon using
// the SDM library.
// 
// This class inherits the abstract class SDMserver (abstract by virtue that its has
// pure virtual functions).
//
//
//---------------------------------------------------------------------------

#include <SDM/SDMserver.h>
#include <MT/COLevent.h>

class COLstring;

class SDMexampleServer{ 
public:
   SDMexampleServer();
   ~SDMexampleServer();
 
   void onRun();
   void onStopRequest();

   // This variable pertains only to this example. You don't have to include it in your example
   static const COLuint32 SDM_TEST_PERIOD;
private:
   COLevent m_StopEvent;   
   COLostream m_LogStream;
   SDMexampleServer(const SDMexampleServer& Orig); // Not allowed
   SDMexampleServer& operator=(const SDMexampleServer& Orig); // Not allowed
};
#endif // end of defensive include

