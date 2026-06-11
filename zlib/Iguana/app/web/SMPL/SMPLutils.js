function SMPLindexVal(){
   let Value = parseInt(PAGEhashParameter("sample"));
   return Value;
}

function SMPLindex(){
   let Value = SMPLindexVal();
   if (isNaN(Value)) {
      COL_TRC("No sample data index in URL so default to zero")
      Value = 1;
   }
   // COL_TRC("Sample data index =", Value);
   return Value;
}

function SMPLhandleError(R) {
   if (R.success !== null && R.success == false) {
      SNCKsnackbar('error', R.error, 6000);
      return true;
   }
   return false;
}

function SMPLgetSampleIndex(FileName, Data){
   if (FileName.indexOf( SMPLrootFolder() ) === -1) { return 0; }
   FileName = FileName.split(FILsep())[1];
   let NewIndex = -1;
   let SampleData = Data;
   if(!SampleData) SampleData = SMPLdataObject();
   for (let i = 0; i < SampleData.length; i++) {
      let Sample = SampleData[i];
      if (Sample.name === FileName) {
         NewIndex = i;
         break;
      }
   }
   NewIndex++;    // convert 0-based index to 1-based
   return NewIndex;
}

function SMPLsetIndexWithFileName(FileName) {
   let NewIndex = SMPLgetSampleIndex(FileName);
   if (0 === NewIndex || NewIndex === SMPLindex()) { return; }
   SMPLsetSampleIndex(NewIndex);
}

function SMPLsetSampleIndex(NewIndex) {
   COL_TRC("NewIndex=", NewIndex, "SMPLindex=", SMPLindex());
   if (NewIndex > 0) {
      COL_TRC("Need to make sure this doesn't trigger a hash change...?");
      PAGEsetHashParameter("sample", NewIndex); 
   } else {
      PAGEsetHashParameter("sample");
   }
}

function SMPLrootFolder() { return "Samples" + FILsep(); }

function SMPLeditingSampleData() {
   return TRANSfile().indexOf( SMPLrootFolder() ) == 0;
}

