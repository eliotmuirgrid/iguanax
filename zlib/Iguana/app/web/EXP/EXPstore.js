let EXPdatasource           = undefined;
let EXPdatabaseTypeSelected = undefined;
let EXPlocation             = undefined;
let EXPdatabase             = undefined;
let EXPusername             = undefined;

function EXPget(selector){ 
   let div = document.querySelector(selector);
   if(!div) return undefined;
   return div.value; 
}

function EXPstore(){
   var EXPguid = TRANScomponentGuid();
   if(EXPget(".EXPdatasourceIn") !== undefined) EXPdatasource           = EXPget(".EXPdatasourceIn");
   if(EXPget(".EXPdatabaseType") !== undefined) EXPdatabaseTypeSelected = EXPget(".EXPdatabaseType");
   if(EXPget(".EXPlocationIn")   !== undefined) EXPlocation             = EXPget(".EXPlocationIn");
   if(EXPget(".EXPdatabaseIn")   !== undefined) EXPdatabase             = EXPget(".EXPdatabaseIn");
   if(EXPget(".EXPusernameIn")   !== undefined) EXPusername             = EXPget(".EXPusernameIn");
   STORElocalCreate("EXPdatasource_"           + EXPguid, EXPdatasource);
   STORElocalCreate("EXPdatabaseTypeSelected_" + EXPguid, EXPdatabaseTypeSelected);
   STORElocalCreate("EXPlocation_"             + EXPguid, EXPlocation);
   STORElocalCreate("EXPdatabase_"             + EXPguid, EXPdatabase);
   STORElocalCreate("EXPusername_"             + EXPguid, EXPusername);
}

function EXPread(){
   var EXPguid = TRANScomponentGuid();
   EXPdatabaseTypeSelected = STORElocalRead("EXPdatabaseTypeSelected_" + EXPguid) || "";
   EXPdatasource           = STORElocalRead("EXPdatasource_"           + EXPguid) || "";
   EXPlocation             = STORElocalRead("EXPlocation_"             + EXPguid) || "";
   EXPdatabase             = STORElocalRead("EXPdatabase_"             + EXPguid) || "";
   EXPusername             = STORElocalRead("EXPusername_"             + EXPguid) || "";
}

// This is just used for testing purposes
function EXPclearLocalStorage() {
   var EXPguid = TRANScomponentGuid();
   STORElocalErase("EXPusername_"             + EXPguid);
   STORElocalErase("EXPdatabaseTypeSelected_" + EXPguid);
   STORElocalErase("EXPdatabase_"             + EXPguid);
   STORElocalErase("EXPlocation_"             + EXPguid);
   STORElocalErase("EXPdatasource_"           + EXPguid);
}
