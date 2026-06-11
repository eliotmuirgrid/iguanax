//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INScutils
//
// Description:
//
// See header file.
//
// Author: John Qi
// Date:   Wed 20 Jun 2019 10:07:56 EDT
//
//---------------------------------------------------------------------------

#include "INSutils.h"

#include <stdio.h>   // fgets

#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <PRO/PROexecute.h>
#include <PRO/PROenv.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>


#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLuint32 BUFFER_SIZE = 1024;

COLstring INSaskUser(const COLstring& Question, const COLstring& DefaultAnswer) {
   COL_FUNCTION(INSaskUser);
   COLcout << Question << " [" << DefaultAnswer << "]: ";
   char InputBuffer[BUFFER_SIZE];
   COLstring Answer = ::fgets(InputBuffer, sizeof(InputBuffer), stdin);
   Answer = COLstripWhiteSpace(Answer);
   if (Answer.is_null()) {
      Answer = DefaultAnswer;
   }
   return Answer;
}


bool INSuserExist(const COLstring& UserId){
   COL_FUNCTION(INSuserExist);
#ifndef _WIN32
   COLstring Sink;
   int ExitCode = PROexecuteString(".", "id " + UserId, &Sink, &Sink);
   COL_VAR2(ExitCode, Sink);
   return ExitCode == 0;
#else
   SID_NAME_USE Blah;
   COL_VAR(UserId);
   //BOOL Result = LookupAccountNameA(NULL, UserId.c_str(), NULL, 0, NULL, 0, &Blah);
   //COL_VAR(Result);
   //return Result == TRUE;
   return true;
#endif
   COL_ERR("INSuserExist not implemented.");
   return false;
}

COLstring INScurrentUser(){
   COL_FUNCTION(INScurrentUser);
#ifdef _WIN32
   COLstring UserName;
   UserName.setCapacity(256);
   DWORD Length=256;
   BOOL Result = GetUserNameA((LPSTR)UserName.get_buffer(), &Length);
   COL_VAR(Length);
   UserName.setSize(Length-1); // -1 since Length includes null terminating character
   COL_VAR(UserName);
   return UserName;
#else
   COLstring Sink;
   int ExitCode = PROexecuteString(".", "whoami", &Sink, &Sink);
   COL_VAR2(ExitCode, Sink);
   Sink.stripAll('\n');
   Sink.stripAll('\r');
   return Sink;
#endif // not _WIN32
}

#ifdef __APPLE__
#  define INS_APP_BASE "/Users/"
#endif
#ifdef __linux__
#  define INS_APP_BASE "/home/"
#endif

COLstring INShomeDir(const COLstring* pUser){
   COL_FUNCTION(INShomeDir);
   COLstring User;
   if (!pUser){
      User = INScurrentUser();
   } else {
      User = *pUser;
   }
   COL_VAR(User);
#ifndef _WIN32
   COLstring Home = INS_APP_BASE + User;
   FILaddPathSeparator(Home);
#else
   COLstring Home = PROgetenv("HOMEDRIVE");
   if (Home.is_null()){
      Home = "C:";
   }
   Home += "\\Users\\" + User + "\\";
#endif
   return Home;
}

bool INSisDirUnderInstaller(const COLstring& Dir){
   COL_FUNCTION(INSisDirUnderInstaller);
   COLstring Path = FILpathResolveEnvironment(Dir);
   COL_TRC(Path);
   COLstring ExeDir = FILexecutableDir();
   if (Path.substr(0, ExeDir.size()) != ExeDir){
      COL_TRC("User probably didn't intend to install under the installer directory.");
      return false;
   }
   return (Path != ExeDir);  // Application dir should not the installer directory
}

bool INSisAdmin(){
   COL_FUNCTION(INSisAdmin);
#ifdef _WIN32
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if(OpenProcessToken( GetCurrentProcess(),TOKEN_QUERY,&hToken)){
      TOKEN_ELEVATION Elevation;
      DWORD cbSize = sizeof(TOKEN_ELEVATION);
      if(GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize)) {
         fRet = Elevation.TokenIsElevated;
      }
   }
   if(hToken) {
      CloseHandle( hToken );
   }
   return (fRet == TRUE);
#else
   return (0 == getuid());  // Root User has id 0
#endif
}

#ifdef _WIN32
COLstring INSprogramFilesDir(){
   COL_FUNCTION(INSprogramFilesDir);
   COLstring Dir = PROgetenv("HOMEDRIVE");
   if (Dir.is_null()){
      Dir = "C:";
   }
   Dir += "\\Program Files\\";
   COL_VAR(Dir);
   return Dir;
}
#endif
