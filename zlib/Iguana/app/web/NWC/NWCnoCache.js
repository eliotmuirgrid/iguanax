let NWCnoCachePopout;

function NWClibraryIssueHtml(GitUrl, GitWebUrl, LibUrl, LibWebUrl){
   return /*html*/`
   The following component uses a library that could not be cached. Without access to the library you cannot create the component.
   <br>
   <br>
   <br>
   ${ GitUrl ? /*html*/`<div class="NWCnoCacheRow"><div>The component:</div><a target="_blank" href=${GitWebUrl}>${GitUrl}</a></div>` : ""}
   ${!GitUrl ? /*html*/`<div class="NWCnoCacheRow"><div>The component:</div><div>[ url provided is blank ]</div></div>` : ""}
   <br>
   ${ LibUrl ? /*html*/`<div class="NWCnoCacheRow"><div>The problematic library:</div><a target="_blank" href=${LibWebUrl}>${LibUrl}</a></div>` : ""}
   ${!LibUrl ? /*html*/`<div class="NWCnoCacheRow"><div>The problematic library: </div><div>[ url provided is blank ]</div></div>` : ""}
   `;
}

function NWCrepositoryIssueHtml(GitUrl, GitWebUrl){
   return /*html*/`
   The following component could not be cached. Without access to the repository you cannot create the component.
   <br>
   <br>
   <br>
   ${ GitUrl ? /*html*/`<div class="NWCnoCacheRow"><div>The component:</div> <a target="_blank" href=${GitWebUrl}>${GitUrl}</a></div>` : ""}
   ${!GitUrl ? /*html*/`<div class="NWCnoCacheRow"><div>The component:</div> <div>[ url provided is blank ]</div></div>` : ""}`;
}

function NWCnoCacheHtml(GitUrl, GitWebUrl, LibUrl, LibWebUrl){
   return /*html*/`
   ${ !LibUrl ? NWCrepositoryIssueHtml(GitUrl, GitWebUrl) : NWClibraryIssueHtml(GitUrl, GitWebUrl, LibUrl, LibWebUrl)}
   <br>
   <br>
   You may not have access to this repository or it no longer exists in this workspace. 
   <br>
   <br>
   If you think this repository exists and you have access, make sure you are authenticated or try refreshing the cache.
   <br>
   <div class="BUTTONgroup CARDtemplateRemoveButtons">
      <div class='NWCremoveNoCache BUTTONaction BUTTONstandard'>OKAY</div>
   </div>`;
}

function NWCnoCachePopoutClicked(target){
   let SelectedCard = target.closest('.NWCtemplateCard');
   let Index = Array.from(document.querySelector('.NWCcardContent').children).indexOf(SelectedCard);
   let Templates = NWCtemplates();
   let Template = Templates[Index];
   if(!Template) return;
   console.log(Template);
   let LibWebUrl = Template?.lib_urls?.web || "";
   let LibUrl = Template?.lib_urls?.url || "";
   let GitUrl = Template?.git;
   let GitWebUrl = Template?.urls?.web || "";
   let Content = NWCnoCacheHtml(GitUrl, GitWebUrl, LibUrl, LibWebUrl);
   NWCnoCachePopout = POPpopoutOpen({title: 'Repository not found', content: Content});
   document.querySelector('.NWCremoveNoCache').addEventListener('click', function() { POPpopoutClose(NWCnoCachePopout);  });
}
