let PRJcomponentListRendered = undefined;

function PRJhasPermission(guid){
   let key = TRANSdevelopment() ? "edit_script" : "view_script";
   let data = TRANSpermissionAll()[guid];
   if(!data) return false;
   let Perm  = data[key];
   return Perm;
}

function PRJcomponentListHtml(List, NoSelected = false){
   let SelectedGuid = TRANScomponentGuid();
   let html         = "";
   let DevNow       = TRANSdevelopment(); // current mode

   const generateSection = (filtered, title, devModeForSection, selectHeader=false) => {
      if (filtered.length === 0) return '';
      let section = `<option disabled ${selectHeader ? 'selected' : ''}>--- ${title} ---</option>`;
      filtered.forEach(Option => {
         let disabled = PRJhasPermission(Option.guid) ? "" : "disabled";

         // Only select the copy that matches current dev mode
         let selected = "";
         if (!NoSelected && Option.guid === SelectedGuid && DevNow === devModeForSection){
            selected = "selected";
         }

         section += `<option ${selected}
                            value="${Option.guid}"
                            data-development="${devModeForSection ? "true" : "false"}"
                            ${disabled}>${ESChtmlEscape(Option.name)}</option>`;
      });
      return section;
   };

   // Independent criteria (can overlap)
   const EditableList = List.filter(o => o.template === false);
   const ViewableList = List.filter(o => o.commit !== '');

   html += generateSection(EditableList, 'Editable', true,  true);
   html += generateSection(ViewableList, 'Viewable', false, html === '');
   return html;
}


function PRJcomponentListUpdate(){
   let ComponentList = PRJcomponentListData();
   if(DEEPequal(PRJcomponentListRendered, ComponentList)) return;
   let H = PRJcomponentListHtml(ComponentList);
   let Div = document.querySelector('.PRJcomponentSelect');
   if(!Div) return;
   if(Div.innerHTML != H) Div.innerHTML = H;
   PRJcomponentListRendered = ComponentList;
}

function PRJeditableList(){
   let Data = PRJcomponentListData();
   let List = [];
   for(i of Data) if(!i.template) List.push(i);
   return List;
}
function PRJcomponentListData(){
   return MODELdata?.TRANScomponentList?.data?.component_data || [];
}

function PRJcomponentSelectChange(){
   let C = document.querySelector(".PRJcomponentSelect");
   if(!C) return;

   let Value = C.value;

   let opt = C.options[C.selectedIndex];
   let devMode = opt?.dataset?.development === "true";

   window.location.hash = TRANSformatUrl(Value, devMode);
   MODELforcePoll('CORmodel');
}