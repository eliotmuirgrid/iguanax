function NWCtemplates(){
   return MODELdata?.NWCtemplates?.data;
}

function NWCtemplatesError(){
   return MODELdata?.NWCtemplates?.error;
}

function NWCupdateTemplateError(){
   let ErrorDiv = document.querySelector('.NWCnewCardErrorDiv');
   if(!ErrorDiv) return;
   let ErrorHtml = "";
   let TemplatesError = NWCtemplatesError();
   if(TemplatesError) { ErrorHtml = /*html*/`<div class="NWCnewCardError"> There was a problem loading templates in the collection: ${TemplatesError}</div>` }
   if(ErrorDiv.innerHTML !== ErrorHtml) ErrorDiv.innerHTML = ErrorHtml;
}

function NWCupdateTemplateLoading(){
   let Loading = document.querySelector('.NWCcardLoading');
   if(!Loading) return;
   let Templates = NWCtemplates();
   if(Templates) Loading.classList.add   ('NWChidden');
   else          Loading.classList.remove('NWChidden');
}

function NWCupdateTemplates(){
   let Templates = NWCtemplates();
   if(!Templates) return;
   if (DEEPequal(Templates, NWCtemplatesRendered)) return;
   let CardContentDiv = document.querySelector('.NWCcardContent-hide');
   if(!CardContentDiv) return;
   CardContentDiv.innerHTML = NWCcardsHtml();
   NWCtemplatesRendered = JSON.parse(JSON.stringify(Templates));
   NWCanimateCards();
}

function NWCupdateAccessWarning(){
   let Div = document.querySelector('.NWCaccessWarning');
   if(!Div) return;
   let Html = "";
   let Templates = NWCtemplates();
   if(!Templates) return;
   if(NWCbitbucketAccessData() && Object.keys(Templates).length && !NWCbitbucketAccess()) Html = `<b>WARNING:</b> Cannot connect to ${NWCbitbucketAccessData()?.url || "bitbucket.org"}   [<a href="#settings/git">Learn more</a>]`
   if(Div.innerHTML !== Html) Div.innerHTML = Html;
}

function NWCanimateCards(){
   const Container     = document.querySelector('.NWCcardContent-hide');
   const Cards         = document.querySelectorAll('.NWCtemplateCard');
   const ElementMargin = 32;
   for(let idx = 0; idx < Cards.length; idx++){
      let e = Cards[idx];
      const CardPos = e.offsetTop;
		e.classList.add('NWCtemplateCard-visible');
      if(CardPos <= Container.clientHeight + ElementMargin) {e.classList.add('NWCtemplateCard-visible');}
      else {
         Container.addEventListener('scroll', function() {
            let CurrentPos = Container.scrollTop + (Container.clientHeight + ElementMargin) - e.clientHeight;
				if(CardPos <= CurrentPos) {e.classList.add('NWCtemplateCard-visible');}
            Cards.forEach(e => {
               const CardPos = e.offsetTop;
               if(CardPos <= CurrentPos) {e.classList.add('NWCtemplateCard-visible');}
            });
         });
      }
   }
}

function NWCcardsHtml(){
   let Templates = NWCtemplates();
   if(NWCbitbucketAccessData() && !NWCbitbucketAccess() && Object.keys(Templates).length === 0) { return /*html*/`<div class='NWCnoBitbucketAccess'><b class="NWCaccessError">Error</b>: You do not have access to <b class="NWCbitbucketUrl">${NWCbitbucketAccessData()?.url || "bitbucket.org"}</b> - [<a href="#settings/git">Learn more</a>]</div>`;}
   if(Object.keys(Templates).length === 0) { return /*html*/`<div class='NWCnoTemplatesDefined'>${NWCblankCardHtml()}<div class="NWCnoTemplatesDefinedText">* No templates are defined in this repository *</div></div>`;}
   let CardsHtml = /*html*/`<div class="NWCcardContent">${NWCblankCardHtml()}`;
   for(let Template of Templates) { CardsHtml += NWCcardHtml(Template); }
   return CardsHtml + /*html*/`</div>`;
}

function NWCblankCardHtml(){
   return /*html*/`
   <div class="NWCblankCard">
      <div class="NWCblankCardName">Custom Blank</div>
      <div class="NWCblankCardContent">Start a project from scratch.</div>
   </div>`
}

