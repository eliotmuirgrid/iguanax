let SETlastPage;

function SETsaveLastPage(){ SETlastPage = PAGEhash(); }

function SETformatUrl(){ return "#" + (SETlastPage || "settings"); }

function SETrender(){
   COL_TRC("SETrender");
   SETsaveLastPage();
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORsettingsIcon');
   SETmodelInit();
   if(document.querySelector('.SETmainContent')) return SETclearSelected();
   document.querySelector('.CORmain').innerHTML = SETsettingsHtml;
   if (typeof CORauthorizedPeersBadgeUpdate === "function") CORauthorizedPeersBadgeUpdate();
}

function SETclearSelected(){
   let Selected = document.querySelectorAll('.SETtitleLink.selected');
   for(let i=0; i < Selected.length; i++) Selected[i].classList.remove('selected');
}

// IX-3790 - turned off clustering
{/* <li class="SETsettingsNavPageLayout"><a class="SETtitleLink PERheader"        href="#settings/peers"         >Peers</a></li> */}

function SETsettingOptions(){
   return /*html*/`
   <div class="SETsettingsNav SETsettingsNavLayout SETcontentAsideLayout">
      <ul>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STAabout"         href="#settings"               >About</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink AUTHheader"       href="#settings/authentication">Authentication</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink AUPauthorizedPeersNav" href="#settings/authorized_peers">Authorized Peers</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STCcollection"    href="#settings/collections"   >Collections</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STDdatabase"      href="#settings/database"      >Database</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink ENVenvironment"   href="#settings/environment"   >Environment</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink GITHgitHost"      href="#settings/git"           >Git</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink GSVheader"        href="#settings/git_servers"   >Git Servers</a></li>` +
         // <li class="SETsettingsNavPageLayout"><a class="SETtitleLink HAheader"         href="#settings/high_availability">High Availability</a></li> IX-4335 disabling for release
         `<li class="SETsettingsNavPageLayout"><a class="SETtitleLink SLBheader"        href="#settings/libraries"     >Library Usage</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STLlicense"       href="#settings/license"       >License</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink SLClogConfig"     href="#settings/log_config"    >Log Encryption</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STGpurging"       href="#settings/purging"       >Log Purging</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STGlogging"       href="#settings/storage"       >Log Storage</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink GINheader"        href="#settings/instance"      >Manage Instance</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STNnotifications" href="#settings/notifications" >Notifications</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink ROLroles"         href="#settings/roles"         >Roles</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink SANDaccess"       href="#settings/security"      >Security</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink USERusers"        href="#settings/users"         >Users</a></li>
         <li class="SETsettingsNavPageLayout"><a class="SETtitleLink STSserver"        href="#settings/webserver"     >Web Server</a></li>
      </ul>
   </div>`;
}

const SETsettingsHtml =/*html*/`
<div class="SETmainContent SETmainContentLayout">
   <div class="SETcontentAside">
      <div class="SETcontentAsideHeader">
         <div class="SETheading">Settings</div>
      </div>
      ${SETsettingOptions()}
   </div>
   <div class="SETcontentBody SETcontentBodyLayout"></div>
</div>`;

function SETmodelInit(){
   MODELpollerAdd('SETmodel', 'SETwarningData', 2, 'notifications/enabled', {});
   MODELforcePoll('SETmodel');
   MODELstart    ('SETmodel');
}

function SETemptyList(text, data) {
   return data ? '' :  `<div class="SETemptyTable">No ${text} - click EDIT to create some</div>`;
}

function SETappendTo(element, htmlString) {
   var tempElement = document.createElement('div');
   tempElement.innerHTML = htmlString;
   while (tempElement.firstChild) element.appendChild(tempElement.firstChild);
}

function SETfocusLast(Selector){
   let Items = document.querySelectorAll(Selector);
   let lastItem = Items[Items.length -1];
   FORMfocus(lastItem);
}

function SETresetToDefaultHtml(Message, ClassName){
   return /*html*/`
   <div class="SETresetGrid">
   <div></div>
   <div>[</div>
   <div class="SETdefaultLink ${ClassName}">${Message}</div>
   <div>]</div>
   </div>`;
}
