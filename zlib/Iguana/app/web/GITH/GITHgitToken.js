
function GITHgitLabTokenHelpHtml(){
   let Link =GITHgitAddressLink();
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">Access Token</div> with the following scopes in your <a class="GITHlink" href="${Link}/-/user_settings/personal_access_tokens" target="_blank">Personal Access Tokens ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
         <img class="GITHtokenGitLabImg" src="GITH/GITHtokenGitLab.png">
      </div>
   <br>
   <br>
   Copy and paste the <div class="GITHtableTitleStyle">Personal Access Token</div> here:
   <br>
   <br>`;
}

function GITHgitHubTokenHelpHtml(){
   let Link = GITHgitAddressLink();
   return /*html*/`
   Create a <div class="GITHtableTitleStyle">Access Token</div> with the following scopes in your <a class="GITHlink" href="${Link}/settings/tokens/new" target="_blank">Personal Access Tokens ( click me ! )</a>:
   <br>
   <br>
   <div class="GITSimageDiv">
      <div class="GITHimageGrid">
         <img class="GITHtokenGitLabImg" src="GITH/GITHgitHubOne.png">
      </div>
      <div class="GITHimageGrid">
         <img class="GITHtokenGitLabImg" src="GITH/GITHgitHubTwo.png">
      </div>
   </div>
   <div>
   <br>
   <br>
   Copy and paste the <div class="GITHtableTitleStyle">Personal Access Token</div> here:
   <br>
   <br>`;
}