function NWCcardHtml(Template) {
   const CustomTemplate = Template.git === "ssh://git@bitbucket.org/interfaceware/custom.git"; // Hiding this repo/card as we now support application-level custom blank component.
   return /*html*/`
   <div class="NWCtemplateCard ${Template.cached ? 'NWCcached' : 'NWCnoCache'} ${CustomTemplate ? 'NWCcustomTemplateCard' : ''}" data-repo="${Template.git}" ${Template.cached ? '' : /*html**/`title="Iguana was unable to cache the Git repository, possibly due to lack of access or the repository no longer existing in this workspace."`}>
      <div class="NWCtemplateCardHeader">
         <div class="NWCtemplateCardName">${ESChtmlEscape(Template.name)}</div>
      </div>
      <div class="NWCtemplateCardDesc">
         ${NWCdescriptionHtml(Template)}
      </div>
   </div>`;
}

function NWCdescriptionHtml(Template){
   let Cached = Template?.cached;
   let LibWebUrl = Template?.lib_urls?.web || "";
   let LibUrl = Template?.lib_urls?.url || "";
   let GitUrl = Template?.git;
   let GitWebUrl = Template?.urls?.web || "";
   let Branch = Template?.branch || "";
   let Description = ESChtmlEscape(Template.description);
   if(Cached) return HASHTAGrenderHashtags(Description);
   return NWCerrorCachingHtml(GitUrl, GitWebUrl, LibUrl, LibWebUrl, Branch);
}

function NWCcouldNotLoadHtml(GitUrl, GitWebUrl, Branch){
   if(!Branch) return /*html*/`Repo unreachable or access failed: <br><a target="_blank" href="${GitWebUrl}">${GitUrl}</a>`;
   return /*html*/`Branch <b>${Branch}</b> not found or access failed:<br><a target="_blank" href="${GitWebUrl}">${GitUrl}</a>`;
}

function NWCerrorCachingHtml(GitUrl, GitWebUrl, LibUrl, LibWebUrl, Branch){
   if(!LibUrl) return NWCcouldNotLoadHtml(GitUrl, GitWebUrl, Branch);
   return /*html*/`<p>This uses a library that could not be cached: <br> <a target="_blank" href="${LibWebUrl}">${LibUrl}</a></p>`;
}

function NWCpassesFilter(Template, Search, SelectedCollections){
   let SearchWords = Search.split(' ');
   let PassesName = Template.name === undefined;
   if(!PassesName) PassesName = NWChasWords(SearchWords, Template.name);
   let PassesDesc = Template.description === undefined;
   if(!PassesDesc) PassesDesc = NWChasWords(SearchWords, Template.description);
   let SearchEmpty = Search === "";
   let PassesColl = SelectedCollections.includes(Template.collection) || SelectedCollections.includes(NWCshowAllCollections);
   return (PassesName || PassesDesc || SearchEmpty) && PassesColl;
}

function NWCfilterTemplates(){
   let Cards = document.querySelectorAll('.NWCtemplateCard');
   if(!Cards.length) return;
   let Search  = document.querySelector('.NWCfilter').value.trim();
   let Templates = NWCtemplates();
   let SelectedCollections = NWCselectedCollections(); 
   for(let idx in Templates) {
      let StyleClass = "none";
      let Template = Templates[idx];
      if(NWCpassesFilter(Template, Search, SelectedCollections)) StyleClass = "block";
      if(Cards[idx].style.display !== StyleClass) Cards[idx].style.display = StyleClass;
   }
}

function NWCselectedCollections(){
   let divs = document.querySelectorAll('.NWCfilterByCollectionSelect .NWCcollectionCheck');
   let collections = [];
   for(let div of divs) {
      if(div.checked) collections.push(div.value);
   }
   if(collections.length === 0) collections.push(NWCshowAllCollections);
   return collections;
}

function NWChasWords(KeyWords, SearchText){   // true if search text has all the key words
   if (KeyWords.length === 0) return true;
   if (SearchText.toLowerCase().indexOf(KeyWords[0].toLowerCase()) !== -1) { // text contains the first keyword
      return NWChasWords(KeyWords.slice(1), SearchText);
   } else {
      return false;
   }
}
