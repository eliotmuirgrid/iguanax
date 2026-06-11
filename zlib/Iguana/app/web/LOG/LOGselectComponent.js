let LOGcomponentListRendered = undefined;

function LOGcomponentListHtml(List, IncludeServer, NoSelected = false){
   let SelectedGuid = LOGcomponentId();
   let H = IncludeServer ? `<option value="" ${!SelectedGuid ? "selected" : ""}>Iguana Service</option>` : "";
   for (let i=0; i < List.length; i++){
      let Option = List[i];
      let Guid = Option.guid;
      let Perm = LOGpermissionAll()[Guid]?.view_logs ? "" : "disabled";
      let Selected = !NoSelected && (Guid == SelectedGuid) ? "selected" : "";
      H += `<option ${Selected} ${Perm} value="${Guid}">${Option.name}</option>`;
   }
   return H;
}

function LOGcomponentListUpdate(){
   let ComponentList = LOGcomponentListData();
   if(ComponentList === undefined) { return'' }
   if(DEEPequal(LOGcomponentListRendered, ComponentList)) return;
   let H = LOGcomponentListHtml(ComponentList, true);
   let Div = document.querySelector('.LOGcomponentSelect');
   if(!Div) return;
   if(Div.innerHTML != H) { Div.innerHTML = H; }
   LOGcomponentListRendered = ComponentList;
}