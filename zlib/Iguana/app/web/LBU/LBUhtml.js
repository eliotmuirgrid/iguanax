function LBUhtml(){
   return LBUtableHtml(LBUdata(), "Libraries in this component:");
}

function LBUheaderHtml(){
   return /*html*/`
   <div class="LBUheading">
      <div>Name</div>
      <div class="LBUahead">Customized</div>
      <div class="LBUbehind">Outdated</div>
      <div class="LBUusedIn">Used in</div>
   </div>`;
}

function LBUrowHtml(DataRow, Key, Settings){
   let Name = LBUname(DataRow?.name, Key);
   let Link = DataRow?.urls?.web || "";
   let Url = DataRow?.urls?.url || "";
   let Ssh = DataRow?.urls?.ssh || "";
   let Usage = DataRow?.usage;
   let Desc = HASHTAGrenderHashtags(DataRow?.description) || ``;
   let href1 = "";
   let href2 = "";
   let href3 = "";
   if(Settings){
      href1 = `href="#settings/libraries?url=${Ssh}&focus=ahead" `;
      href2 = `href="#settings/libraries?url=${Ssh}&focus=behind" `;
      href3 = `href="#settings/libraries?url=${Ssh}&focus=usage" `;
   }
   return /*html*/`
   <div class="LBUrow">
      <div class="LBUnameDiv">
         <a class="LBUname" href="${Link}" target="_blank" title="${Url}">${Name}</a>
         <div class="LBUdesc">${Desc}</div>
      </div>
      <div class="LBUdata LBUaheadHref" >${LBUformat(LBUcount(Usage, "ahead"), true, href1)}</div>
      <div class="LBUdata LBUbehindHref">${LBUformat(LBUcount(Usage, "behind"), true, href2)}</div>
      <div class="LBUdata LBUusageHref" >${LBUformat(Usage?.length || 0, false, href3)}</div>
   </div>`;
}

function LBUtableHtml(Data, Header, Settings){
   let Html = "";
   let HeaderHtml = "";
   if(Header) HeaderHtml = /*html*/`<div class="LBUheaderDesc">${Header}</div><br>`;
   for(Key in Data){
      let RowData = Data[Key];
      Html = Html + LBUrowHtml(RowData, Key, Settings);
   }
   return /*html*/`
   ${HeaderHtml}
   <div class="LBUtable LBUtableTop">${LBUheaderHtml()}</div>
   <div class="LBUtable LBUtableBottom">${Html}</div>
   <br>`;
}