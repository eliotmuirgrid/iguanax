function ROLaddRender(){
   ROLrender();
   if(document.querySelector(".ROLnew")) return;
   POPpopoutOpen({
      title: 'Add a role', 
      content: ROLaddHtml(), 
      esc_close : true,
      width : "400px",
      callback : ROLdefault
   });
   document.querySelector(".ROLnew").addEventListener("click", ROLnew);
   FORMfocus(document.querySelector(".ROLnewName"));
   FORMenterListener(document.querySelector(".ROLnewName"), ROLnew);
}

function ROLaddHtml(){
   return /*html*/`
   Roles must begin with '#' and have no spaces.
   <br>
   <br>
   <br>
   <div class="FORMlabel">Role:</div>
   <br>
   <input class="FORMinput ROLnewName">
   <br>
   <br>
   <div class="ROLaddError"></div>
   <br>
   <br>
   <div class="ROLgroup BUTTONgroup">
      <a href="/#settings/roles" class="BUTTONcancel BUTTONstandard" >CANCEL</a>
      <div class="ROLnew BUTTONaction BUTTONstandard">CONFIRM</div>
   </div>`;
}

function ROLaddError(Error){
   let div = document.querySelector(".ROLaddError");
   div.innerHTML = "Error - " + Error;
}

function ROLnew(){
   let Div = document.querySelector(".ROLnewName");
   let Name = (Div?.value || "").trim();
   APIcall("role/add", {name : Name}, function(R){
      if(!R?.success) return ROLaddError(R?.error);
      let Role = R?.data || "";
      let Message = GINaddRole(Role);
      GINaddAndCommit({message: Message, settings: true});
      window.location = "/#settings/roles/edit?role=" + Name;
   });
}
