//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMinstall
//
// Description:
//
// Implementation
//
// Author:  Eliot Muir
// Date:    Thu 10/1/2022
//---------------------------------------------------------------------------
#include "SDMpermissions.h"


#include <windows.h>
#include <memory.h>
#include <string.h>
#include <Ntsecapi.h>
#include <lsalookup.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDMextractUserDomain(const COLstring& User, COLstring* pUser, COLstring* pDomain){
   COL_FUNCTION(SDMextractUserDomain);
   COLstring Full = User;
   Full.replace("\\", "/");
   if (!Full.split(*pDomain, *pUser, "/")){
      pDomain->clear();
      *pUser = User;
   }
   COL_VAR2(*pUser, pDomain);
}

bool SDMverifyPassword(const COLstring& User, const COLstring& Password){
   COL_FUNCTION(SDMverifyPassword);
   HANDLE hToken;
   BOOL bResult;
   DWORD dwLastError;
   COLstring UserName, Domain;
   SDMextractUserDomain(User, &UserName, &Domain);
   LPCSTR pDomain = NULL;
   LPCSTR pUser   = UserName.c_str();
   if (!Domain.is_null()){
      pDomain = Domain.c_str();
   }
   COL_VAR3(pUser, pDomain, Password.c_str());
   // Log on the user
   bResult = LogonUserA(pUser, pDomain, Password.c_str(), LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &hToken);
   if (bResult){
      CloseHandle(hToken);
      return true;
   } else {
      return false;
   }
}

void SDMconvertString(const COLstring& Input, LSA_UNICODE_STRING* pOut){
   COL_FUNCTION(SDMconvertString);
   COL_VAR(Input);
   int CountOfBytes = MultiByteToWideChar(CP_UTF8, 0, Input.c_str(), Input.size(), NULL, 0);
   COL_VAR(CountOfBytes);
   pOut->Buffer = new wchar_t[CountOfBytes+1];  // This requires clean up!
   CountOfBytes = ::MultiByteToWideChar(CP_UTF8, 0, Input.c_str(), Input.size(), pOut->Buffer, CountOfBytes);
   pOut->Length = CountOfBytes *2;
   pOut->MaximumLength = CountOfBytes * 2 + 2;
   COL_DUMP("Converted unicode:", pOut->Buffer, pOut->MaximumLength);
   COL_VAR(CountOfBytes);   
}

bool SDMassignRight(LSA_HANDLE PolicyHandle, void* pSid, const COLstring& Right){
   COL_FUNCTION(SDMassignRight);
   COL_VAR2(pSid, Right);
   LSA_UNICODE_STRING right_name;
   SDMconvertString(Right, &right_name);
   NTSTATUS status = LsaAddAccountRights(PolicyHandle, pSid, &right_name, 1);
   if (status != 0){
      COL_ERR(COLerrorString(LsaNtStatusToWinError(status)));
      return false;
   }
   COL_TRC("Successfully assigned right.");
   return true;
}

bool SDMopenPolicyManager(LSA_HANDLE* pPolicyHandle, COLostream& Log){
   COL_FUNCTION(SDMopenPolicyManager);
   LSA_OBJECT_ATTRIBUTES object_attributes;
   ZeroMemory(&object_attributes, sizeof(object_attributes));  // Will crash without this
   NTSTATUS status = LsaOpenPolicy(NULL, &object_attributes, POLICY_ALL_ACCESS, pPolicyHandle);
   COL_VAR(status);
   if (status != 0){
      Log << "Are you running with adminstration rights?" << newline;
      Log << "Unable to alter user permissions: " << COLerrorString(LsaNtStatusToWinError(status));
      return false;
   }
   return true;
}

