function SMPLrenderNavGoto(){
   let TotalCount = document.querySelector('.SMPLtotalCount');
   let GoToInput  = document.querySelector('.SMPLgoToInput');
   if(!TotalCount || !GoToInput) return;
   let SampleCount = SMPLdataCount();
   let TotalCountHtml = SampleCount ? 'OF '+ SampleCount : "NONE";
   if(TotalCount.innerHTML != TotalCountHtml) TotalCount.innerHTML = TotalCountHtml;
   let StyleDisplay = SampleCount ? '' : "none";
   if(GoToInput.style.display !== StyleDisplay) GoToInput.style.display = StyleDisplay;
   if(SampleCount && document.activeElement !== document.querySelector('.SMPLgoToInput')) document.querySelector('.SMPLgoToInput').value = SMPLindex();
}

function SMPLupdateBackArrows(){
   if(SMPLindex() <= 1){
      document.querySelector('.SMPLmsgPrev') .classList.add('disabled');
      document.querySelector('.SMPLmsgFirst').classList.add('disabled');
   }
   else{
      document.querySelector('.SMPLmsgPrev') .classList.remove('disabled');
      document.querySelector('.SMPLmsgFirst').classList.remove('disabled');
   }
}

function SMPLupdateForwardArrows() {
   if(SMPLindex() >= SMPLdataCount()){
      document.querySelector('.SMPLmsgNext') .classList.add('disabled');
      document.querySelector('.SMPLmsgLast').classList.add('disabled');
   }
   else{
      document.querySelector('.SMPLmsgNext').classList.remove('disabled');
      document.querySelector('.SMPLmsgLast').classList.remove('disabled');
   }
}

function SMPLviewEditButton() {
   let Button = document.querySelector('.SMPLmsgEdit');
   if(!Button) return;
   let Enabled = SMPLdataCount();
   if(Enabled){
      if(Button.classList.contains('disabled')) Button.classList.remove('disabled');
   }
   else{
      if(!Button.classList.contains('disabled')) Button.classList.add('disabled');
   }
}

function SMPLrenderAddButton() {
   let Button = document.querySelector('.SMPLmsgAdd');
   if(!Button) return;
   let Enabled = TRANSdevelopment();
   if(Enabled){
      if(Button.classList.contains('disabled')) Button.classList.remove('disabled');
   }
   else{
      if(!Button.classList.contains('disabled')) Button.classList.add('disabled');
   }
}

function SMPLsetIndex(NewIndex) {
   if (SMPLindex() == NewIndex || 0 == SMPLdataCount()) { return; }
   SMPLsetSampleIndex(NewIndex);
   if (SMPLeditingSampleData()) {
      // switch to the new sample data file
      let SampleData = SMPLdataObject();
      const NewPath = SMPLrootFolder() + SampleData[SMPLindex()-1].name;
      TRANSfileChange(NewPath); // This also does TRANSfileChange
   }
}
