let INITconfigCache = {};

function INITconfig(){
   COL_TRC("Initialize config screen");
   MODELshutdownAll();
   INITframe(INITconfigHtml());
   INITsetBackBtn("location");
   INITgetGitConfigs();
   INITconfigListener();
   INITenableNext();
}

function INITgetGitConfigs() {
   APIcall('install/config/get', {}, (R) => {
      let GitEmail   = document.querySelector('#INITgitEmail')
      let GitUser    = document.querySelector('#INITgitUser');
      let ServerName = document.querySelector('#INITserverName');
      GitUser.value  = R.data.name;
      GitEmail.value = R.data.email;
      // Overwrite default values with cached values
      if (INITconfigCache.server_name) { ServerName.value = INITconfigCache.server_name; }
      if (INITconfigCache.email)       { GitEmail.value   = INITconfigCache.email;       }
      if (INITconfigCache.user)        { GitUser.value    = INITconfigCache.user;        }
   });
}

function INITsaveServerNameConfig() {
   INITconfigCache.server_name = document.querySelector('#INITserverName').value.trim();
}

function INITsaveGitUserConfig() {
   INITconfigCache.user = document.querySelector('#INITgitUser').value.trim();
}

function INITsaveGitEmailConfig() {
   INITconfigCache.email = document.querySelector('#INITgitEmail').value.trim();
}

function INITconfigListener(){
   const ConfigSubmit = document.querySelector('.INITnextBtn');
   ConfigSubmit.addEventListener('click', function() { INITconfigSubmit(); });
   document.querySelector('#INITserverName').addEventListener('focus',  function() { INITclearError('#INITserverName', 'e.g. iNTERFACEWARE IguanaX Build Server'); });
   document.querySelector('#INITgitUser')   .addEventListener('focus',  function() { INITclearError('#INITgitUser'); });
   document.querySelector('#INITgitEmail')  .addEventListener('focus',  function() { INITclearError('#INITgitEmail'); });
   document.querySelector('#INITserverName').addEventListener('change', INITsaveServerNameConfig);
   document.querySelector('#INITgitUser')   .addEventListener('change', INITsaveGitUserConfig);
   document.querySelector('#INITgitEmail')  .addEventListener('change', INITsaveGitEmailConfig);
}

function INITconfigSubmit() {
   if (INITsaveMessageActive()) { return; }
   INIThideStatusDivs();
   let Name = document.querySelector('#INITserverName').value.trim();
   if (!Name) { return INITsetError('#INITserverName', 'Server name required'); }
   let User = document.querySelector('#INITgitUser').value.trim();
   if (!User) { return INITsetError('#INITgitUser', 'User required'); }
   let Email = document.querySelector('#INITgitEmail').value.trim();
   if (!Email) { return INITsetError('#INITgitEmail', 'Email required'); }
   INITdisableNext();
   INITsaveMessage(true);
   APIcall('install/config/set', {name: User, email: Email, server_name: Name}, function(R){
      INITconfigHandleResponse(R);
   });
}

function INITconfigHandleResponse(R) {
   COL_TRC(R);
   INITenableNext();
   INITsaveMessage(false);
   if(!R.success) { return INITshowError(R.error); }
   INITshowSuccess("IguanaX service setup successful");
   COL_TRC("Move to the finish screen...");
   window.location.hash = "user";
}

function INITsaveMessageActive(){
   let D = document.querySelector(".INITsavingConfig");
   if(!D) return;
   return !D.classList.contains("INIThidden");
}

function INITsaveMessage(show){
   let D = document.querySelector(".INITsavingConfig");
   if(!D) return;
   if( show &&  D.classList.contains("INIThidden")) D.classList.remove("INIThidden");
   if(!show && !D.classList.contains("INIThidden")) D.classList.add   ("INIThidden");
}

function INITsetError(Selector, ErrMsg) {
   let Input = document.querySelector(Selector);
   Input.classList.add('INITconfigError');
   Input.placeholder = ErrMsg;
}

function INITclearError(Selector, DefaultMsg = 'Required') {
   let User = document.querySelector(Selector);
   User.classList.remove('INITconfigError');
   User.placeholder = DefaultMsg;
}

function INITconfigHtml() {
   COL_TRC("Git config screen HTML");
   return `
   ${INITsidebarHtml()}
   <div class="INITmodalMainArea">
      <h2 class="INITmodalHeading">
         Configure
      </h2>
      <p>Choose a unique name for this server:</p>
      <div class="INITsignupFormContainer">
         <div class="INITformGroup">
            <input class="FORMinput" type="text" id="INITserverName" placeholder="e.g. iNTERFACEWARE IguanaX Build Server">
         </div>
      </div>
      <p>Set Git user and email for committing changes in the Translator:</p>
      <div class="INITsignupFormContainer">
         <form class="INITsignupForm">
            <div class="INITformGroup">
               <div class="INITuserLabelRow">
                  <label class="FORMlabel" for="INITgitUser">User:</label>
               </div>
               <input class="FORMinput" type="text" id="INITgitUser" name="username" placeholder="Required">
            </div>
            <div class="INITformGroup">
               <div class="INITuserLabelRow">
                  <label class="FORMlabel" for="INITgitEmail">Email:</label>
               </div>
               <input class="FORMinput" type="email" id="INITgitEmail" name="password" placeholder="Required">
            </div>
         </form>
      </div>
      <div class="INIThidden INITsavingConfig">
         <div class="INITinstall-text">Configuring IguanaX server name and GIT ...</div>
         <div class="INITspinner"></div>
      </div>
      ${INITcreateBtnGroupAndStatusDivs()}
   </div>`;
}
