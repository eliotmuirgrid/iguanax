let ENVvariables = {};

function ENVscreen(){
   SETrender();
   document.querySelector(".ENVenvironment").classList.add("selected");
   ENVmodelInit();
   ENVlisteners();   
}

function ENVinit(){
   ENVvariables = ENVenvironmentData();
}

function ENVsort() {
   if(ENVvariables.local)  { Object.fromEntries( Object.entries(ENVvariables.local) .sort((a, b) => a[0].localeCompare(b[0]))); }
   if(ENVvariables.system) { Object.fromEntries( Object.entries(ENVvariables.system).sort((a, b) => a[0].localeCompare(b[0]))); }
}

function ENVresolveValue(key) {
   let value = '';
   if(ENVvariables.system[key]) { value = ENVvariables.system[key]; }
   if(ENVvariables.local[key])  { value = ENVvariables.local[key]; }
   return value;
}

function ENVextractKey(envVar) {
   const start = envVar.indexOf("${") + 2;
   const end   = envVar.indexOf("}", start);
   if(start > 1 && end > start) { return envVar.substring(start, end); }
   return envVar;
}

function ENVexpandEnvVariable(str) {
   if(!str || typeof str !== 'string') { return ''; }
   return str.split(' ').map(function(word) {
      let key = ENVextractKey(word);
      if (key === word) { return word; } // no substring, so not an env variable
      let value = ENVresolveValue(key);
      return value ? ENVexpandEnvVariable(value) : word;
   }).join(' ');
}

function ENVline(K, V) {
   let Editable = ENVcreatePermission() ? `<div class="ENVedit">...</div>` : "";
   return /*html*/`
   <div class="ENVrow">
      <div class="ENVkey">${ESChtml(K)}</div>
      <div class="ENVvalue">${ESChtml(ENVexpandEnvVariable(V))}</div>
      ${Editable}
   </div>`;
}

function ENVcreateButtonHtml(){
   let Create = ENVcreatePermission();
   let Classes =  Create ? "ENVnew ENVcreate BUTTONaction" : "ENVnew BUTTONdisabled";
   let title   = !Create ? `title="You do not have permission to do this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>CREATE</div>`;
}

function ENVlist() {
   const localVariables  = Object.entries(ENVvariables.local).map(([K, V]) => ENVline(K, V)).join('');
   const systemVariables = Object.entries(ENVvariables.system).map(([K, V]) => ENVline(K, V)).join('');
   let Editable = ENVcreatePermission() ? `class="ENVeditable"` : ""; 
   return /*html*/`
      <div ${Editable}>
         ${ENVcreateButtonHtml()}
         <div class="ENVsection">
            <div class="ENVsectionTitle">Local Variables</div>
            ${localVariables}
         </div>
         <div class="ENVseparator"></div>
         <div class="ENVhide BUTTONstandard BUTTONdefault">HIDE</div>
         <div class='ENVsection'>
            <div class="ENVsectionTitle">System Variables</div>
            ${systemVariables}
         </div>   
      </div>`;
}

// TODO should use standard SETtitle across all settings pages.
function ENVpaint(){
   let Target = document.querySelector(".SETcontentBody");
   Target.innerHTML = /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETtitle ENVtitle">Environment</div>
      <div class="SETpageDescription">These are useful to differentiate production/staging and development systems by <a href="${LINKsettingsEnvironmentHeader}" target="_blank">setting different values of these variables</a></div>
      <div class="ENVtable">
          ${ENVlist()}
      </div>
   </div>`;
}
