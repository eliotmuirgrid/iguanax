let RPOmidOnSubmitRequest;

function RPOclearGlobals(){
   RPOslugError = false;
   RPOworkspaceError = false;
   RPOgitUrlError = false;
   RPOmidOnSubmitRequest = false;
}

function RPOmodelInit(){
   RPOclearGlobals();
   MODELpollerAdd('RPOmodel', 'RPOlistData',            23, 'component/list',      {});
   MODELpollerAdd('RPOmodel', 'RPOrunningComponents',    3, 'component/run_list',  {});
   MODELpollerAdd('RPOmodel', 'RPOpermissionData',      10, 'component/permission', RPOargs);
   MODELpollerAdd('RPOmodel', 'RPOownersData',  20, 'git/owners', {});
   MODELpollerAdd('RPOmodel', 'RPOserviceData', 21, 'git/server', {});
   MODELviewAdd  ('RPOmodel', 'RPOnoValidWorkspaceCache');
   MODELviewAdd  ('RPOmodel', 'RPOupdateHost');
   MODELviewAdd  ('RPOmodel', 'RPOupdateSlugExists');
   MODELviewAdd  ('RPOmodel', 'RPOupdateSlug');
   MODELviewAdd  ('RPOmodel', 'RPOupdateGitUrl');
   MODELviewAdd  ('RPOmodel', 'RPOupdateGitUrlError');
   MODELviewAdd  ('RPOmodel', 'RPOupdateButton');
   MODELviewAdd  ('RPOmodel', 'RPOupdateCustomButton');
   MODELforcePoll('RPOmodel', function(){
      MODELpollerAdd('RPOmodel', 'RPOslugData', 2, 'git/check_repo_path', RPOslugArgs, RPOmidRequest); 
      MODELstart('RPOmodel');
      let Div = document.querySelector(".RPOmodal");
      if(!Div) return;  
      Div.innerHTML = RPOhtml();
      let Workspaces = document.querySelector('.RPOworkspaces');
      GITutilsUpdateWorkspaces(Workspaces, 'RPOselectedWorkspace', RPOownersData()?.owners);
      if(RPOpreCondition) return RPOpreCondition(RPOpreConditionSuccess, RPOpreConditionFail);
      RPOpreConditionSuccess();
   });
}
function RPOdisableButton(div){
   if(!div) return;
   div.classList.add("BUTTONdisabled");
   div.title = "You do not have permission to do this.";
}

function RPOpreConditionFail(){
   MODELremove("RPOmodel");
   let CustomBtn = document.querySelector('.RPOconfirmCustom');
   let Btn = document.querySelector('.RPOconfirm');
   RPOdisableButton(CustomBtn);
   RPOdisableButton(Btn);
}

function RPOpreConditionSuccess(){
   let div = document.querySelector(".RPOslug");
   let custom = document.querySelector(".RPOgitUrlCustom");
   let focusDiv = RPOdefaultCustomValue ? custom : div;
   FORMfocus(focusDiv);
   FORMenterListener(div, RPOconfirmSubmit);
   FORMenterListener(custom, RPOconfirmCustomSubmit);
   RPOslowModelInit();
}

function RPOvalidWorkspaceCache(){ 
   let Data = RPOownersData();
   return Data?.exists && Data?.owners.length != 0; 
}

function RPOownersData(){ return MODELdata?.RPOownersData?.data; }

function RPOgitData(){ return MODELdata?.RPOgitData?.data; }

function RPOslugArgs() { 
   let ApiData = {};
   ApiData.slug = RPOslugValue();
   ApiData.owner = RPOworkspaceValue();
   return ApiData;
}

function RPOslugValue(){
   let Div = document.querySelector('.RPOslug');
   return Div ? Div.value : ""; 
}

function RPOworkspaceValue(){
   let Div = document.querySelector('.RPOworkspaces');
   return Div ? Div.value : ""; 
}

function RPOhostData(){
   return MODELdata?.RPOserviceData?.data?.address || "";
}

function RPOsshUrl(){
   let Data = MODELdata?.RPOslugData?.data || {};
   return Data?.urls?.ssh || "";
}

function RPOwebUrl(){
   let Data = MODELdata?.RPOslugData?.data || {};
   return Data?.urls?.web || "";
}

function RPOlistData(){ 
   return MODELdata?.RPOlistData?.data?.component_data || []; 
}

function RPOcomponentRunning(){
   let RunningData = MODELdata?.RPOrunningComponents?.data || [];
   return RunningData[RPOcomponent()] ? true : false; 
}

function RPOargs(){
   let Args = {};
   Args.component = RPOcomponent();
   return Args;
}

function RPOpermissionData(){
   return MODELdata?.RPOpermissionData?.data;
}
function RPOmidRequest(){
   return !RPOmidOnSubmitRequest;
}