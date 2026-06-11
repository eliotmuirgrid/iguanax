function BLKcriteriaHtml() {
   let Args = BLKmiddleArgs();
   let Range = BLKformatRange(Args.start, Args.end);
   let Search = Args.search ? `<li><b>Search text:</b> ${Args.search}</li>` : "";
   let Tags = Args.tags ? `<li><b>Tag filter:</b> ${Args.tags}</li>` : "";
   let Exact = (Search && Args.exact) ? `<li>Match Whole Word</li>` : "";
   let Name = DETcomponentName(Args.component, BLKcomponentListData());
   let Source = `<li><b>Source Component:</b> ${Name}</li>`;
   if(BLKsingleResubmit()) return Source;
   return /*html*/ `
     ${Source}
     ${Range}
     ${Search}
     ${Tags}
     ${Exact}`;
}

function BLKformatRange(startTime, endTime) {
   let Start = startTime ? LOGtimeFormat(startTime, false, true) : "newest message";
   let End = endTime ? LOGtimeFormat(endTime, false, true) : "oldest message";
   return `
      <li><b>Range start:</b> ${End}</li>
      <li><b>Range end:</b> ${Start}</li>
   `;
}

function BLKrenderCriteria(){
   let Html = BLKcriteriaHtml();
   let Div = document.querySelector(".BLKcriteria");
   if(Div) Div.innerHTML = Html;
}

function BLKrenderSelect(){
   let Html = /*html*/`
   ${BLKresubmitDownstreamList(BLKqueueConsumers())}
   <option disabled>All</option>
   ${BLKcomponentListHtml(BLKcomponentListData(), BLKqueueConsumers())}`;
   let Div = document.querySelector(".BLKselect");
   Div.innerHTML = Html;
}

function BLKresubmitDownstreamList(Data){
   let H = Data.length ? `<option disabled>Downstream</option>` : "";
   let selected = true;
   for (let i = 0; i < Data.length; i++){
      let Guid = Data[i];
      let Perm = BLKpermissionAll()[Guid]?.resubmit_message ? "" : "disabled";
      H += `<option ${selected ? "selected " : ""}value="${Guid}" ${Perm}>${DETcomponentName(Guid, BLKcomponentListData())}</option>`;
      selected = false;
   }
   return H;
}

function BLKcomponentListHtml(List, Downstream){
   let SelectedBefore = Downstream?.length;
   let SelectedGuid = LOGcomponentId();
   let H = "";
   for (let i=0; i < List.length; i++){
      let Option = List[i];
      let Guid = Option.guid;
      let Perm = BLKpermissionAll()[Guid]?.edit_script ? "" : "disabled";
      let Selected = (Guid == SelectedGuid && !SelectedBefore) ? "selected" : "";
      H += `<option ${Selected} ${Perm} value="${Guid}">${Option.name}</option>`;
   }
   return H;
}