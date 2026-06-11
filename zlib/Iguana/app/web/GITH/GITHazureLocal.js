function GITHazureLocalUsernameHtml(){
   let Link = GITHgitAddressLink();
   return /*html*/`
   Find your username in your <a class="GITHlink" href="${Link}" target="_blank">Azure Devops user profile tab ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHusernameAzureImg" src="GITH/GITHazureLocalUsername.png">
   </div>
   <br>
   <br>
   In the example the username is 'example-user'.
   <br>
   <br>
   <br>`;
}

function GITHazureLocalTokenHtml(){
   let Link = GITHgitAddressLink();
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">New Token</div> in your <a class="GITHlink" href="${Link}/_usersSettings/tokens" target="_blank">Azure Devops User Settings ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenBitbucketImg" src="GITH/GITHazureLocalNewToken.png">
   </div>
   <br>
   <br>
   With the following permissions:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenAzureLocalImg" src="GITH/GITHazureToken.png">
   </div>
   <br>
   <br>
   Copy and paste the <div class="GITHtableTitleStyle">Personal Access Token</div> here:
   <br>
   <br>`;
}

function GITHazureLocalRegisterKeyHtml(PublicKey){
   let Link = GITHgitAddressLink();
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">New Key</div> in your <a class="GITHlink" href="${Link}/_usersSettings/keys" target="_blank">Azure Devops User Settings ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenAzureLocalImg" src="GITH/GITHazureLocalAddKey.png">
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
      <img class="GITHaddLocalKeyAzureImg" src="GITH/GITHazureLocalAddPublicKey.png">
   </div>
   <div class="BUTTONgroup GITHbutton">
      <div class="GITHdone BUTTONstandard BUTTONdefault">CLOSE</div>
   </div>
   `;
}  