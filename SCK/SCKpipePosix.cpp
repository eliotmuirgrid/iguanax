//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKpipe
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thu 22 Jan 2015 11:59:50 EST
//---------------------------------------------------------------------------

#include "SCKpipe.h"
#include "SCKloop.h"
#include "SCKutils.h"
#include "SCKerror.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

SCKpipe::SCKpipe() : m_pLoop(NULL), m_InFd(0), m_OutFd(0){
   COL_METHOD(SCKpipe::SCKpipe);
   COL_VAR2(m_InFd, m_OutFd);
}

SCKpipe::~SCKpipe(){
   COL_METHOD(SCKpipe::~SCKpipe);
   destroy();
} 

void SCKpipe::destroy(){
   COL_METHOD(SCKpipe::destroy);
   COL_VAR2(m_InFd, m_OutFd);
   if (m_InFd){
      if (m_pLoop){
         COL_TRC("Remove callback.");
         m_pLoop->removeAllCallback(m_InFd);
      }
      close(m_InFd);
      close(m_OutFd);
      m_InFd = 0;
      m_OutFd  =0;
   }
}

void SCKpipe::create(){
   COL_METHOD(SCKpipe::create);
   ::pipe(m_AtoB);  // TODO error checking.
   ::pipe(m_BtoA);
}

void SCKpipe::attachA(SCKloop* pLoop){
   COL_METHOD(SCKpipe::attachA);
   ::close(m_BtoA[1]);
   ::close(m_AtoB[0]);
   m_InFd = m_BtoA[0];
   m_OutFd = m_AtoB[1];
   initLoop(pLoop);
}

void SCKpipe::attachB(SCKloop* pLoop){
   COL_METHOD(SCKpipe::attachB);
   ::close(m_BtoA[0]);
   ::close(m_AtoB[1]);
   m_InFd = m_AtoB[0];
   m_OutFd = m_BtoA[1];
   initLoop(pLoop);
}

void SCKpipe::initLoop(SCKloop* pLoop){
   COL_METHOD(SCKpipe::initLoop);
   m_pLoop = pLoop;
   COL_VAR(m_InFd);
   m_pLoop->registerReadCallback(m_InFd, COLnewClosure0(this, &SCKpipe::onReadable));
   m_pLoop->addReadCallback(m_InFd);
}

void SCKpipe::setOnData(COLclosure1<const COLstring&>* pCallback){
   COL_FUNCTION(SCKpipe::setOnData);
   m_pOnData = pCallback;
}

void SCKpipe::setOnClose(COLclosure0* pCallback){
   COL_FUNCTION(SCKpipe::setOnClose);
   m_pOnClose = pCallback;
}

void SCKpipe::setOnError(COLclosure1<int>* pCallback){
   COL_FUNCTION(SCKpipe::setOnError);
   m_pOnError = pCallback;
}

void SCKpipe::write(const COLstring& Data){
   COL_FUNCTION(SCKpipe::write);
   COL_VAR(Data);
   ::write(m_OutFd, (void*)Data.c_str(), Data.size());
}

void SCKpipe::onReadable(){
   COL_METHOD(SCKpipe::onReadable);
   COLstring Buffer;
   Buffer.setCapacity(500);
   int AmountRead = read(m_InFd, (void*)Buffer.c_str(), Buffer.capacity()-1);
   if (AmountRead < 0){
      int Error = SCKlastSocketError();
      COL_VAR(AmountRead);
      COL_ERR(Error << ':' << SCKsocketError(Error));
      if (m_pOnError.get()){
         m_pOnError->run(Error);
      }
      if (m_pOnClose.get()){
         try{
            m_pOnClose->run();
         } catch(const COLerror& Error){
           COL_ERR("ERROR - MAJOR PROBLEM - ON CLOSE HANDLER FOR PIPE THREW EXCEPTION" << newline << Error);
         }
      }
      destroy();
      return;
   }
   COL_VAR(AmountRead);
   if (0 == AmountRead){
      if (m_pOnClose.get()){
         try{
            m_pOnClose->run();
         } catch(const COLerror& Error){
           COL_ERR("ERROR - MAJOR PROBLEM - ON CLOSE HANDLER FOR PIPE THREW EXCEPTION" << newline << Error);
         }

      }
      return;
   }
   Buffer.setSize(AmountRead);
   if (m_pOnData.get()){
      try{
         m_pOnData->run(Buffer);
      } catch(const COLerror& Error){
         COL_ERR("ERROR - MAJOR PROBLEM - ON DATA HANDLER FOR PIPE THREW EXCEPTION" << newline << Error);
      }
   }
}
