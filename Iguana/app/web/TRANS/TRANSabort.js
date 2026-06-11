let TRANSrequests = {}

function TRANSabort(RequestName){
   if (TRANSrequests[RequestName]){
      COL_TRC("Aborted ", RequestName);
      TRANSrequests[RequestName].abort();
      delete TRANSrequests[RequestName];
   } else {
      COL_TRC(RequestName + " - not running");
   }
}

function TRANSabortAll(){
   COL_TRC("Aborting all requests:", Object.keys(TRANSrequests).length);
   for (K in TRANSrequests) {
      COL_TRC("Aborting " + K);
      TRANSrequests[K].abort();
   }
   TRANSrequests ={};
}

function TRANSabortRegister(Name, Request){
   COL_TRC("Register request", Name);
   COL_DBG(Request);
   TRANSrequests[Name] = Request;
}

function TRANSabortRequestDone(Name){
   delete TRANSrequests[Name];
}