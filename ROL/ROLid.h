#ifndef __ROLID_H__
#define __ROLID_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ROLid.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   25/01/24 2:03 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
#include <COL/COLarray.h>

class COLstring;

// IF YOU ADD/REMOVE A ROLE -- update vector size in ROLrole to prevent segfaults
enum ROLid {
   START,
   STOP,
   REPOSITION_QUEUE,
   CLEAR_QUEUE,
   CLEAR_ERRORS,
   DELETE_COMPONENT,
   VIEW_LOGS,
   RESUBMIT_MESSAGE,
   EXPORT_SAMPLE_DATA,
   DELETE_LOGS,
   DOWNLOAD_LOGS,
   EDIT_NAME,
   EDIT_DESCRIPTION,
   EDIT_COMMIT,
   EDIT_TAGS,
   VIEW_SCRIPT,
   EDIT_SCRIPT,
   EDIT_UPSTREAM,
   EDIT_CONNECTIONS,
   EDIT_FIELDS,
   EDIT_WEBSERVER,
   EDIT_NOTIFICATIONS,
   EDIT_ROLES,
   CREATE_COMPONENT,
   EDIT_USERS,
   EDIT_COLLECTIONS,
   ENV_VARIABLES,
   DATABASE,
   UPGRADE,
   CRASH_FILE,
   LOG_PURGING,
   EDIT_LICENSE,
   DELETE_SERVICE_LOGS,
   LOG_GENERATOR,
   EDIT_GITSERVERS,
   MANAGE_INSTANCE,
   EDIT_LOG_ENCRYPTION,
   EDIT_SECURITY,
   CHANGE_LOG_FOLDER,
   CLEAR_ARCHIVE,
   EDIT_AUTHENTICATION,
   EDIT_PEERS
};

// m_Role size must be equal to number of ROLid's
class ROLrole {
  public:
   explicit ROLrole(const bool DefaultPermission = false, const bool IsUnsecure = false,
                    const bool ComponentRole = false)
       : m_Role(42, DefaultPermission), m_Unsecure(IsUnsecure), m_ComponentRole(ComponentRole) {}
   COLarray<bool> m_Role;
   bool            m_Unsecure      = false;
   bool            m_ComponentRole = false;
};

class ROLmap {
 public:
   COLmap<COLstring, ROLrole> m_Roles;
};

COLstring ROLidToString(const ROLid& Id);
ROLid     ROLstringToId(const COLstring& String);
bool      ROLisIguanaWide(const ROLid& Id);

#endif // end of defensive include
