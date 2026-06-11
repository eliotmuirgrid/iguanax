let PINScard;
let PINSlist;

function PINSrender(){
	DASHdashboardScreen();
   APIcall("component/list", {}, function(R){
      if(!R?.success) {
         SNCKsnackbar("error", R?.error || "Could not load component list.");
         return PINSclose();
      }
      PINSlist = R?.data?.component_data || [];
      APIcall("pins/load", {}, function(R){
         if(document.querySelector('.PINSmodal')) return;
         if(R?.success) return PINSrenderPopout(R?.data || {});
         SNCKsnackbar("error", R?.error || "Could not load user pins.json");
         PINSclose();
      });
   });
}

function PINSrenderPopout(Pins){
   PINScard = POPpopoutOpen({
      title: 'Edit Exact Coordinates',
      width: '800px', 
      maxHeight: '800px',
      minWidth: '400px', 
      content: PINShtml(Pins),  
      esc_close : true,
      callback : PINSclose
   });
   PINSlisteners();
   FORMfocus(document.querySelector(".PINSguid"));
   FORMenterListener(document.querySelector(".PINStable"), PINSsaveCall);
}

function PINSdescriptionHtml(){
   return /*html*/`
   <div class="PINShelperText">
      Edit exact X/Y positions for map components. This is an advanced manual layout tool.
      [ <a target="_blank" href=${LINKsetPins}>Learn more</a> ]
   </div>`;
}

function PINSheaderHtml(){
   return /*html*/`
   <div class="PINSheader">
      <div>Component</div>
      <div>Position X</div>
      <div>Position Y</div>
      <div></div>
   </div>`;
}

function PINSrowHtml(Guid, Data){
   return /*html*/`
   <div class="PINSrow">
      <input class="FORMinput PINSguid" value="${Guid || ""}" list="PINScomponentList">
      <input class="FORMinput PINSpx" value="${(Data?.px || 0)}" type="number">
      <input class="FORMinput PINSpy" value="${(Data?.py || 0)}" type="number">
      <div class="PINSdeletetableCell">
         <div class="BUTTONcancel PINSdelete"><img src="/DASH/DASHclearSearch.svg"></div>
      </div>
   </div>`;
}

function PINStableHtml(Pins){
   let Html = "";
   for(Pin in Pins) Html = Html + PINSrowHtml(Pin, Pins[Pin]);
   if(Html == "") Html = PINSrowHtml("", {});
   return Html;
}

function PINShtml(Pins){
   return /*html*/`
   <div class="PINSmodal">
   ${PINSdescriptionHtml()}
   <div class="PINScontainer">
      <div class="PINSheaderTitle">Exact Coordinates</div>
      ${PINSheaderHtml()}
      <div class="PINStableContainer">
         <div class="PINStable">
            ${PINStableHtml(Pins)}
         </div>
      </div>
      ${PINScomponentListHtml()}
      <div class="PINSrowsFooter">
         <div class="PINSaddRow">+ Add Row</div>
         <div class="PINSclearAllContainer">
            <div>[</div>
            <div class="PINSclearAll">Clear All Pins</div>
            <div>]</div>
         </div>
      </div>
   </div>
   <div class="PINSbottomRow">
      <div class="PINSerrorFeedback"></div>
      <div class="PINSsaveConfigButton BUTTONgroup">
         <div class='BUTTONstandard BUTTONcancel'>Cancel</div>
         <div class='PINSsave BUTTONstandard BUTTONaction'>Save</div>
      </div>
   </div>
   </div>`;
}

function PINScomponentOptions(Components){
   let html = "";
   for(Component of Components) html = html + /*html*/`<option value="${Component?.guid}">${Component?.name}</option>`;
   return html;
}

function PINScomponentListHtml(){
   return /*html*/`
   <datalist id="PINScomponentList">
      ${PINScomponentOptions(PINSlist)}
   </datalist>`;
}

function PINSclose() {
   POPpopoutClose(PINScard);
   DASHcloseModal();
}