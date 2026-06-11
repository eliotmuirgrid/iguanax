let LBRnoCachePopout;

function LBRnoCacheHtml(GitUrl){
   return /*html*/`
   ${ GitUrl ? /*html*/`<p>Iguana was unable to cache the Git repository: <a target="_blank" href=${GitUrl}>${GitUrl}</a></p>` : ""}
   ${!GitUrl ? /*html*/`<p>Iguana was unable to cache the Git repository: [ url provided is blank ]</p>` : ""}
   <br>
   You may not have access to this repository or it no longer exists in this workspace. 
   <br>
   <br>
   If you think this repository exists and you have access, make sure you are authenticated or try refreshing the cache.
   <br>
   <div class="BUTTONgroup CARDtemplateRemoveButtons">
      <div class='LBRremoveNoCache BUTTONaction BUTTONstandard'>OKAY</div>
   </div>`;
}

function LBRnoCachePopoutClicked(target){
   let SelectedCard = target.closest('.LBRtemplateCard');
   let Index = Array.from(document.querySelector('.LBRcardContent').children).indexOf(SelectedCard);
   let Templates = LBRtemplates();
   let Template = Templates[Index];
   if(!Template) return;
   APIcall("git/all_urls", {url : Template.git}, function(R){
      let Link = null;
      if (R.success){
         Link = R?.data?.web;
      }
      let Content = LBRnoCacheHtml(Link);
      LBRnoCachePopout = POPpopoutOpen({title: 'Repository not found', content: Content});
      document.querySelector('.LBRremoveNoCache').addEventListener('click', function() { POPpopoutClose(LBRnoCachePopout);  });
   });  
}
