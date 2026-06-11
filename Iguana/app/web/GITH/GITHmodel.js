function GITHinitModel(){
   MODELpollerAdd('GITHmodel', 'GITHkeyData',     5,   'git/private_key_path');
   MODELpollerAdd('GITHmodel', 'GITHinstallData', 100, 'git/check_install');
   MODELpollerAdd('GITHmodel', 'GITHcredData',    100, 'git/credentials');
   MODELpollerAdd('GITHmodel', 'GITHgitServer',   100, 'git/server');
   MODELpollerAdd('GITHmodel', 'GITHgitUsername', 100, 'git/username');
   MODELpollerAdd('GITHmodel', 'GITHgitServers',  100, 'git/servers/load');
   MODELpollerAdd('GITHmodel', 'GITHkeyOverride', 100, 'git/key_type/override');
   MODELpollerAdd('GITHmodel', 'GITHowners',      100, 'git/owners');
   MODELviewAdd('GITHmodel', 'GITHupdateHidden');
   MODELviewAdd('GITHmodel', 'GITHupdateCreds');
   MODELviewAdd('GITHmodel', 'GITHupdateGitServer');
   MODELviewAdd('GITHmodel', 'GITHupdateUsername');
   MODELviewAdd('GITHmodel', 'GITHupdateWarning');
   MODELviewAdd('GITHmodel', 'GITHupdateInstalled');
   MODELviewAdd('GITHmodel', 'GITHupdateOrganizations');
   MODELviewAdd('GITHmodel', 'GITHupdateKeys');
   MODELforcePoll('GITHmodel');
   MODELstart('GITHmodel');
}

function GITHkeyData(){
   return MODELdata?.GITHkeyData?.data || {};
}

function GITHkeyOverride(){
   return MODELdata?.GITHkeyOverride?.data || "";
}

function GITHgitTokenValid(){
   return MODELdata?.GITHgitTokenValid?.data?.username || "";
}

function GITHgitUsername(){
   return MODELdata?.GITHgitUsername?.data?.username || "";
}

function GITHgitServers(){
   return MODELdata?.GITHgitServers?.data || {};
}

function GITHserverError(){
   return !MODELdata?.GITHgitServer?.success;
}

function GITHgitProvider(){
   return MODELdata?.GITHgitServer?.data?.service || "";
}

function GITHgitAddress(){
   return MODELdata?.GITHgitServer?.data?.address || "";
}

function GITHgitAddressLink(){
   let Address = MODELdata?.GITHgitServer?.data?.web || "";
   return Address;
}
function GITHcredentials(){
   return MODELdata?.GITHcredData?.data || {};
}

function GITHgitInstalled(){
   return MODELdata?.GITHinstallData?.data?.installed || false;
}
function GITHupdateValue(div, val){
   if(!div) return;
   if(div.innerHTML !== val) div.innerHTML = val;
}

function GITHowners(){
   return MODELdata?.GITHowners?.data || {};
}