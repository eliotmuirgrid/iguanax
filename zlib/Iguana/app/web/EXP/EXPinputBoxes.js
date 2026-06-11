function EXPupdateInputBoxes(){
   let div = document.querySelector(".EXPinputDivs");
   if(!div) return;
   let Html = /*html*/`<div class="EXPinputDiv">${EXPinputHtml(EXPdatabaseTypeSelected)}</div>`;
   div.innerHTML = Html;
   EXPsetupSqlRegenerateOnChange(EXPdatabaseTypeSelected);
}

function EXPinputHtml(Type){
   if(Type == EXPreferenceList[0]) return EXPlocationHtml();
   if(Type == EXPreferenceList[1]) return EXPpasswordUsernameHtml();
   if(Type == EXPreferenceList[2]) return EXPpasswordUsernameHtml();
   if(Type == EXPreferenceList[3]) return EXPdatabasePasswordUsernameHtml();
   if(Type == EXPreferenceList[4]) return EXPdatabasePasswordUsernameHtml();
   if(Type == EXPreferenceList[5]) return EXPdatabasePasswordUsernameHtml();
   if(Type == EXPreferenceList[6]) return EXPdatabasePasswordUsernameHtml();
   if(Type == EXPreferenceList[7]) return EXPpasswordUsernameHtml();
   return "";
}

function EXPlocationHtml(){
   return /*html*/`
   <div class="EXPheaderFont">FILE:</div>
   <div class="EXPlocationRow">
      <input class="FORMinput EXPlocationIn" value="${ESChtmlEscape(EXPlocation)}">
      <div class="BUTTONcancel BUTTONstandard" title="Select database file">...</div>
   </div>`;
}

function EXPdatabasePasswordUsernameHtml(){
   return /*html*/`
   <div class="EXPheaderFont">DATABASE:</div>
   <input class="FORMinput EXPdatabaseIn" value="${EXPdatabase}">
   ${EXPpasswordUsernameHtml()}`;
}

function EXPpasswordUsernameHtml(){
   return /*html*/`
   <div class="EXPheaderFont">DATASOURCE:</div>
   <input class="FORMinput EXPdatasourceIn" value="${EXPdatasource || ""}">
   <div class="EXPheaderFont">USERNAME:</div>
   <input class="FORMinput EXPusernameIn" value="${EXPusername}">
   <div class="EXPheaderFont">PASSWORD:</div>
   <input class="FORMinput EXPpasswordIn" placeholder="*Optional" type="password">`;
}

function EXPsetupSqlRegenerateOnChange(Type) {
   if(Type == EXPreferenceList[0]) return EXPregenerateOnLocationChange();
   if(Type == EXPreferenceList[1]) return EXPregenerateOnDataSourceChange();
   if(Type == EXPreferenceList[2]) return EXPregenerateOnDataSourceChange();
   if(Type == EXPreferenceList[3]) return EXPregenerateOnDatabaseChange();
   if(Type == EXPreferenceList[4]) return EXPregenerateOnDatabaseChange();
   if(Type == EXPreferenceList[5]) return EXPregenerateOnDatabaseChange();
   if(Type == EXPreferenceList[6]) return EXPregenerateOnDatabaseChange();
   if(Type == EXPreferenceList[7]) return EXPregenerateOnDataSourceChange();
}

function EXPshowRegenerateSqlMessage() {
   let Target = document.querySelector(".EXPgenerateTitle");
   if (!Target) return
   Target.innerHTML = "Don't forget to generate SQL!";
}

function EXPhideRegenerateSqlMessage() {
   let Target = document.querySelector(".EXPgenerateTitle");
   if (!Target) return
   Target.innerHTML = "";
}

function EXPregenerateOnLocationChange() {
   let Location = document.querySelector(".EXPlocationIn");
   Location.addEventListener("change", EXPshowRegenerateSqlMessage);
}

function EXPregenerateOnDatabaseChange() {
   let Database = document.querySelector(".EXPdatabaseIn");
   Database.addEventListener("change", EXPshowRegenerateSqlMessage);
   EXPregenerateOnDataSourceChange();
}

function EXPregenerateOnDataSourceChange() {
   let Source = document.querySelector(".EXPdatasourceIn");
   Source.addEventListener("change", EXPshowRegenerateSqlMessage);
}