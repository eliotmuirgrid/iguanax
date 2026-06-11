function DOC2resetHtml(){
   return /*html*/`
   <div class="DOC2resetPopout">
      <div class="DOC2resetWarning">
         This will reset the repository to the latest commit. Please confirm.
      </div>
      <button class="BUTTONaction BUTTONstandard DOC2resetConfirm">CONFIRM</button>
   </div>
   `;
}

function DOC2resetDialog(){
   let H = DOC2resetHtml();
   POPpopoutOpen({title : "Reset to last commit", content : H, callback : DOC2adapterRefresh});
   document.querySelector(".DOC2resetConfirm").addEventListener('click', function(E){ DOC2resetSubmit(E); });
}

function DOC2resetSubmit(E){
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   ApiData.commands = {0: "git restore ."};
   ApiData.development = true;
   APIcall('git/custom_command', ApiData, function(R) { DOC2resetSubmitResult(R); });
}

function DOC2resetSubmitResult(R){
   if (!R.success){
      document.querySelector(".DOC2resetPopout").innerText = R.error;
   } else {
      document.querySelector(".DOC2resetPopout").innerHTML = "Repository reverted successfully";
   }
}