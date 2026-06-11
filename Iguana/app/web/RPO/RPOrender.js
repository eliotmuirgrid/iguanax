// let Params = {}
// Params.title = "Create a new collection";
// Params.comment = "blah blah";
// Params.git = 'git@bitbucket.org:interfaceware/custom';
// Params.onCancel = APPcancelCallback; 
// Params.onSubmit = APPsubmitCallback;
// Params.onSubmitCustom = APPsubmitCustomCallback;

let RPOcard;
let RPOonSubmit;
let RPOonCancel;
let RPOallowConnect;
let RPOdefaultCustomValue;
let RPOpreCondition;

function RPOcreateRepoDialog(Params){
   let BannerContent = Params?.content || "";
   RPOallowConnect = Params?.allowConnect ? true : false;
   let Args = {};
   Args.title    = Params?.title || "Connect a repository";
   Args.content  = RPOcheckingGitCloudRequirementsHtml(BannerContent);
   Args.width    = "800px";
   Args.className = 'RPOconfigWindow';
   RPOonCancel = Params?.onCancel;
   Args.callback = RPOclose;
   RPOonSubmit   = Params?.onSubmit;
   RPOdefaultCustomValue = Params?.defaultCustomValue || "";
   RPOpreCondition = Params?.precondition;
   RPOcard = POPpopoutOpen(Args);
   APIcall("git/check_install", {}, function(R) {
      let GitIssue = CORgitSshProblem(R?.data, false);
      if(GitIssue){
         let Div = document.querySelector(".RPOmodal");
         Div.innerHTML = RPOsetupGitHtml();
      }
      else{
         RPOmodelInit();
      }
   });
   RPOlisteners();
}

function RPOclose(){
   MODELremove("RPOmodel");
   RPOmidOnSubmitRequest = false;
   if(RPOonCancel) RPOonCancel();
   POPpopoutClose(RPOcard);
}

function RPOsetupGitHtml(){
   return /*html*/ `
   <div class="RPOredirectContainer">To proceed you need to have Git and SSH setup.</div>
   <br>
   <br>
   <div class="BUTTONgroup RPObuttons">
      <div class="RPOcancel BUTTONdefault BUTTONstandard">CANCEL</div>
      <div class="RPOredirectGit RPOredirectBtn BUTTONaction BUTTONstandard"><a href="#settings">SETUP</a></div>
   </div>`;
}

function RPOcheckingGitCloudRequirementsHtml(BannerContent){
   return /*html*/`
   <div class="RPOlistenerDiv">
      ${BannerContent}
      <div class="RPOmodal">
         <div>Checking that git is installed and is properly configured...</div>
      </div>
   </div>`;
}

function RPOmustSetupCloudCredentialsHtml(){
   return /*html*/`
   <div class="RPOredirectContainer">To proceed, it is essential that a Git Server token is set and can be authenticated.</div>
   <br>
   <br>
   <div class="BUTTONgroup RPObuttons">
      <div class="RPOtryAgain BUTTONdefault BUTTONstandard">TRY AGAIN</div>
      <div class="RPOredirectBitbucket RPOredirectBtn BUTTONaction BUTTONstandard"><a href="#settings/git">SET MY GIT</a></div>
   </div>`;
}

function RPOcloudFormHtml(){
   return /*html*/`
      <div class='RPOrow'>
         <div class="RPOsubheader">HOST :</div>
         <select disabled class="RPOhost FORMselectBox">
         </select>
      </div>
      <br>
      <div class='RPOrow'>
         <div class="RPOsubheader">OWNER :</div>
         <select class="RPOworkspaces FORMselectBox"></select>
         <div class="RPOupdateOwners BUTTONdefault BUTTONstandard">
            <div class="RPOupdateOwnersBtn"></div>
         </div>
      </div>
      <div class='RPOrow'>
         <div></div>
         <div class="RPOworkspaceFeedback"></div>
      </div>
      <br>
      <div class='RPOrow'>
         <div class="RPOsubheader">REPOSITORY :</div>
         <input class="RPOslug FORMinput" type="text" placeholder="repository-name">
      </div>
      <div class='RPOrow'>
         <div></div>
         <div class="RPOslugFeedback"></div>
      </div>
      <br>
      <div class='RPOrow'>
         <div class="RPOsubheader">GIT URL :</div>
         <input disabled class="FORMinput RPOgitUrl">
      </div>
      <div class='RPOrow'>
         <div></div>
         <div class="RPOfeedback"></div>
      </div>
      <br>
      <div class='RPOredirectHosts'>
         <div></div>
      </div>
      <br>
      <div class="BUTTONgroup RPObuttonsRepoSet">
         <div class="RPOcancel BUTTONstandard BUTTONdefault">CANCEL</div>
         <div class="RPOconfirm BUTTONstandard BUTTONdisabled">CONFIRM</div>
      </div>`;
}

function RPOhtml(){
   return /*html*/`
   <div class="RPOcontainingDiv">
      <div class="RPOheader">CREATE REPOSITORY:</div>
      <br>
      <div class="RPOsetupstreamDialog">
         <div class="RPOcomponentDiv RPOcloudDiv">
            ${RPOcloudFormHtml()}
         </div>
      </div>
      <br>
      <div class="RPOheader">EXISTING REPOSITORY:</div>
      <br>
      <div class="RPOcomponentDiv RPOcustom">
         <div class='RPOrow'>
            <div class="RPOsubheader">CUSTOM URL :</div>
            <input class="FORMinput RPOgitUrlCustom" value="${RPOdefaultCustomValue}">
         </div>
         <div class='RPOrow'>
            <div></div>
            <div class="RPOfeedbackCustom">${RPOallowConnect ? "" : "The git url you provide must be for an empty repository without any contents in it."}</div>
         </div>
         <br>
         <div class="BUTTONgroup RPObuttonsRepoSet">
            <div class="RPOcancel BUTTONstandard BUTTONdefault">CANCEL</div>
            <div class="RPOconfirmCustom BUTTONstandard BUTTONdisabled">CONNECT</div>
         </div>
      </div>
      <br>
   </div>`;
}