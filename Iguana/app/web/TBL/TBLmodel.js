let TBLnodeWindows ={};
let TBLlistWindows ={};
let TBLeditList = {};
let TBLeditListWindows = {};
let TBLrenderedFileData = {};
let TBLchangedFiles = [];

function TBLmodelInit(){
   MODELpollerAdd('TBLmodel', 'TBLfileData', 10, 'component/db/load', TBLinputArgs);
   MODELviewAdd  ('TBLmodel', 'TBLupdateChangedFiles');
   MODELviewAdd  ('TBLmodel', 'TBLupdateListWindowsPrimer');
   MODELviewAdd  ('TBLmodel', 'TBLupdateNodeWindowsPrimer');
   MODELviewAdd  ('TBLmodel', 'TBLupdateListWindowsSecond');
   MODELviewAdd  ('TBLmodel', 'TBLupdateAnnotations');
   MODELviewAdd  ('TBLmodel', 'TBLupdateErrorOccured');
   MODELforcePoll('TBLmodel');
   MODELstart    ('TBLmodel');
}

function TBLfileData(){
   return MODELdata?.TBLfileData?.data;
}

function TBLinputArgs(){
   TBLclearWindowGuids(TBLlistWindows);
   TBLclearWindowGuids(TBLnodeWindows);
   let CurrentFiles = TBLgetFileList();
   let Args = {};
   Args.path = CurrentFiles;
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}