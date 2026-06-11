//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PRMcallbacks.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   31/01/24 12:05 PM
//  ---------------------------------------------------------------------------
#include <PRM/PRMcallbacks.h>
#include <PRM/PRMpermissionUtils.h>
#include <PRM/PRMroleCheck.h>

#include <CFG/CFGmapApi.h>

#include <ROL/ROLid.h>

#include <USR/USRuser.h>

#include <SCK/SCKloop.h>
#include <COL/COLauto.h>
#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLioCheck.h>
#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLauto<COLclosure5<bool, const COLstring&, const COLstring&, ROLrole*, COLstring*>> pPRMroleChecker = NULL;

static SCKloop* pPRMsckLoop = nullptr;

static void PRMlogPermissionMissing(bool FromTranslator, const COLstring& Action, const COLstring& Agent, const COLstring& Target,
                                    const COLmap<COLstring, USRuser>* pUserMap, CFGmap* pComponentMap){
   COL_FUNCTION(PRMlogPermissionMissing);
   // TODO decide on adequate log message
}

static void PRMcheckRoleOnMain(bool FromTranslator, COLstring Agent, COLstring Target, ROLrole* pAgentRole, COLstring* pError, COLevent* pEvent){
   COL_FUNCTION(PRMcheckRoleOnMain);
   pPRMroleChecker->run(FromTranslator, Agent, Target, pAgentRole, pError);
   pEvent->signal();
}

// These functions can be run from a worker thread or the main thread
static bool PRMcheckRole(bool FromTranslator, const COLstring& Agent, const COLstring& Target, ROLrole* pAgentRole, COLstring* pError) {
   COL_FUNCTION(PRMcheckRole);
   pError->clear();
   if(COLisMainThread()) {
      COL_TRC("On main thread already, can just run the closure");
      pPRMroleChecker->run(FromTranslator, Agent, Target, pAgentRole, pError);
      return pError->is_null();
   }
   COL_TRC("On worker thread, will have to post back to main first");
   if(pPRMsckLoop) {
      COLevent Event;
      pPRMsckLoop->post(COLnewClosure0(&PRMcheckRoleOnMain,
                                       FromTranslator,
                                       Agent,
                                       Target,
                                       pAgentRole,
                                       pError,
                                       &Event));
      Event.wait();
      return pError->is_null();
   }
   return false;
}

bool PRMcheckRolePermissions(bool FromTranslator, const COLstring& Agent, const COLstring& Target, ROLrole* pAgentRole, COLstring* pError) {
   COL_FUNCTION(PRMcheckRolePermissions);
   if (!PRMcheckRole(FromTranslator, Agent, Target, pAgentRole, pError)) {
      COL_TRC("Encountered an error when checking permissions");
      return false;
   }
   return true;
}

bool PRMcheckRoleAccess(bool FromTranslator, const COLstring& Agent, const COLstring& Target, const ROLid& Id, COLstring* pError) {
   COL_FUNCTION(PRMcheckRoleAccess);
   ROLrole Role;
   if (!PRMcheckRole(FromTranslator, Agent, Target, &Role, pError)) {
      COL_TRC("Encountered an error when checking permissions");
      return false;
   }
   return Role.m_Role[(int) Id];
}

void PRMsetRoleCallbacks(SCKloop* pLoop, const COLmap<COLstring, USRuser>* pUserMap, ROLmap* pRoleMap, CFGmap* pComponentMap) {
   COL_FUNCTION(PRMsetRoleCallbacks);
   pPRMsckLoop          = pLoop;
   pPRMroleChecker      = COLnewClosure5(&PRMroleChecker, pUserMap, pRoleMap, pComponentMap);
}

COLstring PRMsetError(const ROLid& Id) {
   COLstring Error = "";
   switch(Id) {
      case START: Error = "Start Component"; break;
      case STOP: Error = "Stop Component"; break;
      case REPOSITION_QUEUE: Error = "Reposition Queue"; break;
      case CLEAR_QUEUE: Error = "Clear Queue"; break;
      case CLEAR_ERRORS: Error = "Clear Errors"; break;
      case DELETE_COMPONENT: Error = "Delete Component"; break;
      case VIEW_LOGS: Error = "View Logs"; break;
      case DOWNLOAD_LOGS: Error = "Downloadx Logs"; break;
      case RESUBMIT_MESSAGE: Error = "Resubmit Message"; break;
      case EXPORT_SAMPLE_DATA: Error = "Export as Sample Data"; break;
      case DELETE_LOGS: Error = "Delete Logs"; break;
      case EDIT_NAME: Error = "Edit Name"; break;
      case EDIT_DESCRIPTION: Error = "Edit Description"; break;
      case EDIT_COMMIT: Error = "Edit Commit"; break;
      case VIEW_SCRIPT: Error = "View Script"; break;
      case EDIT_SCRIPT: Error = "Edit Script"; break;
      case EDIT_UPSTREAM: Error = "Edit Upstream"; break;
      case EDIT_CONNECTIONS: Error = "Edit Connections"; break;
      case EDIT_FIELDS: Error = "Edit Fields"; break;
      case EDIT_WEBSERVER: Error = "Edit Webserver"; break;
      case EDIT_NOTIFICATIONS: Error = "Edit Notifications"; break;
      case EDIT_ROLES: Error = "Edit Roles"; break;
      case CREATE_COMPONENT: Error = "Create Component"; break;
      case EDIT_USERS: Error = "Edit Users"; break;
      case EDIT_COLLECTIONS: Error = "Edit Collections"; break;
      case ENV_VARIABLES: Error = "Edit Environment Var"; break;
      case DATABASE: Error = "Edit Databases"; break;
      case UPGRADE: Error = "Software Updates"; break;
      case CRASH_FILE: Error = "Crash Files"; break;
      case LOG_PURGING: Error = "Edit Log Purging"; break;
      case EDIT_LICENSE: Error = "Edit License"; break;
      case DELETE_SERVICE_LOGS: Error = "Delete Service Logs"; break;
      case LOG_GENERATOR: Error = "Generate Logs"; break;
      case EDIT_GITSERVERS: Error = "Edit Git Servers"; break;
      case MANAGE_INSTANCE: Error = "Manage Instance"; break;
      case EDIT_LOG_ENCRYPTION: Error = "Edit Log Encryption"; break;
      case EDIT_SECURITY: Error = "Edit Security"; break;
      case CLEAR_ARCHIVE: Error = "Clear Archive"; break;
      case CHANGE_LOG_FOLDER: Error = "Change Log Directory"; break;
      case EDIT_AUTHENTICATION: Error = "Edit Authentication"; break;
      default: Error = "Unknown"; break;
   }
   Error += " permission required to perform this action";
   return Error;
}