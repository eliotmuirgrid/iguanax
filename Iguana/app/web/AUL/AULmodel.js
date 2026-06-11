const AUL_MODEL_NAME = "AULmodel";

function AULmodelInit(){
   MODELremove(AUL_MODEL_NAME);
   MODELpollerAdd(AUL_MODEL_NAME, "AULcomponentNamesData", 2, "component/names", {});
   MODELviewAdd(AUL_MODEL_NAME, "AULrenderDefaults");
   MODELviewAdd(AUL_MODEL_NAME, "AULrenderFormState");
   MODELforcePoll(AUL_MODEL_NAME);
}

function AULnameData(){
   return MODELdata?.AULcomponentNamesData?.data || {};
}
