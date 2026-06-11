//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: APIcall.js
//
// Description:
//
// Performs an APIcall to an endpoint.
// The ApiPath variable is the endpoint to be called using CallData as the required arguments. APIcall will do a JSON.stringify for you.
// On success, the function passed in as OnSuccessCallback will be called using the JSON.parsed Response.
// If an error occurs, it is presented as a console error.
//---------------------------------------------------------------------------

// PLEASE TALK TO ELIOT BEFORE MODIFYING THIS FUNCTION!
// IT IS INTENTIONALLY SIMPLE and handles one concern - in the past people have modified this
// with good intentions.  Please don't.
function APIstripForwardSlash(str) {
   if (str.charAt(0) === '/') {
      COL_ERR("All api paths should be relative", str);
      return str.substring(1);
   }
   return str;
}

// This function will strip leading / from calls to make sure it goes to the current Iguana (peer) instance.
function APIcall(ApiPath, CallData, OnSuccessCallback, OnErrorCallback){
   ApiPath = APIstripForwardSlash(ApiPath);
   if (ApiPath==="git/cache/refresh"){
      COL_TRC("Cache call make");
   }
   return APIhostCall(ApiPath, CallData, OnSuccessCallback, OnErrorCallback);
}

// This function can be used to call the host.
function APIhostCall(ApiPath, CallData, OnSuccessCallback, OnErrorCallback){
   const xmlhttp = new XMLHttpRequest();
   xmlhttp.onreadystatechange = function() {
      if (xmlhttp.readyState === XMLHttpRequest.DONE){
         if (xmlhttp.status === 200) {
            OnSuccessCallback(JSON.parse(xmlhttp.responseText));
         } else {
            if (OnErrorCallback) {
               OnErrorCallback()
            }
         }
      }
   };
   try{
      xmlhttp.open("POST", ApiPath, true);
      xmlhttp.send(JSON.stringify(CallData));
   } catch (err){
      if (OnErrorCallback) {
         OnErrorCallback()
      }
   }
   return xmlhttp
}

function APIabortCall(Request) {
   Request.abort();
}

// Function to use if you don't need a success callback
function APIdoNothingCallback(Response){}

/* 
TODO - an idea for MARKing functions so we can tell if a Poller callback or a view call back is doing stuff that it shouldn't 

let SAFEflavor = "FFFF"
let SAFEcurrentCallback = ''

function SAFEsetPoller(){
   SAFEflavor = "SAFEpoller";
}

function SAFEmakeCallback(Func, Flavour){
   return function(R) { console.log(Flavour); SAFEcurrentCallback=Flavour; Func(R); SAFEcurrentCallback = ''; }; 
}

function SAFEapiCall(){
   if (SAFEcurrentCallback === 'Poller'){
      console.warn("Poller callbacks should not do API calls.");
   }
}*/