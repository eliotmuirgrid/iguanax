function TUNmyKeyScreen(){
   TUNscreen();
   POPpopoutOpen({
      title: 'My public key', 
      width: '800px', 
      content: TUNmyKeyHtml(),  
      esc_close : true,
      callback : TUNdestroyAdd
   });
   document.querySelector(".TUNcancel") .addEventListener("click", function() { location.hash = "cluster"; });
   MODELupdate('TUNmodel');
}

function TUNdestroyAdd(){
   location.hash = "cluster";
}

function TUNmyKeyHtml(){
   return /*html*/`
   <div class="TUNinformation">You'll need to share this Public Key with another Iguana user that you want to connect with:
   </div>
   <br>
   <textarea disabled class="TUNpublicKeyCopy" placeholder="Your public key..." title="Public key of the peer you are trying to add."></textarea>
   <br>
   <div class="BUTTONgroup TUNbuttonsRow">
      <div class="TUNcancel BUTTONstandard BUTTONaction">CLOSE</div>
   </div>`;
}

function TUNkey(){
   let Div = document.querySelector('.TUNpublicKeyCopy')
   if(!Div) return;
   if (!MODELdata.TUNpublicKey.success){
      return;
   }
   let Key =  MODELdata.TUNpublicKey.data.key;
   if(Div.innerHTML != Key) Div.innerHTML = Key;
}
