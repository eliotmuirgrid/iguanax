const DASHbulkStopPollers = new Map(); // job_id → intervalId

function DASHbulkStopPollerStart(JobId) {
   if (DASHbulkStopPollers.has(JobId)) {
      return SNCKsnackbar('error', "A request to stop component(s) is currently in progress.", 10000);
   }
   const intervalID = setInterval(() => {
      DASHbulkStopPollerCheck(JobId);
   }, 1000);
   DASHbulkStopPollers.set(JobId, intervalID);
}

function DASHbulkStopPollerStop(JobId) {
   if (!DASHbulkStopPollers.has(JobId)) return;
   clearInterval(DASHbulkStopPollers.get(JobId));
   DASHbulkStopPollers.delete(JobId);
}

function DASHbulkStopPollerCheck(JobId){
   let Args = { 'job_id' : JobId };
   APIcall('component/stop/check', Args, function(R){
      if (R.success && R.data.running) { return; } // still in progress
      DASHbulkStopPollerStop(JobId);
		if (R.error) {
			return SNCKsnackbar('error', R.error, 10000);
		} else {
			return SNCKsnackbar('success', "Components successfully stopped.", 5000);
      }
   });
}
