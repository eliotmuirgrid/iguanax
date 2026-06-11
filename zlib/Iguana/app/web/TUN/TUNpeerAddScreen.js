let TUNaddScreenCard;

function TUNpeerAddScreen(){
   TUNscreen();
   TUNaddScreenCard = POPpopoutOpen({
      title: 'Add peer', 
      width: '900px', 
      content: TUNsetupPeerAddHtml(),  
      esc_close : true,
      callback : TUNdestroyAdd
   });
   document.querySelector(".TUNaddPeerConfirm").addEventListener("click", function() { TUNpeerAdd();});
   document.querySelector(".TUNcancel")        .addEventListener("click", function() { location.hash = "cluster"; });
}

function TUNdestroyAdd(){
   location.hash = "cluster";
}

function TUNsetupPeerAddHtml(){
   return /*html*/`
   <div class="TUNaddHeader">FROM PUBLIC KEY:</div>
   <br>
   <div class="TUNaddContainer">
      <input class="TUNname FORMinput" placeholder="Name" title="Name for the peer you are trying to add.">
      <textarea class="TUNpublicKey" placeholder="Public key" title="Public key of the peer you are trying to add."></textarea>
   </div>
   <div class="TUNerror"></div>
   <div class="BUTTONgroup TUNbuttonsRow">
      <div class="TUNcancel BUTTONstandard BUTTONdefault">CANCEL</div>
      <div class="TUNaddPeerConfirm BUTTONaction BUTTONstandard">ADD</div>
   </div>`;

}

function TUNpeerAdd(){
   let Name        =  document.querySelector(".TUNname")       .value.trim(); 
   let Key         =  document.querySelector(".TUNpublicKey")  .value.trim(); 
   let Data = {};
   Data.name        = Name;
   Data.key         = Key;
   APIcall("tunnel/peer/add", Data, function(R) {
      if (!R.success){
         document.querySelector(".TUNerror").innerHTML = R.error;
      } else {
         location.hash = `cluster/peer?name=${Name}`;
      }
   });
}