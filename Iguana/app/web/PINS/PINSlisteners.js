function PINSlisteners(){
   let div = document.querySelector(".PINSmodal");
   if(!div) return;
   div.addEventListener("click", PINSclick);
}

function PINSclick(e){
   if(e.target.closest(".PINSaddRow"))   return PINSadd();
   if(e.target.closest(".PINSclearAll")) return PINSclearAll();
   if(e.target.closest(".PINSdelete"))   return PINSdeleteRow(e.target);
   if(e.target.closest(".PINSsave"))     return PINSsaveCall();
   if(e.target.closest(".BUTTONcancel")) return PINSclose();
}

function PINSerror(Error){
   let div = document.querySelector(".PINSerrorFeedback");
   if(!div) return;
   div.innerHTML = Error || "could not save user pins. (Reason unknown)";
}

function PINSvalue(){
   const Rows = document.querySelectorAll('.PINSrow');
   const Data = {};
   for(let i=0; i < Rows.length; i++){
      let Row = Rows[i];
      const Guid = Row.querySelector('.PINSguid').value.trim();
      if(Guid == "") continue;
      let Entry = {};
      let Px = parseInt(Row.querySelector('.PINSpx').value.trim(),10) || 0;
      let Py = parseInt(Row.querySelector('.PINSpy').value.trim(),10) || 0;
      Entry.px = Math.round(Px);
      Entry.py = Math.round(Py);
      Data[Guid] = Entry;
   }
   return Data;
}

function PINSsaveCall(){
   let Data = {};
   Data.pins = PINSvalue();
   APIcall("pins/save", Data, function(R){
      if(!R.success) return PINSerror("Could not save: " + R.error || "");
      let Message = GINsavePins();
      GINaddAndCommit({message: Message, users: true});
      PINSclose();
   });
}

function PINSadd(){
   let div = document.querySelector(".PINStable");
   if(!div) return;
   SETappendTo(div, PINSrowHtml("", {}));
   SETfocusLast(".PINSguid");
}

function PINSdeleteRow(target){
   let row = target.closest(".PINSrow");
   if(row) row.remove();
}

function PINSclearAll(){
   let div = document.querySelector(".PINStable");
   if(!div) return;
   div.innerHTML = PINSrowHtml("", {});
}
