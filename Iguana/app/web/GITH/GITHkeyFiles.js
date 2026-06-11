let GITHsetKeysCard;

function GITHupdateKeys(){
   let Data = GITHkeyData();
   let PrivHtml = GITHpathHtml(Data?.private_key || "", Data?.private_key_exists || false);
   let PubHtml  = GITHpathHtml(Data?.public_key  || "", Data?.public_key_exists  || false);
   GITHupdateValue(document.querySelector(".GITHprivateKey"), PrivHtml);
   GITHupdateValue(document.querySelector(".GITHpublicKey"),  PubHtml);
}

function GITHpathHtml(Path, Exists){
   if(Exists) return /*html*/`<div class="GITHeditKeyPath">${Path}</div>`;
   return GITHdoesNotExistHtml(Path);
}

function GITHdoesNotExistHtml(Path){
   return /*html*/`<div class="GITHnotSet">Does not exist: ${Path}</div>`;
}

function GITHkeyFilesHtml(){
   let Link = ""; //GITHserviceHelpLinks(Service);
   let LinkHtml = Link ? /*html*/`[<a class="SETlink" target="_blank" href="${Link}">...</a>]` : ""; 
   return /*html*/`
   <div class="GITHbox GITHkeyFiles">
      <div class="GITHtitleRow"> 
         <div class="GITHtitle">Key Pair SSH</div>
         <div class="GITHdesc">Store key pair for Git Server SSH authentication.
         ${LinkHtml}
         </div>
         <div class="GITHedit BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">Private Key Path:</div>
         <div class="GITHtableVal GITHprivateKey">...</div>
         <div class="GITHtableTitle"> Public Key Path:</div>
         <div class="GITHtableVal GITHpublicKey">...</div>
      </div>
   </div>`;
}

function GITHkeyFilesEditHtml(FeedbackHtml){
   let Data = GITHkeyData();
   let PrivHtml = GITHpathHtml(Data?.private_key || "", Data?.private_key_exists || false);
   let PubHtml  = GITHpathHtml(Data?.public_key  || "", Data?.public_key_exists  || false);
   let Action   = GITHkeyWarning() ? "GENERATE" : "REGENERATE";
   let BaseKeyType = (GITHgitProvider().includes("Azure Devops")) ? " rsa-sha2-256" : "n ed25519";
   let Override = GITHkeyOverride();
   let KeyTypeText = Override.length > 0 ? "IFW_SSH_GEN_TYPE detected. Iguana will generate a " + Override + " type key pair." :
                                       "Iguana will generate a" + BaseKeyType + " type key pair."
   // check here if we have ssh-keygen
   return /*html*/`
   Iguana needs the public and private keys to be stored in a specific file location:
   <br>
   <br>
   <div class="GITKbox">
      <div class="GITKtableEdit">
         <div class="GITKtableTitle">Private Key:</div>
         <div class="GITKpath">${PrivHtml}</div>
         <div class="GITKresetPath BUTTONstandard BUTTONaction">RESET</div>
         <div class="GITKtableTitle">Public Key:</div>
         <div class="GITKpath">${PubHtml}</div>
      </div>
      <div class="GITHfeedback">${FeedbackHtml}</div>
   </div>
   <br>
   Generate or regenerate your public and private key pair:
   <br>
   <br>
   <div class="GITKbox">
      <div class="GITHgeneratorRow">
         <div class="GITKtableTitle">SSH-Keygen Tool:</div>
         <div class="GITHgenerate BUTTONstandard BUTTONaction">${Action}</div>
      </div>
      <div>${KeyTypeText}</div>
   </div>
   <div class="GITHerror"></div>
   <br>
   <div class="BUTTONgroup GITHbutton">
      <div class="GITHdone BUTTONstandard BUTTONdefault">CLOSE</div>
   </div>`;
}

function GITHsetKeyFilesPopout(){
   GITHsetKeysCard = POPpopoutOpen({
      title:`Set Key Pair SSH`, 
      minWidth: "850px",
      content: `<div class="GITHkeyEditModal">${GITHkeyFilesEditHtml("")}</div>`
   });
   let div = document.querySelector(".GITHkeyEditModal");
   if(!div) return;
   div.addEventListener("click", GITHkeyListeners);
}

function GITHkeyListeners(e){
   if(e.target.closest(".GITHdone"))      return POPpopoutClose(GITHsetKeysCard);
   if(e.target.closest(".GITHgenerate"))  return GITHgenerateKeys();
   if(e.target.closest(".GITKresetPath")) return GITHresetKeyPath();
}

function GITHgenerateKeys(){
   let div = document.querySelector(".GITHfeedback");
   if(div) div.innerHTML = "";
   let Args = {};
   if(GITHgitProvider().includes("Azure Devops")) Args.key_type = "rsa-sha2-256";
   APIcall('git/generate_default_key_pair', Args, function(R) {
      let FeedbackHtml = GITHfeedbackHtml(R?.success, "Successfully generated a new private and public key pair.", R?.error || "");
      if(R.success) { 
         let Message = GINgenerateKeyPair();
         GINaddAndCommit({message: Message, users: true}); 
      }
      MODELforcePoll("GITHmodel", function(){
         GITHupdateSetKeysModal(FeedbackHtml);
      });
      MODELforcePoll("GITHslowModel");
      MODELforcePoll("CORmodel");
      MODELforcePoll("CORmodelSlow");
   });
}

function GITHfeedbackHtml(Success, SuccessMessage, Error){
   if(Success) return `<div class="GITHsuccess">${SuccessMessage}</div>`;
   return `<div class="GITHerror">${Error}</div>`
}

function GITHupdateSetKeysModal(FeedbackHtml){
   let div = document.querySelector('.GITHkeyEditModal');
   if(!div) return;
   div.innerHTML = GITHkeyFilesEditHtml("<br>" + FeedbackHtml);
}

function GITHkeyWarning(){
   let Data = GITHkeyData();
   return !Data?.private_key_exists || !Data?.public_key_exists;
}

function GITHresetKeyPath(){
   APIcall('git/reset_key_path', {}, function(R) {
      let FeedbackHtml = GITHfeedbackHtml(R?.success, "Successfully reset the file location for the public and private key.", R?.error || "");
      if(R.success) { 
         let Message = GINresetKeyPath();
         GINaddAndCommit({message: Message, users: true}); 
      }
      MODELforcePoll("GITHmodel", function(){
         GITHupdateSetKeysModal(FeedbackHtml);
      });
      MODELforcePoll("GITHslowModel");
      MODELforcePoll("CORmodel");
      MODELforcePoll("CORmodelSlow");
   });
}