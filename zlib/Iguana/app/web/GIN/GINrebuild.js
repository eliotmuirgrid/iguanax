function GINrestart() {
   SNCKsnackbar('success', "Instance restarting...", 5000)
   APIcall('instance/restart', {}, function(R) {
      // TODO currently restart auto responds success
      if(!R.success) { return SNCKsnackbar('error', `Error restarting instnance: ${R.error}`); }
   })
}