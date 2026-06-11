function IMPimportSave(){
   let NewData    = JSON.parse(JSON.stringify(IMPtoFileData));
   let ImportData = JSON.parse(JSON.stringify(IMPfromFileData));
   let ImportList = JSON.parse(JSON.stringify(IMPimportNodeList));
   for(NodeName in IMPimportNodeList){ if(ImportList[NodeName]) NewData["nodes"][NodeName] = ImportData["nodes"][NodeName]; }
   for(Rule of IMPimportMatchingList) NewData["matching"].push(Rule);
   let ApiData = {};
   ApiData.path = IMPtoFile;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(NewData);
   ApiData.development = TRANSdevelopment();
   APIcall('component/edi/set', ApiData, function(R){ 
      if(!R.success) SNCKsnackbar("error", "Error importing:" + R.error);
      TRANSfileCurrentReload();
      MODELremove('IMPmodel');
   });
}