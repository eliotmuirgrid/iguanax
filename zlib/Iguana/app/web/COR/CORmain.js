function CORheader() {
   let H = document.querySelector(".CORheader");
   if (H.innerHTML.length == 0) {
      COL_TRC("Setting up header HTML for init.");
      H.innerHTML = CORheaderHtml();
   } else {
      COL_TRC("Header HTML already set up.");
   }
   CORmodelInit();
   CORmodelSlowInit();
   CORmodelPushInit();
   CORsetListeners();
}

function CORlogoutAction(){
   COL_TRC("Set click handler for logout user.");
   let E = document.querySelector(".CORheaderLogoutUser");
   if (E) {
      E.addEventListener('click', SESSlogout);
   }
}

function CORsetNavIcon(IconClass){
   COL_TRC(IconClass);
   let List = document.querySelectorAll(".CORheaderNavLinkStyle");
   for (let i=0; i < List.length; i++){
      COL_TRC(i, List[i]);
      List[i].classList.remove('selected');
   }
   // This let's Eliot cheat and access the cluster page on tunnelled servers.  This got broke
   // when we switched to the close icon.
   let E = document.querySelector("." + IconClass);
   if (E) {
      E.classList.add('selected');
   }
}

function CORrouteRedirect(Hash) {
   if (window.location.hash === Hash) return;
   window.location.hash = Hash;
}

function CORnotifyRouteBlocked(Message) {
   SNCKsnackbar('warning', Message, 3500, false);
}

function CORrouteCommandGuard(Action) {
   return Action();
}

function CORrouteAuthorizedPeersGuard(Action) {
   return Action();
}

// TODO make the names of screen functions consistent
function CORpageTable() {
   COL_TRC("Set up page routing");
   PAGEstart({
      ''                           : function() { DASHcloseModal(); },
      'command'                    : function() { CORrouteCommandGuard(COMDscreen); },
      'command/access'             : function() { CORrouteCommandGuard(COMDaccessScreen); },
      'command/details'            : function() { CORrouteCommandGuard(COMDdetailsScreen); },
      'command/connect'            : function() { CORrouteCommandGuard(COMDconnectScreen); },
      'command/add'                : function() { CORrouteCommandGuard(COMDaddPeerScreen); },
      'upgrade_special'            : function() { UPGRADEupgradeOld(); },
      'dashboard'                  : function() { DASHdashboardScreen(); },
      'dashboard/new'              : function() { NWCnewComponentScreen(); },
      'dashboard/new/url'          : function() { NRLnewComponentUrlScreen(); },
      'dashboard/details'          : function() { CARDscreen(); },
      'dashboard/set_repo'         : function() { STRrender(); },
      'dashboard/set_branch'       : function() { BRNCrender(); },
      'dashboard/reposition_queue' : function() { REPQscreen(); },
      'dashboard/connections'      : function() { ECNscreen(); },
      'dev'                        : function() { DEVscreen(); },
      'logs'                       : function() { LOGscreen(); },
      'logs/details'               : function() { DETdetailScreen(); },
      'logs/related'               : function() { RELrender(); },
      'logs/download'              : function() { DETdetailScreen(); },
      'log_generator'              : function() { LGGgeneratorScreen(); },
      'settings'                   : function() { STAaboutScreen(); },
      'settings/authentication'    : function() { AUTHview(); },
      'settings/authentication/edit': function() { AUTHedit(); },
      'settings/authorized_peers'  : function() {
         CORrouteAuthorizedPeersGuard(function() { AUPscreen(true); });
      },
      'settings/authorized_peers/access': function() {
         CORrouteAuthorizedPeersGuard(AUPaccessScreen);
      },
      'settings/authorized_peers/add': function() {
         CORrouteAuthorizedPeersGuard(AUPaddPeerScreen);
      },
      'settings/authorized_peers/details': function() {
         CORrouteAuthorizedPeersGuard(AUPdetailsScreen);
      },
      'settings/authorized_peers/connect': function() {
         CORrouteAuthorizedPeersGuard(AUPconnectScreen);
      },
      'settings/users'             : function() { USERscreen(); },
      "settings/notifications"     : function() { STNview(); },
      "settings/notifications/edit": function() { STNedit(); },
      "settings/log_config"        : function() { SLCview(); },
      "settings/log_config/edit"   : function() { SLCedit(); },
      "settings/storage"           : function() { STUrender(); },
      "settings/purging"           : function() { STGview(); },
      "settings/purging/edit"      : function() { STGedit(); },
      "settings/database"          : function() { STDdatabaseScreen(); },
      "settings/libraries"         : function() { SLBview(); },
      "settings/license"           : function() { STLlicenseScreen(); },
      "settings/environment"       : function() { ENVscreen(); },
      "settings/collections"       : function() { STCcollectionScreen(); },
      "settings/git"               : function() { GITHbitbucketSetupScreen(); },
      // 'settings/high_availability' : function() { HAview(); },  IX-4335 disabling for release
      // 'settings/high_availability/edit' : function() { HAedit(); },
      "settings/instance"          : function() { GINview(); },
      "settings/git_servers"       : function() { GSVview(); },
      "settings/git_servers/edit"  : function() { GSVedit(); },
      // IX-3790 - turned off clustering
      // 'settings/peers'             : function() { PERview(); },
      // 'settings/peers/add'         : function() { PERadd(); },
      // 'settings/peers/delete'      : function() { PERdelete(); },
      // 'settings/peers/myinfo'      : function() { PERmyInfo(); },
      // 'settings/peers/details'     : function() { PERdetails(); },
      'settings/roles'             : function() { ROLrender(); },
      'settings/roles/add'         : function() { ROLaddRender(); },
      'settings/roles/edit'        : function() { ROLeditRender(); },
      'settings/roles/remove'      : function() { ROLremoveRender(); },
      "settings/security"          : function() { SECview(); },
      "settings/security/edit"     : function() { SECedit(); },
      'settings/update'            : function() { UPGRADEupgrade(); },
      "settings/webserver"         : function() { STSserverScreen(); },
      "settings/webserver/edit"    : function() { STSserverEditScreen(); },
      'translator'                 : function() { TRANSrender(); },
      'unittests'                  : function() { UNITtestScreen(); },
      //'quill'                      : function() { QUILLdemoScreen(); },
      //'websockets'                 : function() { APIwebsocketScreen();},
      'documents'                  : function() { DOCmainScreen();},
      'documents2'                 : function() { DOCmain2Screen();},
      'error'                      : function() { ERRerror();}
   });
}

