function ROLeditRender(){
   SETrender();
   document.querySelector('.ROLroles').classList.add('selected');
   let Role = PAGEhashParameter("role");
   APIcall("role/query", { name : Role }, function(R){
      if(!R?.success) return window.location = "/#settings/roles";
      let Data = R?.data;
      ROLopenedTo = Role;
      document.querySelector('.SETcontentBody').innerHTML = ROLeditHtml(Data);
      ROLlisteners();
   });
}

function ROLcheckbox(Checked){
   return /*html*/`<input type="checkbox" ${Checked ? "checked" : ""}>`;
}
function ROLeditHtml(Data){
   let Role = PAGEhashParameter("role");
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane ROLmodal">
      <div class="ROLtitle">
         <div>Roles</div>
         <div class="ROLgroup BUTTONgroup">
            <div class="ROLsave BUTTONaction BUTTONstandard">SAVE</div>
            <a href="/#settings/roles"class="BUTTONcancel BUTTONstandard" >CANCEL</a>
         </div>
      </div>
      <div class="ROLcontent">${ROLcardHtml(Data, Role, true)}
      </div>
   </div>`;
}

function ROLroleData() {
   let Data                 = {};
   // Component Operations
   Data.start               = ROLvar(".ROLstart");
   Data.stop                = ROLvar(".ROLstop");
   Data.reposition_queue    = ROLvar(".ROLrepositionQueue");
   Data.clear_queue         = ROLvar(".ROLclearQueue");
   Data.clear_errors        = ROLvar(".ROLclearErrors");
   Data.delete_component    = ROLvar(".ROLdeleteComponent");
   // Component Logs
   Data.view_logs           = ROLvar(".ROLviewLogs");
   Data.resubmit_message    = ROLvar(".ROLresubmitMessage");
   Data.export_sample_data  = ROLvar(".ROLexportSample");
   Data.download_logs       = ROLvar(".ROLdownloadLogs");
   Data.delete_logs         = ROLvar(".ROLdeleteLogs");
   // Component Configuration
   Data.edit_name           = ROLvar(".ROLeditName");
   Data.edit_description    = ROLvar(".ROLeditDescription");
   Data.edit_commit         = ROLvar(".ROLchangeCommit");
   Data.edit_tags           = ROLvar(".ROLeditTags");
   // Component Development
   Data.view_script         = ROLvar(".ROLviewScript");
   Data.edit_script         = ROLvar(".ROLeditScript");
   Data.edit_upstream       = ROLvar(".ROLchangeUpstream");
   Data.edit_connections    = ROLvar(".ROLeditConnections");
   Data.edit_fields         = ROLvar(".ROLeditFields");
   Data.edit_security       = ROLvar(".ROLeditSecurity");
   // Iguana Wide
   Data.edit_webserver      = ROLvar(".ROLeditServer");
   Data.manage_instance     = ROLvar(".ROLmanageInstance");
   Data.edit_notifications  = ROLvar(".ROLeditNotifications");
   Data.edit_peers          = ROLvar(".ROLeditPeers");
   Data.edit_roles          = ROLvar(".ROLeditRoles");
   Data.create_component    = ROLvar(".ROLcreateComponent");
   Data.edit_users          = ROLvar(".ROLeditUsers");
   Data.edit_authentication = ROLvar(".ROLeditAuthentication");
   Data.edit_collections    = ROLvar(".ROLeditCollection");
   Data.edit_gitservers     = ROLvar(".ROLeditGitServers");
   Data.env_variables       = ROLvar(".ROLenvironmentVar");
   Data.database            = ROLvar(".ROLeditDatabases");
   Data.upgrade             = ROLvar(".ROLiguanaUpdates");
   Data.crash_file          = ROLvar(".ROLcrashFiles");
   Data.log_purging         = ROLvar(".ROLlogPurging");
   Data.edit_license        = ROLvar(".ROLeditLicense");
   Data.delete_service_logs = ROLvar(".ROLdeleteServiceLogs");
   Data.log_generator       = ROLvar(".ROLgenerateLogs");
   Data.edit_logencryption  = ROLvar(".ROLeditLogEncryption");
   Data.clear_archive       = ROLvar(".ROLclearArchive");
   Data.change_log_folder   = ROLvar(".ROLchangeLogFolder");
   Data.component_role      = ROLcomponentType();
   return Data;
}

function ROLvar(Selector){
   let div = document.querySelector(Selector);
   if(!div) return false;
   return div.checked;
}
function ROLsaveRole(){
   let Name = PAGEhashParameter("role");
   let Data = {};
   Data.name = Name;
   Data.role = ROLroleData();
   APIcall("role/edit", Data, function(R){
      if(!R?.success) return;
      let Role = R?.data || "";
      let Message = GINsaveRole(Role);
      GINaddAndCommit({message: Message, settings: true});
      window.location = "/#settings/roles";
   });
}
