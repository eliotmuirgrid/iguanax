function DOCresetHtml(){
   return /*html*/`
   <div class="DOCresetPopout">
      <div class="DOCresetWarning">
         This will reset the repository to the latest commit. Please confirm.
      </div>
      <button class="BUTTONaction BUTTONstandard DOCresetConfirm">CONFIRM</button>
   </div>
   `;
}

function DOCresetClick(){
   document.querySelector(".DOCreset").addEventListener('click', function(E){
      DOCresetDialog();
   });
}

function DOCresetDialog(){
   let H = DOCresetHtml();
   POPpopoutOpen({title : "Reset to last commit", content : H, callback : DOCadapterRefresh});
   document.querySelector(".DOCresetConfirm").addEventListener('click', function(E){ DOCresetSubmit(E); });
}

function DOCresetSubmit(E){
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   ApiData.commands = {0: "git restore ."};
   ApiData.development = true;
   APIcall('git/custom_command', ApiData, function(R) { DOCresetSubmitResult(R); });
}

function DOCresetSubmitResult(R){
   if (!R.success){
      document.querySelector(".DOCresetPopout").innerText = R.error;
   } else {
      document.querySelector(".DOCresetPopout").innerHTML = "Repository reverted successfully";
   }
}