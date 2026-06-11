#ifndef __INS_INSTALL_H__
#define __INS_INSTALL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSinstall
//
// Description:
//
// Installation utility functions.
// 
// Author: John Qi
// Date:   Mon 24 Jun 2019 16:07:16 EDT
//
//---------------------------------------------------------------------------

class COLstring;

// extract the program files from the Installer file and save them in AppDir.
// AppDir cannot exist already.
void INSinstallProgramFiles(const COLstring& Installer, const COLstring& ZipRootDir, const COLstring& AppDir);


#endif // end defensive include
