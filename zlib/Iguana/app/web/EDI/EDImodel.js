// EDIfileData is the source of truth for all of the file data, indexed by path.
// EDIeditList is a map that stores the reference id of the anchor for draggable lists, indexed by windowId
// EDInodeWindows is a map of all the node windows currently open with the nodename and filename indexed by windowId
// EDIlistWindows is a map of all the list windows currently open with their filenames indexed by windowId

let EDImidRequest;
let EDInodeWindows = {};
let EDIlistWindows = {};
let EDImatchingWindows = {};
let EDIrenderedFileData = {};
let EDIeditList = {};
let EDIeditListWindows = {};
let EDIchangedFiles = [];

function EDIreset(){
   EDImidRequest = undefined;
   EDInodeWindows = {};
   EDIlistWindows = {};
   EDImatchingWindows = {};
   EDIrenderedFileData = {};
   EDIeditList = {};
   EDIeditListWindows = {};
   EDIchangedFiles = [];
}

function EDImodelInit(){
   EDIreset();
   MODELpollerAdd('EDImodel', 'EDIfileData', 5, 'component/edi/load', EDIinputArgs);
   MODELviewAdd  ('EDImodel', 'EDIreturnMidRequest');
   MODELviewAdd  ('EDImodel', 'EDIupdateWindowGuids');
   MODELviewAdd  ('EDImodel', 'EDIupdateChangedFiles');
   MODELviewAdd  ('EDImodel', 'EDIupdateListWindows');
   MODELviewAdd  ('EDImodel', 'EDIupdateMatchingWindows');
   MODELviewAdd  ('EDImodel', 'EDIupdateNodeWindows');
   MODELviewAdd  ('EDImodel', 'EDIupdateAnnotations');
   MODELviewAdd  ('EDImodel', 'EDIupdateErrorOccured');
   MODELforcePoll('EDImodel');
   MODELstart    ('EDImodel');
}

function EDIfileData(){
   return MODELdata.EDIfileData.data;
}

function EDIinputArgs(){
   EDIclearWindowGuids(EDIlistWindows);
   EDIclearWindowGuids(EDInodeWindows);
   EDImidRequest = true;
   let CurrentFiles = EDIgetFileList();
   let Args = {};
   Args.path        = CurrentFiles;
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function EDIreturnMidRequest(){
   EDImidRequest = false;
}