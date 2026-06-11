function CARDenableCurrentCommitEdit(){
   CARDdisableOpenEdit();
   CARDcommitEdit = true;
   MODELforcePoll('CARDcommitModel', function(){
      CARDrenderCommitEditHtml();
      MODELupdate('CARDmodel');
   });
}

function CARDnewCommitExists() {
   if (CARDgitPlain()) return false;
   const currentCommit = CARDcommit();
   if (!currentCommit) return false;
   const commits = CARDcommits();
   if (!commits?.length) return false;
   const latestHash = commits[0]?.hash;
   if (!latestHash) return false;
   return !CARDcommitsMatch(currentCommit, latestHash);
 }

function CARDrenderCommitEditHtml(){
   let Div = document.querySelector('.CARDgitEditCurrentCommitButton');
   if(Div) Div.classList.remove('CARDgitEditCurrentCommitButton');
   document.querySelector('.CARDrunningCommit').innerHTML = CARDcurrentCommitEditHtml();
}

function CARDcurrentCommitEditInputHtml(){
   return /*html*/`<div class="CARDcommitInputRowGrid CARDcommitInput FORMinputGroup">
      <input class="FORMinput CARDcommitInputValue" type="text" placeholder="Enter a commit id" value="${CARDcommit()}">
      <div class="CARDcommitIdHelp BUTTONstandard BUTTONcancel"><img src="CARD/CARDiconMoreOptions.svg"></div>
   </div>`;
}

function CARDcomponentListHtml(){
   let Components = CARDcomponents();
   let Html = "<datalist id='CARDcomponentList'>";
   for(let Component of Components){
      if(Component.guid === CARDcomponentId()) continue;
      Html += `<option value="${Component.guid}">${Component.name}</option>`;
   }
   return Html + "</datalist>";
}

function CARDcurrentPlainCommitOptionsHtml(){
   if(!CARDgitPlain()) return "";
   let Less = STORElocalRead('CARDlessVar') === "true";
   let LessClass = Less ? " CARDless" : "";
   return /*html*/`
   <div class='CARDhideShowRow${LessClass}'>
   <div class="CARDcommitInputRow"></div>
   <input class="CARDcommitComponentInput FORMinput" list="CARDcomponentList" type="text" placeholder="Enter a different component id to run from" value="${CARDrunComp()}">
   <a class="BUTTONdefault BUTTONstandard CARDrunFromComponentMoreInfo CARDcommitInputRow" title="More information on running from a different component." href="https://man.interfaceware.com/code/3257139203" target="_blank">ⓘ</a>
   <div></div>
   <div class="CARDshowHideGrid">
      <div>[</div>
      <div class="CARDshowMore">Show more options</div>
      <div class="CARDshowLess">Show less options</div>
      <div>]</div>
      <div></div>
      </div>
   </div>
   ${CARDcomponentListHtml()}
   `;
}

function CARDcurrentCommitEditHtml(){
   let PlainHtml = CARDcurrentPlainCommitOptionsHtml();
   return /*html*/`
   <div class='CARDgitRow'>
      <div class="FORMlabel">Using Commit:</div>
      ${CARDcurrentCommitEditInputHtml()}
      <div class="CARDcurrentActions BUTTONgroup CARDedit">
         <div class="CARDrefreshCurrentCommit BUTTONcancel BUTTONstandard" title="Update the git cache with the latest commits">
            <div class="CARDrefreshDiv"><img src="CARD/CARDiconRefresh.svg"></div>
         </div>
         <div class="CARDcancelCurrentCommit BUTTONcancel BUTTONstandard"><img src="CARD/CARDiconCancel.svg"></div>
         <div class="CARDsaveCurrentCommit BUTTONaction BUTTONstandard"><img src="CARD/CARDiconCheck.svg"></div>
      </div>
   </div>
   ${PlainHtml}
   `;
}

function CARDshowLess(){
   let Div = document.querySelector('.CARDhideShowRow');
   Div.classList.add('CARDless');
   STORElocalCreate('CARDlessVar', true);
}

function CARDshowMore(){
   let Div = document.querySelector('.CARDhideShowRow');
   Div.classList.remove('CARDless');
   STORElocalCreate('CARDlessVar', false);
}

function CARDrefreshCommits(){
   let Div = document.querySelector(".CARDrefreshDiv");
   let DivParent = Div.closest(".BUTTONcancel");
   if(Div) {
      Div.classList.add("CARDrefreshSpinner");
   }
   if(DivParent){
      DivParent.classList.add("BUTTONdisabled");
      DivParent.classList.remove("BUTTONaction");
      DivParent.title = "Pulling in the latest commits";
   }
   APIcall("component/fetch_repositories", {component:CARDcomponentId()}, function(R){
      CARDupdateCommitsClicked(CARDupdateCommitCallback);
   });
}

function CARDupdateCommitCallback(){
   CARDrenderCommitEditHtml();
   let Div = document.querySelector(".CARDrefreshDiv");
   let DivParent = Div.closest(".BUTTONcancel");
   if(Div) {
      Div.classList.remove("CARDrefreshSpinner");
   }
   if(DivParent){
      DivParent.classList.remove("BUTTONdisabled");
      DivParent.classList.add("BUTTONaction");
      DivParent.title = "Update the git cache with the latest commits";
   }
}

function CARDupdateCommitsClicked(callback){
   MODELforcePoll('CARDcommitModel', function(){
      MODELforcePoll('CARDmodel', function(){
         if(callback) callback();
      });
   });
}

function CARDcurrentCommitListener(T){
   if(T.targetArea('CARDcancelCurrentCommit')) CARDdisableCurrentCommitEdit();
   if(T.targetArea('CARDsaveCurrentCommit'))   CARDsaveCurrentCommit();
}

function CARDdisableCurrentCommitEdit(){
   CARDcommitEdit = false;
   MODELforcePoll('CARDmodel');
   MODELforcePoll('DASHmodel');
}

function CARDrunFromComponent(){
   return document.querySelector('.CARDcommitComponentInput')?.value || "";
}

function CARDgetCommitArgs(){
   let Commit = document.querySelector('.CARDcommitInputValue')?.value || "";
   let RunComp = CARDrunFromComponent();
   return {component: CARDcomponentId(), commit: Commit, run_comp: RunComp};
}

function CARDsaveCurrentCommit() {
   const Data = CARDgetCommitArgs();
   APIcall('component/set_commit', Data, function(Response) {
      if(!Response['success']) { return SNCKsnackbar('error', Response['error'], 6000);}
      let Message = GINsetCommit(Data.commit, CARDdata().name);
      GINaddAndCommit({message: Message, component: CARDcomponentId()});
      CARDdisableCurrentCommitEdit();
      let message = Data.commit !== 'DEVELOPMENT' ? `running on commit: ${Data.commit}` : `running in DEVELOPMENT mode`;
      let Warning = Response?.data?.warning;
      if(Warning) SNCKsnackbar('warning', "Switched to selected commit however there was a problem: " + Warning);
      else SNCKsnackbar('success', `Component ${message}`, 6000);
      // CARDfieldRender();
   });
}