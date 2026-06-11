function GITHbitbucketServerUsernameHelpHtml(){
   let Link = GITHgitAddressLink();
   return /*html*/`
   Find your username in your <a class="GITHlink" href="${Link}/profile" target="_blank">Bitbucket profile settings ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <img class="GITHusernameBitbucketImg" src="GITH/GITHusernameBitbucketServer.png">
   </div>
   <br>
   <br>
   In the example the username is 'example-user'.
   <br>
   <br>
   <br>`;
}

function GITHbitbucketServerAppPasswordHelpHtml(){
   let Link = GITHgitAddressLink();
   return /*html*/`
   If you are experiencing errors you may need to disable <b>CAPTCHA</b> or you can <a class="GITHlink" href="${Link}/logout" target="_blank">logout and login</a> again through your browser.
   <br>
   <br>
   <br>
   Enter the <div class="GITHtableTitleStyle">Bitbucket Password</div> you login with here:
   <br>
   <br>`;
}