let RELselectedIndex = undefined;
let RELclearedOldList = false;

function RELrelatedListAppend(Data){
   let Div = document.querySelector('.RELlistDiv');
   if(!Div) return;
   let H = ``;
   RELrelatedListData = RELrelatedListData.concat(Data);
   let StartIdx = RELrelatedListData.length - Data.length;
   for (let i = StartIdx; i < RELrelatedListData.length; i++) H += RELlistItemIdx(i);
   Div.insertAdjacentHTML('beforeend', H);
   if(RELrelatedListData.length) RELrenderRelatedMessagesBorder();
   else                          RELrenderNoRelatedMessagesBorder();
   if(RELselectedIndex !== undefined) RELscrollToSelected();
}

function RELscrollToSelected(){
   let Logs = document.querySelectorAll(".RELlogEntry");
   if(!Logs) return;
   let Log = Logs[RELselectedIndex];
   if(!Log) return;
   Log.scrollIntoView({behavior : "smooth", block: "center"});
}

function RELrenderNoRelatedMessages(){
   let Div = document.querySelector('.RELlistDiv');
   if(!Div) return;
   let html = RELnoRelatedMessagesHtml();
   if(Div.innerHTML != html) Div.innerHTML = html;
   RELrenderNoRelatedMessagesBorder();
}

function RELrenderNoRelatedMessagesBorder(){
   let Div = document.querySelector('.RELlistDiv');
   if(!Div) return;
   Div.classList.add("RELnoBorder");
}

function RELrenderRelatedMessagesBorder(){
   let Div = document.querySelector('.RELlistDiv');
   if(!Div) return;
   Div.classList.remove("RELnoBorder");
}

function RELnoRelatedMessagesHtml(){
   return /*html*/`<div class="RELrowBox RELnoRelated">None</div>`;
}
function RELentrySelected(Idx){
   let I = RELrelatedListData[Idx];
   let Component = PAGEhashParameter("component");
   let Time = PAGEhashParameter("time");
   let Id = PAGEhashParameter("id");
   let Selected = Component == I?.comp && Time == I?.time && Id == I?.id;
   return Selected;
}

function RELlistItemIdx(Idx){
   let I = RELrelatedListData[Idx];
   let CurrentTime = RELrelatedListData[Idx]?.time || 0;
   const CurrentDate = LOGyearMonthDateTime({
      epoch: CurrentTime,
      includeTime: false,
      simpleTime: false,
      shortDayLabel: false
  });
   let DateRow = RELneedsDateRow(Idx, RELrelatedListData) ? LOGrowDateHtml(CurrentDate) : "";
   let Selected = RELentrySelected(Idx);
   let Args = PAGEformatParams(LOGargs(I));
   let href1 = `#logs/details${Args}`;
   let href2 = LOGhasRelated(I) ? `<div class="RELgrey">[<a class="LOGdetails" href="#logs/related${Args}"> Context </a>]</div>` : "<div></div>";
   let href3 = `#logs${Args}`;
   let typeHtml = LOGtypeButton(I.type);
   let Tags = I?.tags || [];
   let tagsHtml = LOGrowTagsHtml(Tags);
   if(!tagsHtml) tagsHtml = "<div></div>";
   if(Selected == true) RELselectedIndex = Idx;
   return /*html*/ `
   ${DateRow}
   <div class="RELlogEntry  ${Selected ? "RELentrySelected" : ""}">
      <div class="RELtime LOGcellTime">${LOGjustTimeFormat(CurrentTime)}</div>
      ${typeHtml}
      <div class="RELcellPreview">${LOGcleanContent(I.content)}</div>
      ${tagsHtml}
      ${href2}
      <div class="RELgrey">[<a class="RELdetails" href=${href1}> ... </a>]</div>
      <a class="RELcellPreview RELname" href=${href3}>${RELcomponentName(I.comp)}</a>
   </div>`;
}