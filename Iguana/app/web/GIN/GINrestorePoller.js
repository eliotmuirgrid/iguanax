let GINrestorePollerID = undefined;

function GINrestorePollerStart(){
   if(!GINrestorePollerID) GINrestorePollerID = setInterval(function(){ GINrestorePollerCheck(); }, 2000);
}

function GINrestorePollerStop(){
   if(GINrestorePollerID) {
      clearInterval(GINrestorePollerID);
      GINrestorePollerID = undefined;
   }
}

function GINrestorePollerCheck(){
   let Args = {};
   APIcall('instance/reset/check', Args, function(R){
      if (R.success && R.data.running) { return; } // git instance reset in progress
      GINrestorePollerStop();
      GINSupdateButtonStates(false);
		const statusDiv = document.querySelector('.GINrestoreStatus');
		if (statusDiv) statusDiv.remove();
		if (R.error) {
			return GINrestoreErrorConfirm(R.error);
		}
		GINrestart();
		GINrestoreClose();
   });
}