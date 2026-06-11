function GITHazureUsernameHelpHtml(){
   let Provider = GITHgitProvider();
   if(Provider == "Server - Azure Devops") return GITHazureLocalUsernameHtml();
   return /*html*/`
   Find your username in your <a class="GITHlink" href="https://aex.dev.azure.com/" target="_blank">Azure Devops profile settings ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHusernameAzureImg" src="GITH/GITHazureUsername.png">
   </div>
   <br>
   <br>
   In the example the username is 'example-user'.
   <br>
   <br>
   <br>`;
}

function GITHazureTokenHelpHtml(){
   let Provider = GITHgitProvider();
   if(Provider == "Server - Azure Devops") return GITHazureLocalTokenHtml();
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">New Token</div> in your <a class="GITHlink" href="https://dev.azure.com/${GITHgitUsername()}/_usersSettings/tokens" target="_blank">Azure Devops User Settings ( click me ! )</a>: <div class="GITHinfo"> Username must be configured already </div>
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenBitbucketImg" src="GITH/GITHazureNewToken.png">
   </div>
   <br>
   <br>
   Ensure the token has access to all organizations with full access:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenAzureImg" src="GITH/GITHazureToken.png">
   </div>
   <br>
   <br>
   Copy and paste the <div class="GITHtableTitleStyle">Personal Access Token</div> here:
   <br>
   <br>`;
}

function GITHproblemWithSshKey(){
   let Azure = GITHgitProvider().includes("Azure Devops");
   let PublicKey = GITHkeyData()?.public_key_content || "";
   if(Azure && !PublicKey.startsWith("ssh-rsa")) return true;
   return false;
}

function GITHproblemAzureHtml(PublicKey){
   return /*html*/`
   <div class="GITHtableTitleStyle">Public Key Data</div>:
   <br>
   <br>
   <textarea class="GITHpublicKeyContent FORMinput">${PublicKey}</textarea>
   <br>
   <br>
   <div class="GITHnotSet">This is an Invalid key: Valid keys will start with "ssh-rsa". [<div class="GITHlink GITHregenerate">Click here to Regenerate Key</div>]</div>
   <br>
   <div class="BUTTONgroup GITHbutton">
      <div class="GITHdone BUTTONstandard BUTTONdefault">CLOSE</div>
   </div>`;
}

function GITHazureRegisterKeyHtml(){
   let Provider = GITHgitProvider();
   let PublicKey = GITHkeyData()?.public_key_content || "";
   if(GITHproblemWithSshKey()) return GITHproblemAzureHtml(PublicKey);
   if(Provider == "Server - Azure Devops") return GITHazureLocalRegisterKeyHtml(PublicKey);
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">New Key</div> in your <a class="GITHlink" href="https://dev.azure.com/${GITHgitUsername()}/_usersSettings/keys" target="_blank">Azure Devops User Settings ( click me ! )</a>: <div class="GITHinfo"> Username must be configured already </div>
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenAzureImg" src="GITH/GITHazureNewKey.png">
   </div>
   <br>
   Copy your <div class="GITHtableTitleStyle">Public Key Data</div>:
   <br>
   <br>
   <textarea class="GITHpublicKeyContent FORMinput">${PublicKey}</textarea>
   <br>
   <br>
   And paste it into the <div class="GITHtableTitleStyle">Public Key Data</div> field:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHaddKeyAzureImg" src="GITH/GITHazureAddKey.png">
   </div>
   <div class="BUTTONgroup GITHbutton">
      <div class="GITHdone BUTTONstandard BUTTONdefault">CLOSE</div>
   </div>
   `;
}

function GITHazureRegenerate(){
   POPpopoutClose(GITHsetSshCard);
   GITHsetKeyFilesPopout();
   GITHgenerateKeys();
}