function PRJupdateButtonGroup(){
   PRJupdateButtonGroupImp(); // to get tracing.
}

function PRJupdateButtonGroupImp(){
   COL_TRC("PRJupdateButtonGroup");
   let ButtonGroup = document.querySelector('.PRJbuttonGroup');
   if(!ButtonGroup) {
      COL_TRC("No button group found");
      return;
   }
   let ButtonDisplay = "flex";
   if(TRANSreadOnly()) ButtonDisplay = "none";
   if(ButtonGroup.style.display !== ButtonDisplay) {
      ButtonGroup.style.display = ButtonDisplay;
      COL_TRC("Set button group display to " + ButtonDisplay);
   }
}