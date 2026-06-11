function DASHclearQueue(ComponentGuids = DASHlistOfCheckedComponents()){ // TODO: need to implement this in Queueing system
   APIcall("component/clear_queue", {'components': ComponentGuids}, function(Response) {
      let Data = Response.data;
      if(!Response.success || !Data) return SNCKsnackbar('error', `Failed to clear queue: ${Response.error}`);
      if (Data.errors.length > 0) {
         SNCKsnackbar('error', 'Failed to clear queue count for ' + Data.errors.length + ' component(s).', 10000);
      } else if (Data.cleared.length === ComponentGuids.length) {
         SNCKsnackbar('success', 'Clearing queue count for all selected components.', 10000);
      } else if (Data.cleared.length > 0) {  // some component has 0 queue count, so no-op for them
         SNCKsnackbar('success', 'Clearing queue count for ' + Data.cleared.length + ` component${Data.cleared.length === 1 ? "": "s"}.`, 10000);
      } else if (Data.skipped.length > 0) {
         SNCKsnackbar('info', 'Skipped ' + Data.skipped.length + ` component${Data.skipped.length === 1 ? "": "s"} because their queue count${Data.skipped.length === 1 ? " is": "s are"} zero.`, 10000);
      }
   });
}