function CORpageTableLoggedOut() {
   COL_TRC("Set up page routing");
   PAGEstart({'': function() { SESSlogout(); }});
}

document.addEventListener('DOMContentLoaded', function() {
   if (PAGEhashParameter("trace")){
      COLtrace(PAGEhashParameter("trace"));  // Setting a hash page parameter will enable tracing.
   }
   CORmodeInit();
});

function CORmodeInit(){
   APIcall("mode", {}, CORonMode);
}

function CORonMode(R){
   if (!R.success){
      COL_ERR("Could not connect to IguanaX backend.", R.error);
      return;
   }
   let Mode = R.data.mode;
   COL_TRC("The mode is", Mode);
   if(Mode !== "log_unlock") FILsepUpdate();
   if (Mode === "startup"){
      INITsetup();
   } else if (Mode == 'initialization'){
      STARTUPsetup();
   } else if (Mode == 'log_unlock'){
      ENClogUnlockSetup();
   } else {
      COL_TRC("Start up normal mode.");
      CORnormalInit();
      // IX-3790 - turned off clustering
      // PEERinit();
   }
}

function CORnormalInit(){
   COL_TRC("Initialize the normal page handing system.");
   MODELshutdownAll(); // TODO models need to be cleared when we change pages
   SESSinit();
}

function CORpreparePage(){
   let body = document.body;
   if(!body.classList.contains('CORapp')) {
      COL_TRC("Add CORapp class to body.");
      body.classList.add('CORapp');
   }
   let PageSet = document.querySelector('.CORmain');
   if(!PageSet) {
      COL_TRC("Setting up CORheader and CORmain");
      body.innerHTML = /*html*/`<div class="CORheader"></div><div class="CORmain"></div>`;
   } else {
      COL_TRC("CORheader and CORmain already set.");
   }
   POPpopoutRemoveAll();
   // CORthemeCheckAndApply();
}

function CORrefresh() { location.reload(true); }
