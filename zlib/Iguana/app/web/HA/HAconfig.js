function HAconfigHtml(Data, Edit) {
   const enabled   = Data?.enabled || false;
   const sharedDir = Data?.shared_working_directory || '';
   const type      = Data?.type || 'Active-Passive Shared Drive';
   return /*html*/`
   <div class="HAbox">
      <div class="FORMlabel">Enabled:</div>
      <div>${HAenabledSelectHtml(enabled, Edit)}</div>
      <div class="FORMlabel">Shared Working Directory:</div>
      <div>${HAsharedDirInputHtml(sharedDir, Edit)}</div>
      <div class="FORMlabel">Type:</div>
      <div>${HAtypeSelectHtml(type, Edit)}</div>
   </div>`;
}

function HAenabledSelectHtml(Enabled, Edit) {
   const displayText = Enabled ? 'Enabled' : 'Disabled';
   if(!Edit) { return `<div class="HAenabledDisplay">${displayText}</div>`; }
   const enabledSelected  = Enabled ? ' selected' : '';
   const disabledSelected = !Enabled ? ' selected' : '';
   return /*html*/`
   <select class="FORMselectBox HAenabledSelect">
      <option value="true"${enabledSelected}>Enabled</option>
      <option value="false"${disabledSelected}>Disabled</option>
   </select>`;
}

function HAsharedDirInputHtml(SharedDir, Edit) {
   if(!Edit) { return `<div class="HAsharedDirDisplay">${SharedDir || '(not set)'}</div>`; }
   return /*html*/`
   <div class="FORMinputGroup HAsharedDirGroup">
      <input class="FORMinput HAsharedDirInput" value="${SharedDir}">
      <div class="HAsharedDirBrowse BUTTONstandard BUTTONcancel">...</div>
   </div>`;
}

function HAtypeSelectHtml(Type, Edit) {
   if(!Edit) { return `<div class="HAtypeDisplay">${Type}</div>`; }
   const select = (type, value) => { return type === value ? 'selected' : ''; }; // use when we have more than one option for selection
   return /*html*/`
   <select class="FORMselectBox HAtypeSelect">
      <option value="Active-Passive Shared Drive" selected>Active-Passive Shared Drive</option>
   </select>`;
}

function HAupdateConfig(Data) {
   let div = document.querySelector(".HAtarget");
   if(!div) { return; }
   let Config;
   if(Data) {
      Config       = Data;
      HAconfigData = undefined;
   } else {
      Config = HAdata();
   }
   let Edit = HAeditPage();
   let Html = HAconfigHtml(Config, Edit);
   if(!DEEPequal(HAconfigData, Config) || HAlastEditState !== Edit) {
      div.innerHTML   = Html;
      HAlastEditState = Edit;
   }
   HAconfigData = Config;
   HAinputSetters();
}

function HAinputSetters() {
   let enableSelect = document.querySelector(".HAenabledSelect");
   if(enableSelect) {
      FORMfocus(enableSelect);
      FORMenterListener(enableSelect, HAsave);
   }

   let inp = document.querySelector(".HAsharedDirInput");
   if(inp) { FORMenterListener(inp, HAsave); }

   let typeSelect = document.querySelector(".HAtypeSelect");
   if(typeSelect) {
      FORMfocus(typeSelect);
      FORMenterListener(typeSelect, HAsave);
   }
}
