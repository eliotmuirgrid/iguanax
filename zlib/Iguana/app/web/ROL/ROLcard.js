function ROLcardHtml(Data, Name, Edit){
   let Permission = ROLeditPermission();
   let Collapsed = ROLopenedTo == Name ? "" : "ROLcollapsed";
   let RoleType = Data?.component_role ? "ROLcomponentRole" : "";
   let buttons = Edit || !Permission ? "<br><br>" : ROLbuttonHtml(Name);
   let EditHtml = Edit ? "ROLedit" : "";
   let TagHtml = HASHTAGrenderTags([Name]);
   return /*html*/`
   <div class="ROLcard ${RoleType} ${Collapsed} ${EditHtml}">
      <div class="ROLheader">
         <div class="ROLroleName"> ${TagHtml} </div>
         ${buttons}
      </div>
      ${ROLroleTypeHtml(Data.component_role, Edit)}
      <div class="ROLscaffold">
         <div class="ROLsubsection">
            ${ROLsubtitleHtml("Configuration", Edit)}
            <div class="ROLlist" >
               ${ROLlineItemHtml("Edit Description",    "ROLeditDescription", Data.edit_description, Edit)}
               ${ROLlineItemHtml("Edit Fields",         "ROLeditFields",      Data.edit_fields,      Edit)}
               ${ROLlineItemHtml("Edit Name",           "ROLeditName",        Data.edit_name,        Edit)}
               ${ROLlineItemHtml("Edit Running Commit", "ROLchangeCommit",    Data.edit_commit,      Edit)}
               ${ROLlineItemHtml("Edit Tags",           "ROLeditTags",        Data.edit_tags,        Edit)}
            </div>
         </div>
         <div class="ROLsubsection">
            ${ROLsubtitleHtml("Development", Edit)}
            <div class="ROLlist" >
               ${ROLlineItemHtml("Change Upstream",  "ROLchangeUpstream",  Data.edit_upstream,    Edit)}
               ${ROLlineItemHtml("Edit Connections", "ROLeditConnections", Data.edit_connections, Edit)}
               ${ROLlineItemHtml("Edit Script",      "ROLeditScript",      Data.edit_script,      Edit)}
               ${ROLlineItemHtml("View Script",      "ROLviewScript",      Data.view_script,      Edit)}
            </div>
         </div>
         <div class="ROLsubsection">
            ${ROLsubtitleHtml("Logs", Edit)}
            <div class="ROLlist" >
               ${ROLlineItemHtml("Delete Logs",           "ROLdeleteLogs",      Data.delete_logs,        Edit)}
               ${ROLlineItemHtml("Download Logs",         "ROLdownloadLogs",    Data.download_logs,      Edit)}
               ${ROLlineItemHtml("Export as Sample Data", "ROLexportSample",    Data.export_sample_data, Edit)}
               ${ROLlineItemHtml("Resubmit Message",      "ROLresubmitMessage", Data.resubmit_message,   Edit)}
               ${ROLlineItemHtml("View Logs",             "ROLviewLogs",        Data.view_logs,          Edit)}
            </div>
         </div>
         <div class="ROLsubsection">
            ${ROLsubtitleHtml("Operations", Edit)}
            <div class="ROLlist" >
               ${ROLlineItemHtml("Clear Errors",     "ROLclearErrors",     Data.clear_errors,     Edit)}
               ${ROLlineItemHtml("Clear Queue",      "ROLclearQueue",      Data.clear_queue,      Edit)}
               ${ROLlineItemHtml("Delete Component", "ROLdeleteComponent", Data.delete_component, Edit)}
               ${ROLlineItemHtml("Reposition Queue", "ROLrepositionQueue", Data.reposition_queue, Edit)}
               ${ROLlineItemHtml("Start Component",  "ROLstart",           Data.start,            Edit)}
               ${ROLlineItemHtml("Stop Component",   "ROLstop",            Data.stop,             Edit)}
            </div>
         </div>
         <div class="ROLsubsection ROLiguanaWide">
            ${ROLsubtitleHtml("Settings", Edit)}
            <div class="ROLlist" >
               ${ROLlineItemHtml("Change Log Folder",    "ROLchangeLogFolder",   Data.change_log_folder,  Edit)}
               ${ROLlineItemHtml("Edit Authentication",  "ROLeditAuthentication",Data.edit_authentication,Edit)}
               ${ROLlineItemHtml("Edit Collections",     "ROLeditCollection",    Data.edit_collections,   Edit)}
               ${ROLlineItemHtml("Edit Databases",       "ROLeditDatabases",     Data.database,           Edit)}
               ${ROLlineItemHtml("Edit Environment Var", "ROLenvironmentVar",    Data.env_variables,      Edit)}
               ${ROLlineItemHtml("Edit Git Servers",     "ROLeditGitServers",    Data.edit_gitservers,    Edit)}
               ${ROLlineItemHtml("Edit License",         "ROLeditLicense",       Data.edit_license,       Edit)}
               ${ROLlineItemHtml("Edit Log Encryption",  "ROLeditLogEncryption", Data.edit_logencryption, Edit)}
               ${ROLlineItemHtml("Edit Log Purging",     "ROLlogPurging",        Data.log_purging,        Edit)}
               ${ROLlineItemHtml("Edit Notifications",   "ROLeditNotifications", Data.edit_notifications, Edit)}
               ${ROLlineItemHtml("Edit Peers",           "ROLeditPeers",         Data.edit_peers,         Edit)}
               ${ROLlineItemHtml("Edit Roles",           "ROLeditRoles",         Data.edit_roles,         Edit)}
               ${ROLlineItemHtml("Edit Security",        "ROLeditSecurity",      Data.edit_security,      Edit)}
               ${ROLlineItemHtml("Edit Users",           "ROLeditUsers",         Data.edit_users,         Edit)}
               ${ROLlineItemHtml("Edit Web Server",      "ROLeditServer",        Data.edit_webserver,     Edit)}
               ${ROLlineItemHtml("Manage Instance",      "ROLmanageInstance",    Data.manage_instance,   Edit)}
            </div>
         </div>
         <div class="ROLsubsection ROLiguanaWide">
         ${ROLsubtitleHtml("Other", Edit)}
         <div class="ROLlist" >
            ${ROLlineItemHtml("Clear Archive",        "ROLclearArchive",      Data.clear_archive,      Edit)}
            ${ROLlineItemHtml("Create Component",     "ROLcreateComponent",   Data.create_component,   Edit)}
            ${ROLlineItemHtml("Crash Files",          "ROLcrashFiles",        Data.crash_file,         Edit)}
            ${ROLlineItemHtml("Delete Service Logs",  "ROLdeleteServiceLogs", Data.delete_service_logs,Edit)}
            ${ROLlineItemHtml("Log Generator",        "ROLgenerateLogs",      Data.log_generator,      Edit)}
            ${ROLlineItemHtml("Software Updates",     "ROLiguanaUpdates",     Data.upgrade,            Edit)}
         </div>
      </div>
      </div>
   </div>`;
}

