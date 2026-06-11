let DBMtoFile = "";
let DBMtoFileData = {};
let DBMtoFileChanged = null;
let DBMtoFileDataBuffer = {};
let DBMtoFileDataRendered = {};
let DBMfromFile = "";
let DBMfromDatabase = "";
let DBMfromFileData = {};
let DBMfromFileChanged = null;
let DBMfromFileDataBuffer = {};
let DBMfromFileDataRendered = {};
let DBMimportList = {};
let DBMimportMatchingList = [];
let DBMimportNodeListDataRendered = {};
let DBMimportMatchingListRendered = [];

function DBMmodelInit(FileName){
   DBMclearGlobals();
   DBMtoFile = FileName;
   MODELpollerAdd('DBMmodel', 'DBMfileData', 5, 'component/db/load', DBMfileArgs);
   MODELviewAdd(  'DBMmodel','DBMprepareGlobals');
   MODELviewAdd(  'DBMmodel','DBMupdateToFileChanged');
   MODELviewAdd(  'DBMmodel','DBMupdateFromFileChanged');
   MODELviewAdd(  'DBMmodel','DBMupdateToFileName');
   MODELviewAdd(  'DBMmodel','DBMupdateFromFileName');
   MODELviewAdd(  'DBMmodel','DBMupdateToFile');
   MODELviewAdd(  'DBMmodel','DBMupdateFromFile');
   MODELviewAdd(  'DBMmodel','DBMupdateImportList');
   MODELviewAdd(  'DBMmodel','DBMupdateDataBuffers');
   MODELforcePoll('DBMmodel');
   MODELstart    ('DBMmodel');
}

function DBMfileArgs(){
   let Args = {};
   let CurrentFiles = DBMvmdFiles();
   Args.path        = CurrentFiles;
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function DBMfileData(){
   return MODELdata.DBMfileData.data;
}

function DBMclearGlobals(){
   DBMtoFile = "";
   DBMtoFileChanged = null;
   DBMtoFileData = {};
   DBMtoFileDataBuffer = {};
   DBMtoFileDataRendered = {};
   DBMfromFile = "";
   DBMfromDatabase = "";
   DBMfromFileChanged = null;
   DBMfromFileData = {};
   DBMfromFileDataBuffer = {};
   DBMfromFileDataRendered = {};
   DBMimportList = {};
   DBMimportMatchingList = [];
   DBMimportNodeListDataRendered = {};
   DBMimportMatchingListRendered = [];
}
