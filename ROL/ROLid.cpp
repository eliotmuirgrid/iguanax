//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLid.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 2:03 PM
//  ---------------------------------------------------------------------------
#include <ROL/ROLid.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring ROLidToString(const ROLid& Id) {
//   COL_FUNCTION(ROLidToString);
   switch(Id) {
      // Component Operations
      case START:              return "start";
      case STOP:               return "stop";
      case REPOSITION_QUEUE:   return "reposition_queue";
      case CLEAR_QUEUE:        return "clear_queue";
      case CLEAR_ERRORS:       return "clear_errors";
      case DELETE_COMPONENT:   return "delete_component";

      // Component Logs
      case VIEW_LOGS:          return "view_logs";
      case RESUBMIT_MESSAGE:   return "resubmit_message";
      case EXPORT_SAMPLE_DATA: return "export_sample_data";
      case DELETE_LOGS:        return "delete_logs";
      case DOWNLOAD_LOGS:      return "download_logs";

      // Component Configuration
      case EDIT_NAME:          return "edit_name";
      case EDIT_DESCRIPTION:   return "edit_description";
      case EDIT_COMMIT:        return "edit_commit";
      case EDIT_TAGS:          return "edit_tags";

      // Component Development
      case VIEW_SCRIPT:        return "view_script";
      case EDIT_SCRIPT:        return "edit_script";
      case EDIT_UPSTREAM:      return "edit_upstream";
      case EDIT_CONNECTIONS:   return "edit_connections";
      case EDIT_FIELDS:        return "edit_fields";

      // Iguana Wide
      case EDIT_WEBSERVER:     return "edit_webserver";
      case EDIT_NOTIFICATIONS: return "edit_notifications";
      case EDIT_ROLES:         return "edit_roles";
      case CREATE_COMPONENT:   return "create_component";
      case EDIT_USERS:         return "edit_users";
      case EDIT_COLLECTIONS:   return "edit_collections";
      case ENV_VARIABLES:      return "env_variables";
      case DATABASE:           return "database";
      case UPGRADE:            return "upgrade";
      case CRASH_FILE:         return "crash_file";
      case LOG_PURGING:        return "log_purging";
      case EDIT_LICENSE:       return "edit_license";
      case DELETE_SERVICE_LOGS:return "delete_service_logs";
      case LOG_GENERATOR:      return "log_generator";
      case EDIT_GITSERVERS:    return "edit_gitservers";
      case MANAGE_INSTANCE:    return "manage_instance";
      case EDIT_LOG_ENCRYPTION:return "edit_logencryption";
      case EDIT_SECURITY:      return "edit_security";
      case CLEAR_ARCHIVE:      return "clear_archive";
      case CHANGE_LOG_FOLDER:  return "change_log_folder";
      case EDIT_AUTHENTICATION: return "edit_authentication";
      case EDIT_PEERS:         return "edit_peers";
      default: return "unknown";
   }
}


ROLid ROLstringToId(const COLstring& String) {
//   COL_FUNCTION(ROLstringToId);
   // Component Operations
   if(String == "start")              return START;
   if(String == "stop")               return STOP;
   if(String == "reposition_queue")   return REPOSITION_QUEUE;
   if(String == "clear_queue")        return CLEAR_QUEUE;
   if(String == "clear_errors")       return CLEAR_ERRORS;
   if(String == "delete_component")   return DELETE_COMPONENT;

   // Component Logs
   if(String == "view_logs")          return VIEW_LOGS;
   if(String == "resubmit_message")   return RESUBMIT_MESSAGE;
   if(String == "export_sample_data") return EXPORT_SAMPLE_DATA;
   if(String == "delete_logs")        return DELETE_LOGS;
   if(String == "download_logs")      return DOWNLOAD_LOGS;

   // Component Configuration
   if(String == "edit_name")          return EDIT_NAME;
   if(String == "edit_description")   return EDIT_DESCRIPTION;
   if(String == "edit_commit")        return EDIT_COMMIT;
   if(String == "edit_tags")          return EDIT_TAGS;

   // Component Development
   if(String == "view_script")        return VIEW_SCRIPT;
   if(String == "edit_script")        return EDIT_SCRIPT;
   if(String == "edit_upstream")      return EDIT_UPSTREAM;
   if(String == "edit_connections")   return EDIT_CONNECTIONS;
   if(String == "edit_fields")        return EDIT_FIELDS;

   // Iguana Wide
   if(String == "edit_webserver")     return EDIT_WEBSERVER;
   if(String == "edit_notifications") return EDIT_NOTIFICATIONS;
   if(String == "edit_roles")         return EDIT_ROLES;
   if(String == "create_component")   return CREATE_COMPONENT;
   if(String == "edit_users")         return EDIT_USERS;
   if(String == "edit_collections")   return EDIT_COLLECTIONS;
   if(String == "env_variables")      return ENV_VARIABLES;
   if(String == "database")           return DATABASE;
   if(String == "upgrade")            return UPGRADE;
   if(String == "crash_file")         return CRASH_FILE;
   if(String == "log_purging")        return LOG_PURGING;
   if(String == "edit_license")       return EDIT_LICENSE;
   if(String == "delete_service_logs")return DELETE_SERVICE_LOGS;
   if(String == "log_generator")      return LOG_GENERATOR;
   if(String == "edit_gitservers")    return EDIT_GITSERVERS;
   if(String == "edit_logencryption") return EDIT_LOG_ENCRYPTION;
   if(String == "edit_security")      return EDIT_SECURITY;
   if(String == "clear_archive")      return CLEAR_ARCHIVE;
   if(String == "change_log_folder")  return CHANGE_LOG_FOLDER;
   if(String == "edit_gitinstance" || String == "manage_instance") return MANAGE_INSTANCE;
   if(String == "edit_authentication") return EDIT_AUTHENTICATION;
   if(String == "edit_peers")         return EDIT_PEERS;
   return (ROLid) -1;
}

bool ROLisIguanaWide(const ROLid& Id) {
   COL_FUNCTION(ROLisIguanaWide);
   switch(Id) {
      case EDIT_WEBSERVER:
      case EDIT_NOTIFICATIONS:
      case EDIT_ROLES:
      case CREATE_COMPONENT:
      case EDIT_USERS:
      case EDIT_COLLECTIONS:
      case ENV_VARIABLES:
      case DATABASE:
      case UPGRADE:
      case CRASH_FILE:
      case LOG_PURGING:
      case EDIT_LICENSE:
      case DELETE_SERVICE_LOGS:
      case LOG_GENERATOR:
      case EDIT_GITSERVERS:
      case MANAGE_INSTANCE:
      case EDIT_LOG_ENCRYPTION:
      case EDIT_SECURITY:
      case CLEAR_ARCHIVE:
      case CHANGE_LOG_FOLDER:
      case EDIT_AUTHENTICATION:
      case EDIT_PEERS:
         return true;
      default: return false;
   }
}

