function PERmyInfo(){
   PERview();
   PERmyInfoPopout();
}

function PERmyInfoPopout(){
   if(document.querySelector(".PERmyInfoModal")) return;
   POPpopoutOpen({
      title: 'My info', 
      content: PERmyInfoHtml(), 
      esc_close : true,
      width : "600px",
      callback : PERdefault
   });
}


function PERmyInfoHtml(){
   let Data = {} || PERmyInfoData();
   return /*html*/`
   <div class="PERmyInfoModal FORMlabel">Address:</div>
   <br>
   <div class="PERdisplay">${Data.address}</div>
   <br>
   <br>
   <div class="FORMlabel">Public Key:</div>
   <br>
   <textarea class="FORMinput PERpublicKey" disabled></textarea>
   <br>
   <div class="PERerror"></div>
   <br>
   <br>
   <div class="PERgroup BUTTONgroup">
      <a href="/#settings/peers" class="BUTTONcancel BUTTONstandard">CLOSE</a>
   </div>`;
}