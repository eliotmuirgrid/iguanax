function SMPLsampleDataError(){
   let Error = MODELdata?.SMPLdataObject?.error;
   if(!Error) return;
   SNCKsnackbar('error', Error, 6000);
}

function SMPLsampleDataManipulation(){
   if (TRANScomponentGuid() !== SMPLcomponentId()) return;
   let SampleCount = SMPLdataCount();
   if(!SampleCount) {
      if(!SMPLindexVal()) return;
      return SMPLsetSampleIndex(0);
   }
   if (SMPLindex() > SampleCount) {
      COL_TRC("Sample data index higher than available data.");
      SMPLsetSampleIndex(SampleCount);
   }
}

// Sample Data main function
function SMPLsetUpToolbar() {
   COL_TRC("Setting up sample toolbar");
   // init sample data index from url or local storage
   // fetch sample data file list
   // SMPLtoolbarSetUpNavEdit();
   // SMPLtoolbarSetUpHexByte();
   SMPLtoolbarSetUpFileType();
}
