
function STUmodelInit(){
   MODELpollerAdd('STUmodel', 'STUpermissionData', 10, 'component/permission', {});
   MODELpollerAdd('STUmodel', 'STUdeletePermission', 10, 'component/permission/all', {});
   MODELpollerAdd('STUmodel', 'STUlogStats',     30, 'log/directory', {});
   MODELpollerAdd('STUmodel', 'STUlogComponent', 30, 'log/component/usage', {});
   MODELviewAdd('STUmodel', 'STUrenderLogging');
   MODELviewAdd('STUmodel', 'STUpathUpdate');
   MODELviewAdd('STUmodel', 'STUaltPathUpdate');
   MODELviewAdd('STUmodel', 'STUsizeUpdate');
   MODELviewAdd('STUmodel', 'STUaltSizeUpdate');
   MODELviewAdd('STUmodel', 'STUarchivePathUpdate');
   MODELviewAdd('STUmodel', 'STUarchiveSizeUpdate');
   MODELviewAdd('STUmodel', 'STUfreeUpdate');
   MODELviewAdd('STUmodel', 'STUstatusUpdate');
   MODELviewAdd('STUmodel', 'STUbarUpdate');
   MODELviewAdd('STUmodel', 'STUbarStatusUpdate');
   MODELviewAdd('STUmodel', 'STUpermissionUpdate');
   MODELforcePoll('STUmodel', function(){
      MODELpollerAdd('STUmodel', 'STUarchiveData', 60, 'component/archive_list', {});
      MODELforcePoll('STUmodel');
   });
}

function STUpermissionData(){
   return MODELdata?.STUpermissionData?.data || {};
}

function STUdeletePermission(){
   return MODELdata?.STUdeletePermission?.data?.components || {};
}

function STUbackupData(){
   let data = STUlogdirData();
   return data?.alt_dir || {};
}

function STUlogData(){
   let data = STUlogdirData();
   return data?.log_dir || {};
}

function STUlogdirData(){
   return MODELdata?.STUlogStats?.data || {};
}

function STUaltLogEnabled(){
   let data = STUlogdirData();
   return data?.alt_enabled || false;
}

function STUarchiveData() {
   return MODELdata?.STUarchiveData?.data || {components: []};
}
function STUcomponentData(){
   return MODELdata?.STUlogComponent?.data || {};
}