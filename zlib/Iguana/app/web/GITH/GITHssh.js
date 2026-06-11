let GITHsetSshCard;

function GITHupdateSsh(){
   let Data = GITHgitSshValid();
   let Html = GITHsshValueHtml(Data);
   GITHupdateValue(document.querySelector(".GITHsshModal .GITHtableVal"),  Html);
}

function GITHsshValueHtml(Data){
   let Error = GITHgitSshError();
   if(Error) return GITHinvalidSsh(Error);
   if(Data?.busy)   return GITHbusy(Data?.message);
   if(!Data?.set)   return GITHnotSet();
   // if(!Data?.access) return GITHnoAccessToken(Data?.message || "");
   if(!Data?.valid) return GITHinvalidSsh(Data?.message || "");
   return GITHsshValid();
}

function GITHbusy(Message){
   return /*html*/`<div class="GITHnotSet">${Message}<a class="SETlink">Try again?</a></div>`;
}

function GITHinvalidSsh(Message){
   return /*html*/`<div class="GITHnotSet">${Message}</div>`;
}

function GITHsshValid(){
   return /*html*/`
   <div class="GITHtokenMark">
      <div class="GITHsuccessMark">Successful SSH authentication with the Git account</div>
   </div>`;
}

function GITHsshHtml(){
   let Link = LINKsettingsGitServerSsh; 
   let LinkHtml = Link ? /*html*/`[<a class="SETlink" target="_blank" href="${Link}">...</a>]` : ""; 
   return /*html*/`
   <div class="GITHbox GITHsshModal">
      <div class="GITHtitleRow"> 
         <div class="GITHtitle">Server SSH</div>
         <div class="GITHdesc">Register SSH key with Git Server.
         ${LinkHtml}
         </div>
         <div class="GITHedit BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">SSH Access:</div>
         <div class="GITHtableVal">...</div>
      </div>
   </div>`;
}

function GITHsshEditHtml(FeedbackHtml){
   let Data = GITHgitSshValid();
   let Service = GITHgitProvider();
   let Button = Data?.valid ? `<div class="GITHsuccess GITHsuccessMark">Public key registered</div>` : `<div class="GITHregister BUTTONstandard BUTTONaction">REGISTER KEY</div>`;
   return /*html*/`
   For SSH authentication you must have your public key registered with your Git account: 
   <br>
   <br>
   <div class="GITKbox">
      <div class="GITHgeneratorRow">
         <div class="GITKtableTitle">${Service} SSH:</div>
         ${Button}
      </div>
   </div>
   <div class="GITHfeedback">${FeedbackHtml}</div>
   <div class="GITHerror"></div>
   <div class="BUTTONgroup GITHbutton">
      <div class="GITHdone BUTTONstandard BUTTONdefault">CLOSE</div>
   </div>`;
}

function GITHsetSshPopout(){
   let Azure = GITHgitProvider().includes("Azure Devops");
   let Html =  Azure ? GITHazureRegisterKeyHtml() : GITHsshEditHtml("");
   let Width = Azure ? "800px" : "650px";
   GITHsetSshCard = POPpopoutOpen({
      title:`Set Server SSH`, 
      width : Width,
      content: `<div class="GITHsshEditModal">${Html}</div>`,
      callback : function(){
         CORmodelUpdateSlow();
         MODELforcePoll('GITHslowModel');
      }
   });
   let div = document.querySelector(".GITHsshEditModal");
   if(!div) return;
   div.addEventListener("click", GITHsshListeners);
}

function GITHsshWarning(){
   let Data = GITHgitSshValid();
   return !Data?.busy && (!Data?.set || !Data?.valid);
}

function GITHsshListeners(e){
   if(e.target.closest(".GITHregenerate")) return GITHazureRegenerate();
   if(e.target.closest(".GITHregister")) return GITHsshRegister();
   if(e.target.closest(".GITHdone"))     return POPpopoutClose(GITHsetSshCard);
}

function GITHsshFeedbackHtml(Success, Error){
   if(Success) return `<div class="GITHsuccess">Successfully registered the public key with the Git account.</div>`;
   return `<div class="GITHerror">${Error}</div>`
}

function GITHupdateSetSshModal(FeedbackHtml){
   let div = document.querySelector('.GITHsshEditModal');
   if(!div) return;
   div.innerHTML = GITHsshEditHtml("<br>" + FeedbackHtml);
}

function GITHconvertToLoading(div){
   if(!div) return;
   div.innerHTML = "...";
   div.classList.remove("BUTTONaction")
   div.classList.add("BUTTONdefault");
}

function GITHsshRegister(){
   let div = document.querySelector(".GITHsshEditModal .GITHregister");
   GITHconvertToLoading(div);
   if(div) div.classList.remove("GITHregister");
   APIcall("git/ssh/set", {}, function(R) {
      let FeedbackHtml = GITHsshFeedbackHtml(R?.success, R?.error || "");
      MODELforcePoll("GITHslowModel", function(){
         GITHupdateSetSshModal(FeedbackHtml);
      });
      MODELforcePoll("GITHmodel");
      MODELforcePoll("CORmodel");
      MODELforcePoll("CORmodelSlow");
   });
}