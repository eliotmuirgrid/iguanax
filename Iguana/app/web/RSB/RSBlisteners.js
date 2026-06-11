function RSBsetListeners(){
   let div = document.querySelector(".RSBmodal");
   if(!div) return;
   div.addEventListener("click", RSBclick);
}

function RSBclick(e){
   if(e.target.closest(".RSBclose"))    return RSBclose();
   if(e.target.closest(".RSBresubmit")) return RSBresubmit();
}

function RSBclose(){
   return POPpopoutClose(RSBcard);
}

function RSBresubmit(){
   const Content = LFDsaveEncoded(RSBeditor.getValue());
   console.log(Content);
   let Args = {
      target_id    : document.querySelector(".RSBselect").value.trim(),
      message      : Content,
      component_id : DETcomponentId(),
      time         : PAGEhashParameter("time"),
      index        : PAGEhashParameter("id"),
   };

   APIcall("logs/resubmit/custom", Args, function(Response){
      if (Response.success) {
         SNCKsnackbar('success', `The modified message is being resubmitted.`, 12000);
         RSBclose();
      } else {
         SNCKsnackbar('error', Response.error);
      }
   });
}