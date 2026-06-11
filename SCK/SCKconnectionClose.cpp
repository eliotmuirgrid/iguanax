#include <SCK/SCKconnection.h>
#include <SCK/SCKonClose.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void SCKconnection::close(){
   COL_METHOD(SCKconnection::close);
   COL_VAR(m_Handle);
   m_ClosePending = true;
}

void SCKconnection::hardClose(){
   COL_METHOD(SCKconnection::hardClose);
   COL_VAR(m_Handle);
   m_HardClose = true;
   m_ClosePending = true;
   COL_VAR2(m_HardClose, m_ClosePending);
   checkPendingClose();
}

bool SCKconnection::checkPendingClose(){
   //COL_METHOD(SCKconnection::checkPendingClose);
   COL_DBG("Closepending = " << m_ClosePending << " Callstack = " << m_CallbackStack << " socket " << (void*)this);
   if (m_ClosePending && m_CallbackStack == 0){
      COL_TRC("checkPendingClose is working.");
      SCKonClose(this);
      cleanUp();
      return true;
   }
   return false;
}
