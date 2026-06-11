#ifndef __INS_UTILS_H__ // begin defensive include
#define __INS_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSutils
//
// Description:
//
// Installer utility functions.
//    
// Author: John Qi
// Date:   Wed 20 Jun 2019 10:07:16 EDT
//---------------------------------------------------------------------------

class COLstring;

COLstring INScurrentUser();

COLstring INSaskUser(const COLstring& Question, const COLstring& DefaultAnswer);

bool INSuserExist(const COLstring& UserId); // Does user exist?

COLstring INShomeDir(const COLstring* pUser=0);

bool INSisSafeAppDir(const COLstring& ApplicationDir, const COLstring& User);  // returns false if the directory in question is the user's home directory or root etc.

bool INSisDirUnderInstaller(const COLstring& Dir);

bool INSisAdmin();

#ifdef _WIN32
COLstring INSprogramFilesDir();   
#endif

#endif // end defensive include
