function ROLremoveRender(){
   ROLrender();
   let Role = PAGEhashParameter("role");
   APIcall("role/query", { name : Role }, function(R){
      if(!R?.success) return window.location = "/#settings/roles";
   });
   if(document.querySelector(".ROLremove")) return;
   POPpopoutOpen({
      title: 'Remove role', 
      content: ROLremoveHtml(Role), 
      esc_close : true,
      width : "500px",
      callback : ROLdefault
   });
   document.querySelector(".ROLremove").addEventListener("click", ROLremove)

}

function ROLdefault(){ window.location = "/#settings/roles"; }
   
function ROLremoveHtml(Role){
   return /*html*/`
   <br>
   <div class="ROLrow">
      <div class="FORMlabel">Role:</div>
      <div>${Role}</div>
   </div>
   <br>
   <br>
   Are you sure you would like to remove this role?
   <br>
   <br>
   <br>
   <br>
   <div class="ROLgroup BUTTONgroup">
      <a class="BUTTONcancel BUTTONstandard" href="/#settings/roles">CANCEL</a>
      <div class="ROLremove BUTTONaction BUTTONstandard">REMOVE</div>
   </div>`;
}

function ROLremove(){
   let Role = PAGEhashParameter("role");
   APIcall("role/delete", { name : Role}, function(R){
      if(!R?.success) return;
      let Message = GINremoveRole(Role);
      GINaddAndCommit({message: Message, settings: true});
      window.location = "/#settings/roles";
   })
}