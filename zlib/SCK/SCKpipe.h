#ifndef __SCK_PIPE_H__
#define __SCK_PIPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKpipe
//
// Description:
//
// Wrapper for a pipe that we can get signals from the message loop from.  It's not
// built in a manner which expects the application to stream out a lot of data, no buffering etc.
//
// Author: Eliot Muir
// Date:   Thu 22 Jan 2015 11:59:50 EST
//---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <COL/COLauto.h>

class SCKloop;

class SCKpipe{
public:
   SCKpipe();
   ~SCKpipe(); 

   void create();
   void destroy();
   // Think of the two ends of the pipe as A and B.
   // If we fork then the child closes one end and the
   // parent process closes the other.
   void attachA(SCKloop* pLoop);
   void attachB(SCKloop* pLoop);

   void setOnData (COLclosure1<const COLstring&>* pCallback);
   void setOnClose(COLclosure0* pCallback);
   void setOnError(COLclosure1<int>* pCallback);
   void write(const COLstring& Data);   

   void onReadable();
private:
   COLauto< COLclosure1<const COLstring&> > m_pOnData;
   COLauto< COLclosure1<int> > m_pOnError;
   COLauto< COLclosure0 > m_pOnClose;
   int m_AtoB[2];
   int m_BtoA[2];

   void initLoop(SCKloop* pLoop);

   int m_InFd;
   int m_OutFd;
   SCKloop* m_pLoop;
};

#endif // end of defensive include
