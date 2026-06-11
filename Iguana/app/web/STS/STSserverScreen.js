function STSserverScreen(){
   SETrender();
	document.querySelector('.STSserver').classList.add('selected');
   let Div = document.querySelector('.SETcontentBody');
   if(!Div) return;
   Div.innerHTML = STSserverScreenHtml();
   STSmodelInit();
}

function STSupdatePermissionEdit(){
   let Args = {};
   Args.target        = document.querySelector(".STSserverEdit");
   Args.enabled       = STSeditPermission();
   Args.disabledTitle = "You do not have permission to edit this.";
   Args.enabledHref   = "#settings/webserver/edit";
   BUTTONcontrol(Args);
}

function STSserverScreenHtml(){
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETdatabaseTitle">
         <div class="SETdatabaseLabel">Web Server</div>
         <div class="SETdatabaseActions BUTTONgroup">
            <a href="#settings/webserver/edit" class="STSserverEdit BUTTONdisabled BUTTONstandard">EDIT</a>
         </div>
      </div>
      <div class="SETpageDescription">Customize the settings for the web server.</div>
		<div class="SETversionPanel ">
         <div class="SETversionLabel">Description:</div>
         <div class="SETversionDetail STSdescription"><a href="${LINKsettingsWebServerFix}" target="_blank">Taking a long time? On a Mac? Click here.</a></div>
         <br>
         <div class="SETversionLabel">Port:</div>
         <div class="SETversionDetail STSport"></div>
         <br>
         <div class="STStable">
            <div class="SETversionLabel">Color:</div>
            <div class="SETversionDetail STScolorRow">
               <div class="STScolorValue"></div>
               <div class="STScolorView FORMinput"></div>
               <div></div>
            </div>
         </div>
         <br>
         <div class="SETversionLabel">Use HTTPS:</div>
         <div class="SETversionDetail STSuseHttps"></div>
         <div class="STSsecureDiv STShidden">
            <br>
            <div class="SETversionLabel">Certificate File:</div>
            <div class="SETversionDetail STScertificate"></div>
            <br>
            <div class="SETversionLabel">Private Key File:</div>
            <div class="SETversionDetail STSprivateKey"></div>
         </div>
		</div>
   </div>`;
}
