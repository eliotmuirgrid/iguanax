function DASHcreateButtonUpdate(){
   let Create = DASHpermissionData();
   let Args = {};
   Args.target = document.querySelector(".DASHaddComponentButton");
   Args.enabled = Create;
   Args.enabledTitle = "Add a component from a collection.";
   Args.disabledTitle = "You do not have permission to do this.";
   BUTTONcontrol(Args);

   Args.target = document.querySelector(".DASHaddFromUrlButton");
   Args.enabledTitle = "Add a component from a git url.";
   BUTTONcontrol(Args);
}