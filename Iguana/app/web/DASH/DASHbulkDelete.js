function DASHdeleteComponents(ComponentIds = DASHlistOfCheckedComponents())       { DASHdeleteComponent(ComponentIds); }
function DASHcanComponentsBeDeleted(ComponentIds = DASHlistOfCheckedComponents()) { return DASHcomponentListCanBeDeleted(ComponentIds); }

function DASHcomponentListCanBeDeleted(componentList) {
   let MapData = DASHmapData();
   for( let i = 0; i < componentList.length; i++){
      let Guid = componentList[i];
      let ComponentInfo = MapData[Guid];
      if( !ComponentInfo ) {
         console.log("Cannot delete a component which cannot be found " + Guid);
         return false;
      }
      if (DASHisComponentRunning(Guid) ){
         console.log("Cannot delete a running component.", Guid);
         return false;
      }
   }
   return true;
}

function DASHdeleteSelected(ComponentGuids){
   DASHresetBulkMenu();
   if( DASHcanComponentsBeDeleted(ComponentGuids) ) DASHbulkActionDialog("Confirm Bulk Deletion", "DELETION", "delete" , ComponentGuids, () => { DASHdeleteComponents(ComponentGuids); });
   else                               DASHcannotDelete();
}

function DASHcannotDelete(){
   MODELforcePoll('DASHmodel');
   SNCKsnackbar("error", "Bulk delete failed: Stop components before deleting.");
}

function DASHgetComponentList(Data){
   let ComponentIds = [];
   for(let i=0; i<Data.length; i++){
      ComponentIds.push(Data[i].guid);
   }
   return ComponentIds;
}

function DASHdeleteComponent(ComponentIds) {
   APIcall('component/delete', {'components': ComponentIds}, function(Response) {
      if (Response.success) {
         let ComponentsData = Response?.data?.removed || [];
         let Components = DASHgetComponentList(ComponentsData);
         let Message = GINcommitMessage("Deleted", ComponentsData);
         GINaddAndCommit({message: Message, components: Components, all: true});
         Response = Response.data;
      }
      if(Response.errors){
         let Msg = Response.removed.length + (Response.removed.length == 1 ? ' component ' : ' components ') + 'deleted. ';
         Msg += Response.errors.length + (Response.errors.length == 1 ? ' component ' : ' components ') + 'could not be deleted due to lack of permissions. <a href="/#logs?tags=%23error&filter=true">View error in Logs</a>';
         SNCKsnackbar('error', Msg, 10000, false);
      }
      else if(Response.skipped.length){
         let Msg = Response.removed.length + (Response.removed.length == 1 ? ' component ' : ' components ') + 'deleted. ';
         Msg += Response.skipped.length + (Response.skipped.length == 1 ? ' component ' : ' components ') + 'could not be deleted while running.';
         SNCKsnackbar('info', Msg, 10000);
      } else {
         let SuccessMessage = `${Response.removed.length} component${Response.removed.length == 1 ? '' : 's'}`;
         SuccessMessage += ` deleted`;
         SNCKsnackbar('success', SuccessMessage, 1000);
      }
      for(let i=0; i < ComponentIds.length; i++){
         delete DASHcheckedComponents[ComponentIds[i]];
      }
      MODELforcePoll('DASHmodel');
   });
}
