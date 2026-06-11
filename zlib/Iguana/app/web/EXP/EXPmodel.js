let EXPfile;
let EXPmodelInitFlag;

function EXPmodelInit(){ 
   EXPclearGlobals();
   MODELpollerAdd('EXPmodel', 'EXPdatabaseList', 100, 'database/list', {});
   MODELviewAdd  ('EXPmodel', 'EXPupdateDatabaseList');
   MODELviewAdd  ('EXPmodel', 'EXPupdateFileName');
   MODELviewAdd  ('EXPmodel', 'EXPinitType');
   MODELviewAdd  ('EXPmodel', 'EXPupdateInputBoxes');
   MODELforcePoll('EXPmodel');
}

function EXPinitType(){
   if(EXPmodelInitFlag) return;
   EXPmodelInitFlag = true;
   if(!EXPdatabaseTypeSelected) {
      EXPdatabaseTypeSelected = EXPget(".EXPdatabaseType");
      EXPgenerateSqlClick();
      MODELupdate("EXPmodel");
   }
}

function EXPclearGlobals(){
   EXPmodelInitFlag = false;
   EXPdatabaseListRendered = undefined;
}

function EXPdatabaseList(){
   return MODELdata?.EXPdatabaseList?.data || [];
}
