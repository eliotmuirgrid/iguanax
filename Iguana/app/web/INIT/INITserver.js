function INITserver() {
   COL_TRC("Set up user screen.");
   MODELshutdownAll();
   INITserverModel(INITserverSetupStep); 
}

function INITserverSetupStep(){
   INITframe(INITserverHtml());
   INITsetBackBtn("user");
   INITserverListener();
   INITenableNext();
}

function INITserverModel(Callback) {
   COL_TRC("Setting up INITmodel model");
   MODELshutdownAll();
   MODELpollerAdd('INITserverModel', 'INITusers',     30,   'install/user/list', {});
   MODELviewAdd  ('INITserverModel', 'INITuserList');
   MODELforcePoll('INITserverModel', Callback);
}

function INITuserList(){
   let E = document.querySelector('.INITaccountUserSelect');
   if (!E){
      COL_TRC("This OS doesn't support selecting a user (yet?)");
      return;
   }
   let List = MODELdata.INITusers.data;
   for (let i=0; i < List.length;i++){
      const Option = document.createElement('option');
      Option.textContent = List[i];
      E.appendChild(Option);
   }
}

function INITserverListener() {
   COL_TRC("Set up the user select...");
   const UserSelect = document.querySelector('.INITaccountUserSelect');
   UserSelect.addEventListener('focus', function() { INITuserSelect(); });
   const PassSelect = document.querySelector('.INITaccountPassword');
   PassSelect.addEventListener('focus', function() { INITuserSelect(); INITclearServiceUserPasswordEmptyError(); });
   COL_TRC("Set up the finish button...");
   const AccountSubmit = document.querySelector('.INITnextBtn');
   AccountSubmit.addEventListener('click', function() { INITserverSubmit(); });
}

function INITuserSelect() {
   // auto-select 'Specific User'
   document.querySelector('#specificuser').checked = true;
}

function INITsetServiceUserPasswordEmptyError() {
   let PasswordInput = document.querySelector('.INITaccountPassword');
   PasswordInput.classList.add('INITinputError');
   PasswordInput.placeholder = 'Password required';
}

function INITclearServiceUserPasswordEmptyError() {
   let PasswordInput = document.querySelector('.INITaccountPassword');
   PasswordInput.classList.remove('INITinputError');
   PasswordInput.placeholder = 'Password';
}

function INITserverSubmit() {
   if (INITinstallMessageActive()) { return; }
   INIThideStatusDivs();
   let Username  = '';
   let Password  = '';
   let type = document.querySelector('input[name="INITserver"]:checked').value;
   if (type === 'specific') {
      Username = document.querySelector('.INITaccountUser').value;
      Password = document.querySelector('.INITaccountPassword').value.trim();
      if (!Password) {
         INITsetServiceUserPasswordEmptyError();
         return;
      }
   }
   INITdisableNext();
   INITinstallMessage(true);
   APIcall('install/server/install', {username: Username, password: Password, exe : INIT_INSTALL_PATH}, function(R){
      INITaccountHandleResponse(R);
   });
}

function INITinstallMessageActive(){
   let D = document.querySelector(".INITattemptingInstall");
   if(!D) return;
   return !D.classList.contains("INIThidden");
}

function INITinstallMessage(show){
   let D = document.querySelector(".INITattemptingInstall");
   if(!D) return;
   if( show &&  D.classList.contains("INIThidden")) D.classList.remove("INIThidden");
   if(!show && !D.classList.contains("INIThidden")) D.classList.add   ("INIThidden");
}

function INITaccountHandleResponse(R) {
   COL_TRC(R);
   INITenableNext();
   INITinstallMessage(false);
   if(!R.success) { return INITshowError(R.error); }
   INITshowSuccess("IguanaX service setup successful");
   COL_TRC("Move to the finish screen...");
   window.location.hash = "finish";
}

function INITserverHtml() {
   return `
   ${INITsidebarHtml()}
   <div class="INITmodalMainArea">
      <h2 class="INITmodalHeading">
         Server Setup
      </h2>
      <input class="INITinvisibleRadio" type="radio" id="radio1" name="INITappTypeSelection" value="local" checked>
      <label class="INITradio INITappType" for="radio1">
         <div class="INITappTypelabel">Run as a Service <span>(Iguana runs continuously in the background)</span></div>
         <form class="INITaccountForm">
            <div class="INITradioContainer">
               <input type="radio" id="systemaccount" name="INITserver" value="local" checked>
               <label class="INITradio" for="systemaccount">Local System Account</label>
            </div>
            <div class="INITradioContainer">
               <input type="radio" id="specificuser" name="INITserver" value="specific">
               <label class="INITradio" for="specificuser">Specific User</label>
               <div class="INITaccountFields">
                  <div class="INITformGroup">
                     <select class="INITaccountUserSelect INITaccountUser">
                     </select>
                  </div>
                  <div class="INITformGroup">
                     <input class="FORMinput INITaccountPassword" type="password" placeholder="Password">
                  </div>
               </div>
            </div>
         </form>
      </label>
      <div class="INIThidden INITattemptingInstall">
         <div class="INITinstall-text">Installing the IguanaX service and granting access to the Iguana data directory ...</div>
         <div class="INITspinner"></div>
      </div>
      ${INITcreateBtnGroupAndStatusDivs()}
   </div>`;
}