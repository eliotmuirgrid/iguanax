// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTRNideAnnotation
//
// Description:
//
// Implementation of tests
//
// Author: Vismay Shah
// Date:   Tue 06/20/2023 
// ---------------------------------------------------------------------------
#include "testCurl.h"

#include <curl/curl.h>
#include <UNIT/UNITapp.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure.h>

#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class AnnState{
public:
   bool m_Cancelled = false;
};

static void ShutdownLoop(SCKloop* pLoop){
   COL_FUNCTION(ShutdownLoop);
   pLoop->shutdown();
}

// This is a callback that can be used by cURL when receiving response data
static size_t ReceivedCurlData(void* pBuffer, size_t SizeOfItem, size_t NumberOfItem, void* vpResponseDataObject) {
   COL_FUNCTION(ReceivedCurlData);
   size_t Size = SizeOfItem * NumberOfItem;
   // The cURL object is built so that the user data object is a reference to a COLstring buffer
   reinterpret_cast<COLstring*>(vpResponseDataObject)->append((const char *)pBuffer, Size);
   return Size;
}

static bool CurlCheckRequestDone(CURLM* pMultiHandle){
   // COL_FUNCTION(CurlCheckRequestDone); // noisy since its called in a busy loop
   int MessagesLeft;
   CURLMsg* pMessage = curl_multi_info_read(pMultiHandle, &MessagesLeft);
   if (pMessage && pMessage->msg == CURLMSG_DONE) {
      if (pMessage->data.result == CURLE_OK) { COL_TRC("Request completed successfully."); } 
      else                                   { COL_TRC("Request failed."); }
      // CURLMSG_DONE means the transfer is complete
      return true;
   }
   return false;
}

static void CurlRequestThread(SCKloop* pLoop, AnnState* pState){
   COL_FUNCTION(CurlRequestThread);
   CURL* pHandle = curl_easy_init();
   if (pHandle) {
      CURLcode Result;
      // Call a Iguana 6 HTTPs channel which waits 10s before responding to fake a web connection which takes a while to respond
      curl_easy_setopt(pHandle, CURLOPT_URL, "http://localhost:6544/");
      curl_easy_setopt(pHandle, CURLOPT_TIMEOUT, 15L); // manually set the curl timeout to a value greater than the time the Iguana channel waits
      // Set the write callback function - this is basically LNETonReceivedCurlData
      curl_easy_setopt(pHandle, CURLOPT_WRITEFUNCTION, ReceivedCurlData);

      // We can create an interruptible curl request using curl_multi
      // Create a multi handle and add the curl* from the easy init to it 
      CURLM* pMultiHandle = curl_multi_init();
      curl_multi_add_handle(pMultiHandle, pHandle);
      int RunningHandles;
      // curl_multi_perform executes the request(s) - we have to check for pending messages
      do {
         curl_multi_perform(pMultiHandle, &RunningHandles);
         // Check if the request should be interrupted
         if (pState->m_Cancelled) {
            COL_TRC("Interrupted - exit.");
            curl_multi_remove_handle(pMultiHandle, pHandle);
            pLoop->post(COLnewClosure0(&ShutdownLoop, pLoop));
            break;
         }
         if (CurlCheckRequestDone(pMultiHandle)) { break; }
      } while (RunningHandles);
      // Clean up the multi handle and easy handle
      curl_multi_cleanup(pMultiHandle);
      curl_easy_cleanup(pHandle);
   }
}

static void cancelConnection(AnnState* pState){
   COL_FUNCTION(cancelConnection);
   COL_TRC("Simulating connection gone. Setting m_Cancelled to true");
   pState->m_Cancelled = true;
}

// curl/request
// Proof of concept test which shows how we might be able to get interruptible curl requests
// makes use of curl_multi to asynchronously run the curl task, and we can check on it periodically
// to see if the data has come in. We can also cancel a running handle this way.
// 
// test by having an Iguana 6 HTTPs channel that looks like the one below. Its important that the sleep
// be at least 2 seconds or so, so we have time to cancel our curl request before Iguana 6 can respond, 
// and also important that the sleep be less than the specified curl timeout time of 15 seconds. 
// 
// function main()
//    util.sleep(5000)
//    net.http.respond{body="Hello ProductX"}
// end 
static void testCurlRequest(){
   COL_FUNCTION(testCurlRequest);
   // This main thread is the lua thread 
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   // Mock of the annotation state object, we only care about the m_Cancelled boolean part of it
   AnnState State;
   // On a worker thread, make a curl request
   Loop.threadPool()->scheduleTask(COLnewClosure0(&CurlRequestThread, &Loop, &State));
   // Also schedule m_Cancelled to be set to true after 1 second, simulating another annotation 
   // request coming in, flagging this one as cancelled.
   Loop.addTimer(COLnewClosure0(&cancelConnection, &State), 1000, false);
   Loop.start();

   COL_TRC("Done");

}  

static void testSimple(){

}

void testCurl(UNITapp* pApp) {
   // pApp->add("curl/request",             &testCurlRequest);  // see comment above test
   pApp->add("test/simple", &testSimple);
}
