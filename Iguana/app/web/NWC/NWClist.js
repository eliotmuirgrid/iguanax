function NWCaddComponentRow(ApiData){
   DRGdragSortListAddItem(NWCcomponentList, NWCcomponentCardHtml(ApiData), {'dragHandleSelector': '.NWCmoveRepoDiv', 'prepend': false});
   let Components = document.querySelectorAll('.NWCcomponentDiv');
   Components[Components.length-1].scrollIntoView({ block: 'end', behavior: 'smooth' });
}

function NWCaddComponentBlankRow() {
   let BlankName = "Custom Blank";
   if(NWCnameExistsBool(BlankName, NWCnameData())) BlankName = NWCnextAvailableName(BlankName, NWCnameData());
   NWCaddComponentRow({name : BlankName});
   MODELforcePoll('NWCmodel');
   NWCfocusLastCard();
}

function NWCfocusLastCard(){
   let div = NWClastCard()?.querySelector(".NWCname");
   if(!div) return;
   FORMfocus(div, true);
}

function NWClastCard(){
   let divs = document.querySelectorAll(".NWCcomponentDiv");
   if(!divs.length) return;
   return divs[divs.length - 1];
}

function NWCfindTags(Description, Flag){
   if(!Description) return "";
   let Words = Description.split(" ");
   let Tags = [];
   for(word of Words){
      if( Boolean(EDITisHashtag(word)) == Flag) Tags.push(word);
   }
   return Tags.join(" ");
}

function NWCcomponentCardHtml(ComponentData){
   let Tags = NWCfindTags(ComponentData?.description, true);
   let Description = NWCfindTags(ComponentData?.description, false);
   let hrefLink = ComponentData?.urls?.web_branch || "";
   let href = hrefLink ? ` href="${hrefLink}" target="_blank"` : "";
   let Git = ComponentData?.git;
   let GitLinkHtml = (Git && hrefLink) ? `
      <a class="NWCrepoLink BUTTONdefault BUTTONstandard"${href} title="Open the git url you are using.">
         <div class="NWCopenLinkDiv">${Git}</div>
      </a>` : `<div></div>`;
   let Branch = ComponentData?.branch || "";
   let BranchHtml = Branch ? `<div class="NWCbranch">${Branch}</div>` : "";
   return /*html*/`
   <div class="NWCfilterContainer NWCcomponentDiv">
      <div class="NWCrepoLinkHrefDiv">
         <div class="FORMbutton BUTTONcancel BUTTONwarning NWCremoveComponent">
            <svg class="NWCtrashIcon" width="16" height="16"><use href="NWC/NWCtrashIcon.svg#NWCtrashIcon"></use></svg>
         </div>
         <input class="NWCname FORMinput" placeholder="Name" value="${ComponentData.name}" title="Name of the component you are creating.">
         <div></div>
			<div class="NWCnameExistsFeedback"></div>
         <div class="NWCmoveRepoDiv">⋮⋮</div>

         <textarea  class="NWCdescription" placeholder="Description - what does this component do?" title="Description of the component you are creating.">${Description}</textarea>
         ${GitLinkHtml}
         ${BranchHtml}
         <input class="FORMinput NWCtag" placeholder="Tags - what groups does this belong to?" value="${Tags}">
      </div>
   </div>`;
}

function NWCremoveComponent(target){
   let ComponentCard = target.closest('.DRGdragSortListItem');
   if(ComponentCard) ComponentCard.remove();
   MODELforcePoll('NWCmodel');
}

function NWCgetComponentListData(){
   let Data = [];
   let Rows = document.querySelectorAll('.NWCcomponentDiv');
   for(let CustomRow of Rows){
      let TagInput = CustomRow.querySelector(".NWCtag").value.trim() || "";
      let GitElement            = CustomRow.querySelector('.NWCopenLinkDiv');
      let Tags = TagInput.split(/\s+/);
      let ComponentData = {};
      ComponentData.name        = CustomRow.querySelector('.NWCname').value.trim();
      ComponentData.description = CustomRow.querySelector('.NWCdescription').value.trim();
      ComponentData.branch      = CustomRow.querySelector('.NWCbranch')?.innerHTML.trim() || "";
      ComponentData.git         = GitElement ? GitElement.innerHTML.trim() : null;
      ComponentData.tags        = Tags;
      ComponentData.editable    = false;
      Data.push(ComponentData);
   }
   return Data;
}