void* SDMgetSidByUserName (PLSA_HANDLE pPolicyHandle, COLstring userName) {
   COL_FUNCTION(SDMgetSidByUserName);
   userName.replace("/", "\\");
	NTSTATUS ntStatus;
   PLSA_REFERENCED_DOMAIN_LIST referencedDomain;
	PLSA_TRANSLATED_SID2 translatedSid;
	LSA_UNICODE_STRING lsaString;
	DWORD dwLen = 0;
   SDMconvertString(userName, &lsaString);
   COL_DUMP("User buffer in unicode", lsaString.Buffer, lsaString.Length);
	ntStatus = LsaLookupNames2(*pPolicyHandle, 0x80000000, 1, &lsaString, &referencedDomain, &translatedSid);
	COL_VAR(ntStatus);
	if (ntStatus == ERROR_SUCCESS) {
		LsaFreeMemory(referencedDomain);
	} else { translatedSid = NULL; }
   if (ntStatus != 0 ) {
      COL_TRC(COLerrorString(LsaNtStatusToWinError(ntStatus)));
      return NULL;  // TODO - needs more attention
   }
   COL_VAR(ntStatus);
   COL_VAR(translatedSid->Sid);
   void* pResult = translatedSid->Sid;
   LsaFreeMemory(translatedSid);
   delete lsaString.Buffer;
	return pResult;
}

void SDMtranslateSid(void* pSid, COLstring* pDomain, COLstring* pUser){
   COL_FUNCTION(SDMtranslateSid);
   pDomain->setCapacity(255);
   pUser->setCapacity(255);
   DWORD DomainLength = 255;
   DWORD UserLength  = 255;
   SID_NAME_USE NameUse;
   BOOL SuccessCode = LookupAccountSidA(NULL, pSid, pUser->get_buffer(), &UserLength, pDomain->get_buffer(), &DomainLength, &NameUse);
   if (SuccessCode == 0){
      COL_ERR(COLerrorString(::GetLastError()));
      return;
   }
   COL_VAR2(UserLength, DomainLength);
   pUser->setSize(UserLength);
   pDomain->setSize(DomainLength);
   COL_DUMP("SID translated", pUser->c_str(), pUser->size());
   COL_DUMP("Domain translated", pDomain->c_str(), pDomain->size());
}

bool SDMgiveUserRightToRunAsService(const COLstring& UserName, COLostream& Log){
   COL_FUNCTION(SDMgiveUserRightToRunAsService);
   LSA_HANDLE PolicyHandle;
   if (!SDMopenPolicyManager(&PolicyHandle,Log)){
      return false;
   }
   void* Sid = SDMgetSidByUserName(&PolicyHandle, UserName);
   if (NULL == Sid){
      Log << "Could not find user: " << UserName << newline;
      LsaClose(PolicyHandle);
      return false;
   }
   if (!SDMassignRight(PolicyHandle, Sid, "SeServiceLogonRight")){
      Log << "Failed to assign login as service right." << newline;
      LsaClose(PolicyHandle);
      return false;
   }
   COLstring User, Domain;
   SDMtranslateSid(Sid, &User, &Domain);
   Log << "Running under " << User << "/" << Domain << " with the right to login as a service." << newline;
   LsaClose(PolicyHandle);
   return true;
}

bool SDMopenPolicyManager(LSA_HANDLE* pPolicyHandle, COLstring* pError){
   COL_FUNCTION(SDMopenPolicyManager);
   LSA_OBJECT_ATTRIBUTES object_attributes;
   ZeroMemory(&object_attributes, sizeof(object_attributes));  // Will crash without this
   NTSTATUS status = LsaOpenPolicy(NULL, &object_attributes, POLICY_ALL_ACCESS, pPolicyHandle);
   COL_VAR(status);
   if (status != 0){
      *pError = "Unable to alter user permissions: " + COLerrorString(LsaNtStatusToWinError(status));
      return false;
   }
   return true;
}


bool SDMgiveUserRightToRunAsService(const COLstring& UserName, COLstring* pError){
   COL_FUNCTION(SDMgiveUserRightToRunAsService);
   LSA_HANDLE PolicyHandle;
   if (!SDMopenPolicyManager(&PolicyHandle,pError)){
      return false;
   }
   void* Sid = SDMgetSidByUserName(&PolicyHandle, UserName);
   if (NULL == Sid){
      *pError = "Could not find user: " + UserName;
      LsaClose(PolicyHandle);
      return false;
   }
   if (!SDMassignRight(PolicyHandle, Sid, "SeServiceLogonRight")){
      *pError = "Failed to assign login as service right.";
      LsaClose(PolicyHandle);
      return false;
   }
   COLstring User, Domain;
   SDMtranslateSid(Sid, &User, &Domain);
   LsaClose(PolicyHandle);
   return true;
}