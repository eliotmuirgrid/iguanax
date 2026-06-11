function TRANSsaveArgs(){
   let Args = PAGEhashParameters();
   let Component = Args?.component || "";
   let Data = {};
   Data.component   = Component;
   let development =  Args?.development || "";
   Data.development = development;
   Data.edit        = Args?.edit        || "";
   Data.sample      = Args?.sample      || "";
   let SaveData = STOREreadJsonVariable("TRANSpreviousArgs");
   let Key = development == "true" ? "true" : "false";
   SaveData[Component + Key] = Data;
   STOREsaveJsonVariable("TRANSpreviousArgs", SaveData);
   let DevData = STOREreadJsonVariable("TRANSpreviousDev");
   DevData[Component] = development;
   STOREsaveJsonVariable("TRANSpreviousDev", DevData);
}


function TRANSformatUrl(Component = "", Dev = ""){
   let Url = "#translator";
   let StoredData = STOREreadJsonVariable("TRANSpreviousArgs");
   let devstring = Dev == true ? "true" : "false";
   let Data = StoredData[Component + devstring] || {};
   Data.component = Component;
   if(Dev != "none") Data.development = Dev;
   if(!Component) Data.development = "";
   return Url + PAGEformatParams(Data);
}

function TRANScontrolElement(Selector, Hide){
   let div = document.querySelector(Selector);
   if(!div) return;
   if( Hide) div.classList.add   ("TRANShide");
   if(!Hide) div.classList.remove("TRANShide");
}

function TRANScontrolTranslatorFeatures(Hide){
   TRANScontrolElement(".TRANStoolbarWrapper", Hide);
   TRANScontrolElement(".PRJtabsAndButtons",   Hide);
   TRANScontrolElement(".TRANSstatus_bar",     Hide);
   TRANScontrolElement(".PRJswitchDiv",        Hide);
   TRANScontrolElement(".PRJdrawers",          Hide);
   TRANScontrolElement(".PRJsources-receivers", Hide);
   TRANScontrolElement(".PRJprojectTreeBody",  Hide);
}

function TRANShandleEmptyComponentGuid(){
   TRANSsetAllListeners();
   TRANSemptyGuid = true;
   document.querySelector('.CodeMirror')?.remove();
   TRANSmodelInit();
}
