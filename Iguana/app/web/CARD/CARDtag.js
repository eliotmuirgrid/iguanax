let CARDconfirmTag;

function CARDtagInitHtml(){
   return /*html*/`
   <div class="CARDtagRow">
      <div class="FORMlabel">Tags:</div>
      <div class="CARDtagTarget">
         ${CARDtagHtml()}
      </div>
   </div>`;
}

function CARDtagHtml(){
   const Perm = CARDpermissionData()?.edit_tags;
   const Btn  = `<div class="CARDeditBtn"><img src="CARD/CARDiconMoreOptions.svg"></div>`;
   return /*html*/`
   <div class="CARDtagRowInner">
      <div class="CARDtagValue"></div>
      <a class="BUTTONdefault BUTTONstandard CARDtagMoreInfo" title="More information on Tags." href="${LINKcardTags}" target="_blank">ⓘ</a>
      <div class="CARDtagEditTarget">${Perm ? Btn : ''}</div>
   </div>`;
}

function CARDtagEditHtml(){
   let Tags = CARDtagData();
   return /*html*/`
   <div class="CARDtagRowInnerEdit">
      <input class="FORMinput CARDtagEditInput" value="${Tags.join(" ")}">
      <div class="BUTTONcancel BUTTONstandard CARDedit">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard">
         <img src="CARD/CARDiconCheck.svg">
      </div>
      <div class="CARDtagEditInputError"></div>
   </div>`;
}

function CARDtagData(){ return CARDdata()?.tags || ""; }

function CARDupdateTag(){
   let Tags = CARDtagData();
   let div = document.querySelector(".CARDtagValue");
   if(!div) return;
   let Html = HASHTAGrenderTags(Tags, "CARDtagClickable", CARDcomponentRoleData());

   const Perm = CARDpermissionData()?.edit_tags;
   const Class = Perm ? 'CARDtags' : 'CARDtags CARDdisableLink';
   let Title   = Perm ? ""         : ` title="You do not have permission to edit."`
   if( Html == "") Html = `<div class="${Class} CARDaddTags"${Title} value="">+ Add tags +</div>`;
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function CARDupdateTagEditButton(){
   let div = document.querySelector(".CARDtagEditTarget");
   if(!div) return;
   const Perm = CARDpermissionData()?.edit_tags;
   let Html = Perm ? `<div class="CARDeditBtn"><img src="CARD/CARDiconMoreOptions.svg"></div>` : "";
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function CARDeditTags(){
   CARDdisableOpenEdit();
   let div = document.querySelector(".CARDtagTarget");
   if(div) { div.innerHTML = CARDtagEditHtml(); };
   FORMfocus(document.querySelector(".CARDtagEditInput"));
   FORMenterListener(document.querySelector(".CARDtagEditInput"), CARDsaveTags);
   MODELupdate('CARDmodel');
}

function CARDviewTags(){
   let div = document.querySelector(".CARDtagTarget");
   if(div) { div.innerHTML = CARDtagHtml(); };
}

function CARDcancelTags(){
   let div = document.querySelector(".CARDtagTarget");
   if(!div) return;
   div.innerHTML = CARDtagHtml();
   CARDupdateTag();
   MODELupdate('CARDmodel');
}

function CARDgetTags(){
   let TagInput = document.querySelector(".CARDtagEditInput").value || "";
   return TagInput.split(/\s+/);
}

function CARDisValidTag(Tag){
   // Have to include # for backward compatibility, but we don't advertise it in CARDdisplayTagError()
   return /^#[a-zA-Z0-9_#]{1,}$/.test(Tag);
}

function CARDhighlightBadTag(TagString, BadTag){
   let Start = TagString.indexOf(BadTag);
   let End = Start + BadTag.length;
   console.warn(Start, End);
   let TagInput = document.querySelector(".CARDtagEditInput");
   TagInput.setSelectionRange(Start, End);
   TagInput.focus();

}

function CARDdisplayTagError(){
   let TagInputError = document.querySelector(".CARDtagEditInputError");
   TagInputError.innerHTML = "Hashtags must start with '#' and only contain alphanumeric characters or '_'.";
}

function CARDclearTagError(){
   let TagInputError = document.querySelector(".CARDtagEditInputError");
   TagInputError.innerHTML = "";
}

function CARDeditValidateTags(){
   let TagString = document.querySelector(".CARDtagEditInput").value.trim();
   if (TagString === "") { return true; }    // clear all tags
   for (const Tag of TagString.split(/\s+/)) {
      if (CARDisValidTag(Tag)) { continue; }
      CARDhighlightBadTag(TagString, Tag);
      CARDdisplayTagError();
      return false;
   }
   CARDclearTagError();
   return true;
}

function CARDsaveTags(){
   if (!CARDeditValidateTags()) { return; }
   let ApiData = {};
   ApiData.component = CARDcomponentId();
   ApiData.value = CARDgetTags();
   APIcall("component/tags/set", ApiData, function(R){
      if(!R.success) return SNCKsnackbar("error", R.error);
      let Message = GINsetTags(`"${CARDdata().name}" component`, ApiData.value);
      GINaddAndCommit({message: Message, component: CARDcomponentId()});
      CARDviewTags();
      MODELforcePoll("CARDmodel");
      MODELforcePoll("DASHmodel");
   });
}

function CARDconfirmTagSave(){
   let ApiData = {};
   ApiData.tags = CARDgetTags();
   APIcall("role/test", ApiData, function(R){
      if(R.success) return CARDsaveTags();
      CARDconfirmTagSaveWindow(R.error);
   });
}

function CARDconfirmTagSaveWindowHtml(Error){
   return /*html*/`
   <div class="CARDconfirmModal">
      <div>Warning - performing this action will result in: </div>
      <br>
      <div>${Error}</div>
      <br>
      <br>
      <div class='DASHbuttonGroup BUTTONgroup'>
         <div class='CARDconfirmCancel BUTTONstandard BUTTONcancel'>CANCEL</div>
         <div class='CARDconfirm BUTTONstandard BUTTONaction'>CONFIRM</div>
      </div>
   </div>`;
}

function CARDconfirmTagSaveWindow(Error){
   CARDconfirmTag = POPpopoutOpen({
      title: 'Confirm Tag Edit',
      width: '550px',
      content: CARDconfirmTagSaveWindowHtml(Error)
   });
   let div = document.querySelector(".CARDconfirmModal");
   if(div) div.addEventListener("click", CARDconfirmModalClick);
}

function CARDconfirmModalClick(e){
   if(e.target.closest(".CARDconfirmCancel")) return POPpopoutClose(CARDconfirmTag);
   if(e.target.closest(".CARDconfirm")) {
      POPpopoutClose(CARDconfirmTag);
      CARDsaveTags();
   }
}

function CARDtagClick(target) {
   const hashtag = target.getAttribute('word')?.trim();
   if (!hashtag) return;
   const Tag = CARDcomponentRoleData()?.[hashtag];
   if (!Tag) return;
   ROLopenedTo = hashtag;
   window.location.hash = "#settings/roles";
}
