function USERmodelInit(){
   MODELpollerAdd('USERmodel', 'USERpermission', 3, 'component/permission', {});
   MODELpollerAdd('USERmodel', 'USERdataModel',  3, 'user/list', {});
   MODELpollerAdd('USERmodel', 'USERcurrentUser', 3, 'user/whoami', {}); 
   MODELpollerAdd('USERmodel', 'USERroleData',   3, 'role/list', {});
   MODELviewAdd  ('USERmodel', 'USERview');
   MODELforcePoll('USERmodel');
   MODELstart    ('USERmodel');
}

function USERroleData()       { return MODELdata?.USERroleData?.data || {}; }
function USEReditPermission() { return MODELdata?.USERpermission?.data?.edit_users || false; }
function USERcurrentUser()    { return MODELdata?.USERcurrentUser?.data || null; }

function USERview() {
   let Target      = document.querySelector(".SETcontentBody");
   let UserData    = MODELdata.USERdataModel.data;
   let CurrentUser = USERcurrentUser();
   let Html        = USERviewHtml(Object.values(UserData), CurrentUser);
   if(Html !== Target.innerHTML) {
      Target.innerHTML = Html;
      USERlisteners();
   }
}

function USERdata(){
   return MODELdata?.USERdataModel?.data || {};
}

function USERviewHtml(UserList, CurrentUser) {
   let FilteredUserList = UserList;
   if(CurrentUser) { FilteredUserList = UserList.filter(user => user.username !== CurrentUser.username); }
   let OrderedUserList = [];
   if(CurrentUser) { OrderedUserList.push(CurrentUser); }
   OrderedUserList = OrderedUserList.concat(FilteredUserList);
   USERlistCache   = OrderedUserList;
   
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane USERmodal">
      <div class="SETusersTitle">
         <div class="SETusersLabel">Users</div>
      ${USEReditButton()}
      </div>
      <div class="SETpageDescription">
      Create, modify and remove user accounts. [<a href="${LINKsettingsUsersHeader}" target="_blank">Learn more</a>]</div>
      <div class="USERtable">
         <div class="SETuserHeading">
            <div class="SETuserTableHeading USERtableHeading">Username</div>
            <div class="SETuserTableHeading USERtableHeading">Roles</div>
            <div class="SETuserTableHeading USERtableHeading">Provider</div>
            <div class="SETuserTableHeading USERtableHeading"></div>
         </div>
         ${OrderedUserList.map((user, index) => {return USERrowHtml(user, index);}).join('')}
      </div>
      <div class="SETtotalUsers">TOTAL USERS: ${UserList.length}</div>
   </div>`;
};
