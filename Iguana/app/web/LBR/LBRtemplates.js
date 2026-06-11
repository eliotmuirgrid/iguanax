function LBRupdateTemplateError(){
   let ErrorDiv = document.querySelector('.LBRnewCardErrorDiv');
   if(!ErrorDiv) return;
   let ErrorHtml = "";
   let TemplatesError = LBRtemplatesError();
   if(TemplatesError !== "") { ErrorHtml = /*html*/`<div class="LBRnewCardError"> There was a problem loading templates in the collection: ${TemplatesError}</div>` }
   if(ErrorDiv.innerHTML !== ErrorHtml) ErrorDiv.innerHTML = ErrorHtml;
}

function LBRupdateTemplateLoading(){
   let Loading = document.querySelector('.LBRcardLoading');
   if(!Loading) return;
   let Templates = LBRtemplates();
   if(Templates !== null) Loading.classList.add   ('LBRhidden');
   else            return Loading.classList.remove('LBRhidden');
}

function LBRupdateTemplates(){
   let Templates = LBRtemplates();
   if(Templates === null ) return;
   if( DEEPequal(Templates, LBRtemplatesRendered) ) return;
   let CardContentDiv = document.querySelector('.LBRcardContent-hide');
   if(!CardContentDiv) return;
   CardContentDiv.innerHTML = LBRcardsHtml();
   LBRtemplatesRendered = JSON.parse(JSON.stringify(Templates));
   LBRanimateCards();
}

function LBRupdateSelectedTemplate(){
   let RepoUrl = document.querySelector('.LBRrepoLinkInput');
   if(!RepoUrl) return;
   let UrlValue = RepoUrl.value.trim();
   let TemplateIdx = LBRselectedTemplateNumber(UrlValue);
   let PrevSelectedCard = document.querySelector('.LBRtemplateCardSelected');
   let PrevIdx = null;
   if(PrevSelectedCard) PrevIdx = Array.from(PrevSelectedCard.parentElement.children).indexOf(PrevSelectedCard);
   if(PrevIdx == TemplateIdx) return;
   if(PrevSelectedCard) PrevSelectedCard.classList.remove('LBRtemplateCardSelected');
   let Cards = document.querySelectorAll('.LBRtemplateCard');
   if(!Cards) return;
   let SelectedCard = Cards[TemplateIdx];
   if(!SelectedCard) return;
   SelectedCard.classList.add('LBRtemplateCardSelected');
}

function LBRselectedTemplateNumber(Url){
   if(Url === "") return -1;
   let Templates = LBRtemplates();
   for(i in Templates){ if(Url === Templates[i].git) return i; }
   return -1;
}

function LBRanimateCards(){
   const Container     = document.querySelector('.LBRcardContent-hide');
   const Cards         = document.querySelectorAll('.LBRtemplateCard');
   const ElementMargin = 32;
   for(let idx = 0; idx < Cards.length; idx++){
      let e = Cards[idx];
      const CardPos = e.offsetTop;
		e.classList.add('LBRtemplateCard-visible');
      if(CardPos <= Container.clientHeight + ElementMargin) {e.classList.add('LBRtemplateCard-visible');}
      else {
         Container.addEventListener('scroll', function() {
            let CurrentPos = Container.scrollTop + (Container.clientHeight + ElementMargin) - e.clientHeight;
				if(CardPos <= CurrentPos) {e.classList.add('LBRtemplateCard-visible');}
            Cards.forEach(e => {
               const CardPos = e.offsetTop;
               if(CardPos <= CurrentPos) {e.classList.add('LBRtemplateCard-visible');}
            });
         });
      }
   }
}

function LBRcardsHtml(){
   let Templates = LBRtemplates();
   if(Object.keys(Templates).length === 0) { return /*html*/`<div class='LBRnoTemplatesDefined'>* No templates are defined in this repository *</div>`;}
   let CardsHtml = /*html*/`<div class="LBRcardContent">`;
   for(Template of Templates){ CardsHtml += LBRcardHtml(Template); }
   return CardsHtml + /*html*/`</div>`;
}

function LBRcardHtml(Template) {
   return /*html*/`
   <div class="LBRtemplateCard ${Template.cached ? "LBRcached" : "LBRnotCached"}" data-repo="${Template.git}" ${Template.cached ? '' : /*html**/`title="Iguana was unable to cache the Git repository, possibly due to lack of access or the repository no longer existing in this workspace."`}>
      <div class="LBRtemplateCardHeader">
         <div class="LBRtemplateCardName">${ESChtmlEscape(Template.name)}</div>
      </div>
      <div class="LBRtemplateCardDesc">
         ${LBRdescriptionHtml(Template)}
      </div>
   </div>`;
}

function LBRdescriptionHtml(Template){
   if(Template?.cached) return /*html*/`${HASHTAGrenderHashtags(ESChtmlEscape(Template.description))}`;
   return /*html*/`<p>Unable to access the repository. Double click for more details.</p>`;
}

function LBRpassesFilter(Template, Search, SelectedCollections){
   let SearchWords = Search.split(' ');
   let PassesName = Template.name === undefined;
   if(!PassesName) PassesName = LBRhasWords(SearchWords, Template.name);
   let PassesDesc = Template.description === undefined;
   if(!PassesDesc) PassesDesc = LBRhasWords(SearchWords, Template.description);
   let SearchEmpty = Search === "";
   let PassesColl = SelectedCollections.includes(LBRshowAllCollections);
   if(!PassesColl) PassesColl = SelectedCollections.includes(Template.collection);
   return (PassesName || PassesDesc || SearchEmpty) && PassesColl;
}

function LBRfilterTemplates(){
   let Cards = document.querySelectorAll('.LBRtemplateCard');
   if(!Cards.length) return;
   let Search  = document.querySelector('.LBRfilter').value;
   let SelectedCollections = LBRselectedCollections();
   let Templates = LBRtemplates();
   for(let idx in Templates) {
      let StyleClass = "none";
      let Template = Templates[idx];
      if(LBRpassesFilter(Template, Search, SelectedCollections)) StyleClass = "block";
      if(Cards[idx].style.display !== StyleClass) Cards[idx].style.display = StyleClass;
   }
}


function LBRselectedCollections(){
   let divs = document.querySelectorAll('.LBRfilterByCollectionSelect .LBRcollectionCheck');
   let collections = [];
   for(let div of divs) {
      if(div.checked) collections.push(div.value);
   }
   if(collections.length === 0) collections.push(LBRshowAllCollections);
   return collections;
}
function LBRhasWords(KeyWords, SearchText){   // true if search text has all the key words
   if (KeyWords.length === 0) return true;
   if (SearchText.toLowerCase().indexOf(KeyWords[0].toLowerCase()) !== -1) { // text contains the first keyword
      return LBRhasWords(KeyWords.slice(1), SearchText);
   } else {
      return false;
   }
}

function LBRhtmlTemplates(){
   return /*html*/`
   <div class="LBRtemplateDiv ">
      <div class="LBRheaderTitle">LIBRARIES</div>
      <div class="LBRheaderDiv">	
         <input class="LBRfilter FORMinput" placeholder="Search" value="">
      </div>
      <div class="LBRcardContainer">
         <div class="LBRcardLoading"></div>
         <div class='LBRnewCardErrorDiv'></div>
         <div class="LBRcardContent-hide"></div>
      </div>
   </div>`;
}