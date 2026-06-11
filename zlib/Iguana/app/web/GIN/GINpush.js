function GINpushTo(){
   let User = GINuser();
   if(!User) SNCKsnackbar("warning", "No user set to Push as. Please set the 'Push/Pull as' field.");
   let div = document.querySelector(".GINpushButtonDiv");
   if(!div) return;
   div.innerHTML = "Pushing...";
   div.classList.remove("GINpushButton");
   div.classList.remove("BUTTONdefault");
   APIcall("instance/push", {force: true, manual: true}, function(response)   {
      div.innerHTML = "";
      if(!response.success) {
         SNCKsnackbar("error", "Could not push to git: " + response?.error || "");
         MODELforcePoll("GINmodel");
         GINresetPushButton();
      }
      else {
         div.innerHTML = "Pushed!";
         MODELforcePoll("GINmodel");
         setTimeout(function(){
            GINresetPushButton();
         }, 2000);
      }
   });
}

function GINresetPushButton(){
   let div = document.querySelector(".GINpushButtonDiv");
   if(!div) return;
   div.innerHTML = "";
   div.classList.add("BUTTONdefault");
   div.classList.add("GINpushButton");
}