let EXPdatabaseListRendered = undefined;
let EXPreferenceList = [
   "SQLite",
   "MySQL",
   "MariaDB",
   "ODBC - PostgreSQL",
   "ODBC - MySQL",
   "ODBC - Oracle",
   "ODBC - MS SQL Server",
   "OCI - Oracle"
];

function EXPupdateDatabaseList(){
   let div = document.querySelector(".EXPdatabaseType");
   if(!div) return;
   let Data = EXPdatabaseList();
   if(DEEPequal(EXPdatabaseListRendered, Data)) return;
   let Html = EXPdatabaseListHtml(Data);
   div.innerHTML = Html;
   EXPdatabaseListRendered = Data;
}

function EXPdatabaseOptionHtml(dbType){ return /*html*/`<option ${EXPselected(dbType)}>${dbType}</option>`; }

function EXPselected(Type){
   let Selected = EXPdatabaseTypeSelected;
   return Selected === Type ? "selected" : "";
}

function EXPdatabaseListHtml(Data){
   let Html = "";
   for(Type of Data) Html += EXPdatabaseOptionHtml(Type);
   return Html;
}
