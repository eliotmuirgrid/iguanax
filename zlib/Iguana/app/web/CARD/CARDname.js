function CARDnameViewHtml(){
   return /*html*/`
   <div class="CARDnameBlock">
      <div class="CARDname">${CARDname()}</div>
      <div class="CARDeditBtn CARDeditName">
         <img src="CARD/CARDiconMoreOptions.svg">
      </div>
      ${CARDeditConnectionsHtml()}
      ${CARDarrowsHtml()}
   </div>`;
}

function CARDeditConnectionsHtml(){
   return /*html*/`<a class="CARDviewConn BUTTONstandard BUTTONdefault" href="#dashboard/connections?component=${CARDcomponentId()}"><div>CONNECTIONS</div></a>`;
}

function CARDnameEditHtml() {
   return /*html*/ `
   <div class='CARDnameBlockEdit'>
      <input class="CARDnameEditInput FORMinput" placeholder="Name your component" value="${CARDname()}">
      <div class="CARDnameActions BUTTONgroup CARDedit">
         <div class="CARDcancelName BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
         <div class="CARDsaveName BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
      </div>
   </div>
   `;
}
function CARDenableNameEdit() {
   let NameEdit = CARDpermissionData()?.edit_name;
   if(!NameEdit) return;
   CARDdisableOpenEdit();
   CARDnameEdit = true;
   document.querySelector('.CARDnameTarget').innerHTML = CARDnameEditHtml();
   CARDfocusAtTheEnd(document.querySelector('.CARDnameEditInput'));
   MODELupdate('CARDmodel');
}

function CARDsaveName() {
   let NewName = document.querySelector('.CARDnameEditInput').value.trim();
   const Data = {component: CARDcomponentId(), value: NewName};
   APIcall('component/name/set', Data, function(Response) {
      if(!Response.success) {
         if (Response.error.search(/permission to rename/)){ return SNCKsnackbar("error",  Response.error + " <a href='/#logs?tags=%23error&filter=true'>View error in Logs</a>", 10000, false); }
         else                                              { return SNCKsnackbar("error", "Problem saving the component name: " + Response.error); }  
      }
      let OldName = CARDdata().name;
      let Message = GINsaveComponentName(OldName, NewName);
      GINaddAndCommit({message: Message, component: CARDcomponentId()});
      CARDdisableNameEdit();
      SNCKsnackbar('success', 'Component name updated.', 6000);
   });
}

function CARDdisableNameEdit(){
   CARDnameEdit = false;
   MODELforcePoll('CARDmodel');
   MODELforcePoll('DASHmodel');
}

function CARDupdateNameView(){
   if(CARDnameEdit) return;
   let NameHtml = CARDnameViewHtml();
   if (document.querySelector(".CARDname") === null || document.querySelector('.CARDname').innerHTML !== CARDname() ){
      document.querySelector('.CARDnameTarget').innerHTML = NameHtml;
   }
}

function CARDnameListener(T){
   if((T.targetArea('CARDname') || T.targetArea('CARDeditName')) ) CARDenableNameEdit();
   if(T.targetArea('CARDcancelName'))  CARDdisableNameEdit();
   if(T.targetArea('CARDsaveName'))    CARDsaveName();
}