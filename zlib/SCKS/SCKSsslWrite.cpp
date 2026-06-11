// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslWrite
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 21 June 2023 - 02:52PM
// ---------------------------------------------------------------------------

#include <SCKS/SCKSsslWrite.h>

#include <SCK/SCKconnection.h>

#include <openssl/bio.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// I just write the entire buffer out.
void SCKSsslWrite(BIO* m_pOutBio, SCKconnection* m_pConnection){
   COL_FUNCTION(SCKSsslWrite);
   int AmountRead;
   if (!m_pConnection){ COL_TRC("There is no connection now."); return; }
   COLstring* pOut = &m_pConnection->bufferOut();
   COL_TRC("Reading from bio to write to socket.");
   int Readable = BIO_pending(m_pOutBio);
   int SizeRequired = Readable + pOut->size();
   if (SizeRequired >= pOut->capacity()){
      COL_TRC("Increasing capacity of outbound buffer to " << SizeRequired);
      pOut->setCapacity(SizeRequired);
   }
   COL_VAR2(Readable, SizeRequired);
   AmountRead = BIO_read(m_pOutBio, pOut->get_buffer()+pOut->size(), Readable);
   COL_TRC(AmountRead << " bytes of data ready to be written to the socket.");
   if (AmountRead <= 0 || !m_pConnection) { 
      COL_TRC("No data ready to be written to the socket.");
      return; 
   }
   pOut->setSize(pOut->size() + AmountRead);
   m_pConnection->rawWrite("", 0);
   COL_DUMP("Out", pOut->c_str(), pOut->size());
   COL_TRC("Done write.");
}