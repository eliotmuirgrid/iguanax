let RELupstreamListData = [];

function RELgetRelatedUpstream(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.time      = PAGEhashParameter("time");
   Args.id        = PAGEhashParameter("id");
   APIcall("related/upstream", Args, function(R){
      let Data = R?.data || [];
      Data.reverse();
      RELupstreamListData = Data;
      RELrenderUpstreamMessages(Data);
   });
}

function RELnoUpstreamMessagesHtml(){
   return /*html*/`<div class="RELrowBox RELnoRelated">None</div>`;
}

function RELrenderUpstreamMessagesBorder(){
   let Div = document.querySelector('.RELupstreamDiv');
   if(!Div) return;
   Div.classList.remove("RELnoBorder");
}

function RELrenderNoUpstreamMessages(){
   let Div = document.querySelector('.RELupstreamDiv');
   if(!Div) return;
   let html = RELnoUpstreamMessagesHtml();
   if(Div.innerHTML != html) Div.innerHTML = html;
   Div.classList.add("RELnoBorder");
}

function RELrenderUpstreamMessages(Data){
   if(Data.length == 0) return RELrenderNoUpstreamMessages();
   let Div = document.querySelector('.RELupstreamDiv');
   if(!Div) return;
   Div.innerHTML = "";
   let H = ``;
   for (let i = 0; i < Data.length; i++) H += RELupstreamListItemIdx(i);
   Div.insertAdjacentHTML('beforeend', H);
   RELrenderUpstreamMessagesBorder();
}

function RELneedsDateRow(Idx, Data){
   if(Idx == 0) return true;
   let CurrentTime = Data[Idx]?.time   || 0;
   let OldTime     = Data[Idx-1]?.time || 0;
   let CurrentDate = LOGdayFormat(CurrentTime);
   let OldDate     = LOGdayFormat(OldTime);
   return CurrentDate != OldDate;
}

function RELupstreamListItemIdx(Idx){
   let I = RELupstreamListData[Idx];
   let CurrentTime = RELupstreamListData[Idx]?.time || 0;
   const CurrentDate = LOGyearMonthDateTime({
       epoch: CurrentTime,
       includeTime: false,
       simpleTime: false,
       shortDayLabel: false
   });
   let DateRow = RELneedsDateRow(Idx, RELupstreamListData) ? LOGrowDateHtml(CurrentDate) : "";
   let Args = PAGEformatParams(LOGargs(I));
   let href1 = `#logs/details${Args}`;
   let href2 = LOGhasRelated(I) ? `<div class="RELgrey">[<a class="LOGdetails" href="#logs/related${Args}"> Context </a>]</div>` : `<div class="RELcontext"></div>`;
   let href3 = `#logs${Args}`;
   let typeHtml = LOGtypeButton(I.type);
   let Tags = I?.tags || [];
   let tagsHtml = LOGrowTagsHtml(Tags);
   if(!tagsHtml) tagsHtml = "<div></div>";
   return /*html*/ `
   ${DateRow}
   <div class="RELupstreamEntry">
      <div class="RELupstreamBody"> 
         <div class="RELtime LOGcellTime">${LOGjustTimeFormat(CurrentTime)}</div>
         ${typeHtml}
         <div class="RELcellPreview">${LOGcleanContent(I.content)}</div>
         ${tagsHtml}
         ${href2}
         <div class="RELgrey">[<a class="RELdetails" href=${href1}> ... </a>]</div>
         <a class="RELcellPreview RELname" href=${href3}>${RELcomponentName(I.comp)}</a>
      </div>
   </div>`;
}

function RELcomponentName(ComponentId){
   let Data = RELcomponentListData();
   for(let i = 0; i < Data.length; i++){
      let I = Data[i];
      if(I.guid == ComponentId) return I.name;
   }
   return "";
}

function RELcomponentListData(){
   return MODELdata?.RELcomponentList?.data?.component_data || [];
}
