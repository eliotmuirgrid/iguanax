// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCcredentials
//
//  Description
//
// Author: Tyler Brown
// Date:   29 Dec 2022
// ---------------------------------------------------------------------------
#include <ctype.h>
#include <string.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITC/GITCsetCredentials.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

void GITCsetUserEmail(const COLstring& Username, const COLstring& Name, const COLstring& Email) {
   COL_FUNCTION(GITCsetUserEmail);
   INIdata Config;
   GITUconfigLoadUser(Username, &Config);
   GITUconfigGlobalSetUserName(&Config, Name);
   GITUconfigGlobalSetEmail   (&Config, Email);
   GITUconfigSaveUser(Username, Config);
}

static bool GITCisValidName(const COLstring& Name) {
   if (Name.is_null()) { return true; }
   if (Name[0] == ' ' || Name[Name.size() - 1] == ' ') { return false; }
   for (int i = 0; i < Name.size(); ++i) {
      switch (Name[i]) {
         case ',':
         case '!':
         case '\'':
         case '"':
         case '@':
         case '<':
         case '>':
         case '$':
         case '%':
         case '&':
         case '(':
         case ')':
         case '\\':  return false;
         default:    break;
      }
   }
   return true;
}

static bool GITCisValidEmail(const COLstring& Email) {
   int atCount = 0;
   int dotCount = 0;
   int len = Email.size();
   if (len == 0) { return true; } // We allow empty emails for when users are trying to swap between values
   // The email must contain at least one "@" and one "."
   if (len < 5) return false;  // Email must be at least "a@b.c"

   // Iterate over each character of the email
   for (int i = 0; i < len; i++) {
      char c = Email[i];
      // Check for valid characters
      if (!isalnum(c) && c != '@' && c != '.' && c != '-' && c != '_') {
         return false;  // Invalid character found
      }
      // Count '@' symbols
      if (c == '@') {
         atCount++;
      }
      // Count '.' symbols after the '@'
      if (c == '.') {
         if (i == 0 || i == len - 1) {
            return false;  // Dot cannot be at the beginning or end
         }
         dotCount++;
      }
   }
   // There must be exactly one '@' symbol and at least one '.' in the domain part
   if (atCount != 1) {
      return false;  // Invalid number of '@' symbols
   }
   // Ensure there is a domain part after '@' and a top-level domain after the last '.'
   size_t Position = Email.find("@");
   if (npos == Position || '.' == Email[Position + 1] || npos == Email.find(".", Position + 1)) {
      return false;  // No domain or TLD after '@'
   }
   return true;  // Valid email
}

static void GITCsetCredentialsImpl(const COLstring& Username, const COLstring& Name, const COLstring& Email, const COLaddress& Address) {
   COL_FUNCTION(GITCsetCredentialsImpl);
   if (!GITCisValidName(Name)) {
      COLrespondError(Address, "Invalid name '" + Name + "'. Reserved characters are [,!'\"@<>$%&()\\].");
      return;
   }
   if (!GITCisValidEmail(Email)) {
      COLrespondError(Address, "Invalid email address '" + Email + "'.");
      return;
   }
   GITCsetUserEmail(Username, Name, Email);
   COLrespondSuccess(Address);
}

// /git/credentials/set
void GITCsetCredentials(const COLwebRequest& Request) {
   COL_FUNCTION(GITCsetCredentials);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   COLstring Username = Request.User;
   COLstring Name     = COLvarRequiredString(Data, "name");
   COLstring Email    = COLvarRequiredString(Data, "email");
   GITCsetCredentialsImpl(Username, Name, Email, Request.Address);
}
