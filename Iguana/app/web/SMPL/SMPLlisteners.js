function SMPLsetListeners(){
   let Modal = document.querySelector('.SMPLmodal');
   if(!Modal) return;
   Modal.removeEventListener("click", SMPLtoolbarSampleDataClick, true);
   Modal.addEventListener   ("click", SMPLtoolbarSampleDataClick);
   Modal.removeEventListener("change", SMPLtoolbarSampleDataChange, true);
   Modal.addEventListener   ("change", SMPLtoolbarSampleDataChange);
}

function SMPLtoolbarSampleDataClick(e){
   let SampleCount = SMPLdataCount();
   if (e.target.closest('.disabled')) return;
   if( e.target.closest('.SMPLmsgFirst') ) SMPLsetIndex(1);
   if( e.target.closest('.SMPLmsgPrev') )  SMPLsetIndex(SMPLindex() > 1 ? SMPLindex() - 1 : 1);
   if( e.target.closest('.SMPLmsgNext') )  SMPLsetIndex(SMPLindex() < SampleCount ? SMPLindex() + 1 : SampleCount);
   if( e.target.closest('.SMPLmsgLast') )  SMPLsetIndex(SampleCount);
   if( e.target.closest('.SMPLmsgAdd') )  {
      SMPLaddSampleData();
   }
   if( e.target.closest('.SMPLmsgEdit') )  {
      if (SMPLeditingSampleData() || SampleCount == 0) { return; } 
      // else swtich to sample data file by changing the edit parameter
      let SampleData = SMPLdataObject();
      PAGEsetHashParameter("edit", SMPLrootFolder() + SampleData[SMPLindex()-1].name);
   }
}

function SMPLtoolbarSampleDataChange(e){
   let SampleCount = SMPLdataCount();
   if( e.target.closest('.SMPLmsgGoto') ) {
      const InputValue = document.querySelector('.SMPLgoToInput').value;
      let GoTo = 1;
      if (InputValue > 1) GoTo = InputValue;
      if (InputValue > SampleCount) GoTo = SampleCount;
      SMPLsetIndex(GoTo);
   }
}