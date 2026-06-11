function GITHbitbucketUsernameHelpHtml(){
   let Link = GITHgitAddressLink();
   return /*html*/`
   Find your username in your <a class="GITHlink" href="${Link}/account/settings/" target="_blank">Bitbucket profile settings ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHusernameBitbucketImg" src="GITS/img/GITSusername.png">
   </div>
   <br>
   <br>
   In the example the username is 'example-user'.
   <br>
   <br>
   <br>`;
}

function GITHbitbucketAppPasswordHelpHtml(){
   let Link = GITHgitAddressLink();
   // let Email = GITHtokenEmail();
   let Email = GITHgitTokenValid()?.email || "";
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">Bitbucket API Token with scopes</div>: <strong><a class="GITHlink" href="https://id.atlassian.com/manage-profile/security/api-tokens" target="_blank">Create ( CLICK HERE ! )</a></strong>
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenBitbucketImg" src="GITS/img/GITScreateBitBuckApiToken.png">
   </div>
   <br>
   <div>Select <div class="GITHtableTitleStyle">Bitbucket</div>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHtokenBitbucketImg" src="GITS/img/GITSselectApp.png">
   </div>
   <br>
   <div>Select the <div class="GITHtableTitleStyle">Bitbucket Scopes</div>:
   <br>
   <br>
   <div class="GITSimageDiv GITSgrid">
      <div>
         <div>admin:repository:bitbucket</div>
         <div>read:repository:bitbucket</div> 
         <div>read:user:bitbucket</div>
      </div>
      <div>
         <div>read:workspace:bitbucket</div>
         <div>read:ssh-key:bitbucket</div>
         <div>write:ssh-key:bitbucket</div>
      </div>
   </div>
   <br>
   <br>
   Enter your <div class="GITHtableTitleStyle">Bitbucket Email </div> here: ( Leave blank if you are unsure! )
   <br>
   <br>
   <input class="GITHtableVal GITHeditEmail FORMinput" value="${Email}">
   <br>
   <br>
   <br>
   Copy and paste the <div class="GITHtableTitleStyle">Bitbucket API Token</div> here:
   <br>
   <br>`;
}

// Create a <div class="GITHtableTitleStyle">Bitbucket API Token with scopes</div> with the selected scopes: <a class="GITHlink" href="${Link}/account/settings/app-passwords/new" target="_blank">Create Token ( CLICK HERE ! )</a>
