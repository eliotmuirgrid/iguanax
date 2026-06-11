function STCeditButtonUpdate(){
   let Edit = STCeditPermission();
   let Args = {};
   Args.enabled = Edit;
   Args.target = document.querySelector(".SETeditCatalogue");
   Args.disabledTitle = "You do not have permission to edit this.";
   BUTTONcontrol(Args);
}