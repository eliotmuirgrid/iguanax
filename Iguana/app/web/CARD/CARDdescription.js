function CARDdescriptionViewHtml() {
   let Description = ESChtmlEscape(CARDdescription());
   const Perm = CARDpermissionData()?.edit_description;
   const Class = Perm ? 'CARDdescription' : 'CARDdescription CARDdisableLink';
   return /*html*/ `
   <div class="${Class}" value="${Description}">${HASHTAGrenderHashtags(Description)}</div>
   <div class="CARDdescriptionEditButton CARDeditBtn"><img src="CARD/CARDiconMoreOptions.svg"></div>`;
}

function CARDdescriptionHtml(){
   return CARDdescriptionWithEditHtml();
   if(!CARDcomponentOn()) return CARDdescriptionWithEditHtml();
   else                   return CARDdescriptionNoEditHtml();
}

function CARDdescriptionWithEditHtml(){
   let Description = ESChtmlEscape(CARDdescription());
   const Perm = CARDpermissionData()?.edit_description;
   const Class = Perm ? 'CARDdescription' : 'CARDdescription CARDdisableLink';
   if(Description) return CARDdescriptionViewHtml();
   else            return /*html*/ `<div class="${Class} CARDaddDescription" value="">+ Add a component description +</div>`;
}

function CARDdescriptionNoEditHtml(){
   let Description = ESChtmlEscape(CARDdescription());
   if(Description) return /*html*/ `<div class="CARDdescription CARDdescriptionNoEdit" value="${Description}">${HASHTAGrenderHashtags(Description)}</div>`;
   else            return /*html*/ `<div class="CARDdescription CARDdescriptionNoEdit CARDblankDescription" value=""></div>`;
}

function CARDdescriptionEditHtml() {
   let Description = ESChtmlEscape(CARDdescription());
   return /*html*/`
   <textarea class="CARDdescriptionEditInput FORMinput" placeholder="What is the purpose of this component?">${Description}</textarea>
   <div class="CARDlabelActions BUTTONgroup CARDedit">
      <div class="CARDcancelLabel BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
      <div class="CARDsaveDescription BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
   </div>`;
}

function CARDupdateDescriptionView(){
   if(CARDdescriptionEdit) return;
   let Description = CARDdescription();
   let Div = document.querySelector(".CARDdescription");
   if ( (Div === null) || ( Description !== Div.getAttribute('value') ) ){ 
      return document.querySelector('.CARDdescriptionTarget').innerHTML = CARDdescriptionHtml(); 
   }
}

function CARDenableDescriptionEdit(){
   CARDdisableOpenEdit();
   CARDdescriptionEdit = true;
   document.querySelector('.CARDdescriptionTarget').innerHTML = CARDdescriptionEditHtml();
   CARDfocusAtTheEnd(document.querySelector('.CARDdescriptionEditInput'));
   MODELupdate('CARDmodel');
}

function CARDdisableDescriptionEdit(){
   CARDdescriptionEdit = false;
   MODELforcePoll('CARDmodel');
   MODELforcePoll('DASHmodel');
}

function CARDsaveDescription() {
   const Data = { component: CARDcomponentId(), value: decodeURI(encodeURI(document.querySelector('.CARDdescriptionEditInput').value))};
   APIcall('component/description/set', Data, function(Response){
      if(!Response.success) {
         if (Response.error.search(/permission to change/)){ return SNCKsnackbar("error", Response.error + " <a href='/#logs?tags=%23error&filter=true'>View error in Logs</a>", 10000, false); }
         else                                              { return SNCKsnackbar("error", "Problem setting the component description: " + Response.error); }  
      }
      let Message = GINsaveComponentDescription(CARDdata().name);
      GINaddAndCommit({message: Message, component: CARDcomponentId()});
      CARDdisableDescriptionEdit();
      SNCKsnackbar('success', 'Component description updated.', 6000);
   });
}

function CARDdescriptionListener(T){
   if(T.targetArea('CARDcancelLabel')){ CARDdisableDescriptionEdit(); }
   if(T.targetArea('CARDsaveDescription')){ CARDsaveDescription(); }
   if(T.targetArea('CARDdescriptionEditButton') || T.targetArea('CARDaddDescription')){ CARDenableDescriptionEdit(); }
}