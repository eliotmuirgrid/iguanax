#include <SCK/SCKloop.h>
#include <SCK/SCKerror.h>


#include <COL/COLlog.h>
COL_LOG_MODULE;

// Interesting - we could use this as a mechanism of deregistering socket objects etc. TODO
void SCKloop::addCallback(int SocketHandle, COLclosure3<const WSAOVERLAPPED&, DWORD, bool>* pClosure){
   COL_METHOD(SCKloop:addCallback);
   COL_VAR(SocketHandle);
   // TODO - This form of callback may still be prone to use getting IO events back on a socket handle which has been reused.
   m_CallbackMap.add(SocketHandle, pClosure);
   // The third argument is the "completion key" basically some opaque User supplied data that we can put application specific stuff into.
   if (::CreateIoCompletionPort((HANDLE)SocketHandle, m_CompletionPort, (ULONG_PTR)SocketHandle, 0) != m_CompletionPort){
      recordLastError();
      COL_ERR("Failed to create completion port:" << SCKsocketError(lastError()));
   }
}

void SCKloop::removeCallback(int SocketHandle){
   COL_METHOD(SCKloop::removeCallback);
   COL_VAR(SocketHandle);
   m_CallbackMap.removeKey(SocketHandle);
}
