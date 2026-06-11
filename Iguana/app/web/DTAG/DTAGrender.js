let DTAGcard;

function DTAGrender(){
	DASHdashboardScreen();
   APIcall("filter_tags/load", {}, function(R){
   	if(document.querySelector('.DTAGmodal')) return;
      if(R?.success) return DTAGrenderPopout(R?.data || []);
      SNCKsnackbar("error", R?.error || "Could not load user filter_tags.json");
      DASHcloseModal();
   });
}

function DTAGrenderPopout(Tags){
   DTAGcard = POPpopoutOpen({
      title: 'Manage Tags',
      width: '800px', 
      minWidth: '400px', 
      content: DTAGhtml(Tags.join(" ")),  
      esc_close : true,
      callback : DASHcloseModal
   });
   DTAGlisteners();
   FORMfocus(document.querySelector(".DTAGinput"));
   FORMenterListener(document.querySelector(".DTAGinput"), DTAGsaveCall);
}

function DTAGhtml(Tags){
   return /*html*/`
   <div class="DTAGmodal">
      <div class="DTAGhelperText">
      Manage tags by separating them with spaces and starting each one with # (no commas).
      <br>
      <br>
      <div class="DTAGrow">
         <div class="DTAGexampleH">Example:</div>
         <input class="DTAGexampleText FORMinput" value="#billing #urgent #bug" readonly>
      </div>
      <br>
      </div>
      <div class="DTAGrow">
         <div class="DTAGlabel FORMlabel">Tags:</div>
         <input class="FORMinput DTAGinput" value="${Tags}" placeholder="#tag1 #tag2">
         <div></div>
         <div class="DTAGerrorFeedback"></div>
      </div>
      <br>
      <br>
      <div class="BUTTONgroup">
         <div class='BUTTONstandard BUTTONcancel'>CANCEL</div>
         <div class='DTAGsave BUTTONstandard BUTTONaction'>SAVE</div>
      </div>
   </div>`;
}
