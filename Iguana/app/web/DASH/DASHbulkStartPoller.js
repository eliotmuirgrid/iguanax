const DASHbulkStartPollers = new Map(); // job_id → intervalId

function DASHbulkStartPollerStart(JobId) {
   if (DASHbulkStartPollers.has(JobId)) {
      return SNCKsnackbar('error', "A request to start component(s) is currently in progress.", 10000);
   }
   const intervalID = setInterval(() => {
      DASHbulkStartPollerCheck(JobId);
   }, 1000);
   DASHbulkStartPollers.set(JobId, intervalID);
}

function DASHbulkStartPollerStop(JobId) {
   if (!DASHbulkStartPollers.has(JobId)) return;
   clearInterval(DASHbulkStartPollers.get(JobId));
   DASHbulkStartPollers.delete(JobId);
}

function DASHbulkStartPollerCheck(JobId){
   let Args = { 'job_id' : JobId };
   APIcall('component/start/check', Args, function(R){
      if (R.success && R.data.running) { return; } // still in progress
      DASHbulkStartPollerStop(JobId);
		if (R.error) {
			return SNCKsnackbar('error', R.error, 10000);
		} else {
			return SNCKsnackbar('success', "Components successfully started.", 5000);
      }
   });
}
