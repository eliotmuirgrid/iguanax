function TRANScomponentListError(){
   let Data  = PRJcomponentListData();
   if(!Data.length) {
      TRANShandleEditorLaunchError("You do not have any components to view - go to the dashboard to make one");
      return;
   }
   if(!TRANSdevelopment()) return;
   let List = PRJeditableList();
   if(List.length) return;
   TRANShandleEditorLaunchError("You do not have any components that are editable - go to the dashboard to make one editable");
}