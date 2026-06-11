let IMPtoFile = "";
let IMPtoFileChanged = null;
let IMPtoFileData = {};
let IMPtoFileDataBuffer = {};
let IMPtoFileNodeDataRendered = {};
let IMPtoFileMatchingDataRendered = [];

let IMPfromFile = "";
let IMPfromLibrary = "";
let IMPfromFileChanged = null;
let IMPfromFileData = {};
let IMPfromFileDataBuffer = {};
let IMPfromFileNodeDataRendered = {};
let IMPfromFileMatchingDataRendered = [];

let IMPimportNodeList = {};
let IMPimportMatchingList = [];
let IMPimportNodeListDataRendered = {};
let IMPimportMatchingListRendered = [];

function IMPmodelInit(FileName){
   IMPresetGlobals();
   IMPtoFile = FileName;
   IMPfromLibrary = "HL7 2.7";
   MODELpollerAdd('IMPmodel', 'IMPfileData', 5, 'component/edi/load', IMPfileArgs);
   MODELviewAdd  ('IMPmodel', 'IMPupdateGlobals');
   MODELviewAdd  ('IMPmodel', 'IMPupdateToFileChanged');
   MODELviewAdd  ('IMPmodel', 'IMPupdateFromFileChanged');
   MODELviewAdd  ('IMPmodel', 'IMPupdateDataBuffers');
   MODELviewAdd  ('IMPmodel', 'IMPupdateToFileName');
   MODELviewAdd  ('IMPmodel', 'IMPupdateFromFileName');
   MODELviewAdd  ('IMPmodel', 'IMPupdateToFile');
   MODELviewAdd  ('IMPmodel', 'IMPupdateFromFile');
   MODELviewAdd  ('IMPmodel', 'IMPupdateImportList');
   MODELforcePoll('IMPmodel');
   MODELstart    ('IMPmodel');
}

function IMPfileArgs(){
   let Args = {};
   let CurrentFiles = IMPvmdFiles();
   Args.path        = CurrentFiles;
   Args.component   = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function IMPfileData(){
   return MODELdata.IMPfileData.data;
}

function IMPresetGlobals(){
   IMPtoFile = "";
   IMPtoFileChanged = null;
   IMPtoFileData = {};
   IMPtoFileDataBuffer = {};
   IMPtoFileNodeDataRendered = {};
   IMPtoFileMatchingDataRendered = [];
   
   IMPfromFile = "";
   IMPfromLibrary = "";
   IMPfromFileChanged = null;
   IMPfromFileData = {};
   IMPfromFileDataBuffer = {};
   IMPfromFileNodeDataRendered = {};
   IMPfromFileMatchingDataRendered = [];
   
   IMPimportNodeList = {};
   IMPimportMatchingList = [];
   IMPimportNodeListDataRendered = {};
   IMPimportMatchingListRendered = [];
}