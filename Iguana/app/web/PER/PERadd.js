function PERadd(){
   PERview();
   PERaddPopout();
}

function PERaddPopout(){
   if(document.querySelector(".PERnew")) return;
   POPpopoutOpen({
      title: 'Add Iguana', 
      content: PERaddHtml(), 
      esc_close : true,
      width : "600px",
      callback : PERdefault
   });
   document.querySelector(".PERnew").addEventListener("click", PERnew);
   FORMfocus(document.querySelector(".PERnewAddress"));
   FORMenterListener(document.querySelector(".PERnewAddress"), PERnew);
}

function PERaddHtml(){
   return /*html*/`
   <div class="FORMlabel">Address:</div>
   <br>
   <input class="FORMinput PERnewAddress">
   <br>
   <br>
   <div class="FORMlabel">Public Key:</div>
   <br>
   <textarea class="FORMinput PERpublicKey"></textarea>
   <br>
   <div class="PERerror"></div>
   <br>
   <br>
   <div class="PERgroup BUTTONgroup">
      <a href="/#settings/peers" class="BUTTONcancel BUTTONstandard">CANCEL</a>
      <div class="PERnew BUTTONaction BUTTONstandard">CONFIRM</div>
   </div>`;
}

function PERrenderError(Error){
   let div = document.querySelector(".PERerror");
   div.innerHTML = "Error - " + Error;
}

function PERnew(){
   let Div = document.querySelector(".PERnewAddress");
   let Address = (Div?.value || "").trim();
   APIcall("peers/add", { address : Address }, function(R){
      if(!R?.success) return PERrenderError(R?.error);
      let Data = R?.data || "";
      let Message = GINaddPeer(Address, Data?.peer);
      GINaddAndCommit({message: Message, settings: true});
      window.location = PERlocation();
   });
}
