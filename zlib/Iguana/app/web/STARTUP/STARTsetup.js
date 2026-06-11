function STARTUPgoToLoginClick() { window.location.reload(); }

function STARTUPmodelInit(){  // model for initialization mode
   MODELshutdownAll();
   MODELpollerAdd('STARTUPmodel', 'STARTUPmodeValue', 1, 'mode', {});
   MODELviewAdd  ('STARTUPmodel', 'STARTUPupdateStatusText');
   MODELforcePoll('STARTUPmodel');
   MODELstart    ('STARTUPmodel');
}

function STARTUPmodelNormal(){ // model for normal mode
   MODELshutdownAll();
   MODELforcePoll('STARTUPmodel');
   MODELstart    ('STARTUPmodel');
   STARTUPupdateProceedButton();
}

function STARTUPclearButtonPoller(){
   for(let i = 0; i < MODELpollers.length; i++){
      if(MODELpollers[i].model === "STARTUPmodel" && MODELpollers[i].name === "STARTUPmodeValue") {
         MODELpollers.splice(i, 1);
      }
   }
}

function STARTUPisDone(){
   return MODELdata.STARTUPmodeValue && MODELdata.STARTUPmodeValue.data && MODELdata.STARTUPmodeValue.data.mode === "normal";
}

// Functions for status text and button
function STARTUPupdateStatusText(){
   let Text = document.querySelector('.STARTUPheading');
   if (Text && STARTUPisDone()) {
      STARTUPclearButtonPoller();
      Text.innerHTML = "IguanaX initialized.";
      STARTUPmodelNormal();
   }
}

function STARTUPupdateProceedButton(){
   let Button = document.querySelector('.STARTUPgoToLogin');
   if (!Button) return;
   COL_TRC("Enable continue to login button.")
   Button.classList.add('BUTTONaction');
   Button.classList.remove('BUTTONdisabled');
   Button.addEventListener("click", function () { STARTUPgoToLoginClick() });
   COL_TRC("Mode switched to normal - stop mode poller");
}

function STARTUPpage(){
   return /*html*/`
   <div class="STARTUPbackground">
      <div class='STARTUPelements'>
			<h1 class='STARTUPheading'>IguanaX is initializing...</h1>
         <div class='STARTUPgoToLogin BUTTONstandard BUTTONdisabled'>Continue to Login</div>
      </div>
   </div>`;
}

function STARTUPsetup(){
   STARTUPmodelInit();
   document.body.innerHTML = STARTUPpage();
}