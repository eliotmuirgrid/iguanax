function GINupdateStatus(){
   let div = document.querySelector(".GINstatus");
   if(!div) return;
   let html = GINstatusHtml();
   if(div.innerHTML != html) div.innerHTML = html;
}

function GINstatusHtml(){
   let LP = GINlastPushAttempted();
   let SP = GINlastPushSuccessful();
   let EP = GINlastPushError();
   let Error = LP > SP;
   let html;
   if(SP == 0 && LP == 0) html = "";
   else if(!Error) html = `Successfully pushed ${LOGtimeFormatShort(SP)} ago.`;
   else           html = `Tried to push ${LOGtimeFormatShort(LP)} ago.<br><br> <div class="GINerror">${EP}</div>`;
   return /*html*/`<div class="GINstatusDiv">${html}</div>`;
}

function GINupdateWarning(){
   let div = document.querySelector(".GINwarning");
   if(!div) return;
   let html = GINwarningHtml();
   if(div.innerHTML != html) div.innerHTML = html;
}

function GINwarningHtml(){
   let Warning = GINpushWarningError();
   if(!Warning) return "";
   let Dismissed = STORElocalRead("GINpushWarningDismissed");
   if(Warning == Dismissed) return "";
   return /*html*/`
   <div class="GINwarningDiv">
      <div class="GINdismissGrid">
         <div class="GINwarningTitle FORMlabel">Push Warning:</div>
         <div class="GINbracket">[</div>
         <div class="GINwarningDismiss">Dismiss Warning</div>
         <div class="GINbracket">]</div>
      </div>
      <br> 
      <div class="GINwarningMessage">${Warning}</div>
   </div>
   <br> 
   `;
}