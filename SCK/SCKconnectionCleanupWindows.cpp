#include <SCK/SCKconnection.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKconnectionRegister.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;



void SCKconnection::cleanUp(){
   COL_METHOD(SCKconnection::cleanUp);
   if (m_Handle != SCK_INVALID_SOCKET){
      COL_TRC("Closing socket: " << m_Handle);
      //shutdown(m_Handle, SD_BOTH);
      m_pLoop->removeCallback(m_Handle);
      if (m_HardClose){
         struct linger so_linger = { true, 0 };
         COL_TRC("Initiating hard close");
         setsockopt(m_Handle, SOL_SOCKET, SO_LINGER, (char *)&so_linger, sizeof(so_linger));
      } else {
         COL_TRC("We have to call DisconnectEx otherwise queued data will be lost.");
         (*m_pLoop->disconnectEx())(m_Handle, NULL, 0,0);
      }
      COL_TRC("Call CancelIo");
      ::CancelIo((HANDLE)m_Handle);
      COL_TRC("Then call closesocket");
      int closesocketRC = ::closesocket(m_Handle);
      COL_VAR(closesocketRC);
      m_Handle = SCK_INVALID_SOCKET;
   }
   SCKconnectionUnregister(m_pLoop, m_Id);
   delete this;
}