function ROLroleTypeHtml(Value, Edit){
   let disabled = Edit ? "" : "disabled";
   let UserRole = Value ? "" : "selected";
   let ComponentRole = Value ? "selected" : "";
   return /*html*/`
   <div class="ROLroleAppliesTo">
      <div class="FORMlabel ROLlabel">Type of Role</div>
      <select class="FORMselectBox" ${disabled}>
         <option value="user" ${UserRole}>User Role</option>
         <option value="component" ${ComponentRole}>Component Role</option>
      </select>
      <div></div>
   </div>`;
}

function ROLsubtitleHtml(Title, Edit){
   if(!Edit) return /*html*/`
   <div class="FORMlabel ROLlabel">
      ${Title}
   </div>
   `;
   return /*html*/`
   <div class="ROLlabel">
      <div  class="FORMlabel">${Title}</div>
      <div class="ROLselectRow">[
         <a class="ROLselectAll">Select All</a> |
         <a class="ROLdeselectAll">Deselect</a>
      ]
      </div>
   </div>`;
}

function ROLbuttonHtml(Name){
   return /*html*/`         
   <a href="#settings/roles/edit?role=${Name}" class="BUTTONstandard BUTTONdefault">EDIT</a>
   <a class="FORMbutton BUTTONcancel BUTTONwarning" href="#settings/roles/remove?role=${Name}">
      <svg class="ROLtrash" width="16" height="16"><use href="ROL/ROLtrashIcon.svg#ROLtrashIcon"></use></svg>
   </a>`;
}

function ROLlineItemHtml(Label, Selector, Flag, Edit){
   let checked = Flag ? "checked" : "";
   let disabled = !Edit ? "disabled" : "";
   return /*html*/`
   <div class="ROLlineItem">
      <input type="checkbox" class="${Selector}" ${checked} ${disabled}>
      <div>${Label}</div>
   </div>`;
}
