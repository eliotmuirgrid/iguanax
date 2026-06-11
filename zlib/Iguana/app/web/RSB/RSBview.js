function RSBresubmitDownstreamList(Data){
   let H = Data.length ? `<option disabled>Downstream</option>` : "";
   let selected = true;
   for (let i = 0; i < Data.length; i++){
      let Guid = Data[i];
      let Perm = RSBpermissionAll()[Guid]?.resubmit_message ? "" : "disabled";
      H += `<option ${selected ? "selected " : ""}value="${Guid}" ${Perm}>${DETcomponentName(Guid, RSBcomponentListData())}</option>`;
      selected = false;
   }
   return H;
}

function RSBrenderSelect(){
   let Html = /*html*/`
   ${RSBresubmitDownstreamList(RSBqueueConsumers())}
   <option disabled>All</option>
   ${RSBcomponentListHtml(RSBcomponentListData(), RSBqueueConsumers())}`;
   let Div = document.querySelector(".RSBselect");
   Div.innerHTML = Html;
}

function RSBcomponentListHtml(List, Downstream){
   let SelectedBefore = Downstream?.length;
   let SelectedGuid = LOGcomponentId();
   let H = "";
   for (let i=0; i < List.length; i++){
      let Option = List[i];
      let Guid = Option.guid;
      let Perm = RSBpermissionAll()[Guid]?.edit_script ? "" : "disabled";
      let Selected = (Guid == SelectedGuid && !SelectedBefore) ? "selected" : "";
      H += `<option ${Selected} ${Perm} value="${Guid}">${Option.name}</option>`;
   }
   return H;
}