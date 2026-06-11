//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBconfig
//
// Description:
//
// Implementation
//
// Author: Shuo Hou
// Date:   Monday, March 15th, 2021 @ 03:03:15 PM
//---------------------------------------------------------------------------
#include <IWEB/IWEBconfig.h>

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static COLstring IWEBfileCorrectFileName(COLstring FileName){
   if (!FileName.is_null()){
      // Strip leading and trailing quotes as the path might have been received from user input
      FileName = FileName.strip(COLstring::Both, '\"');
      FileName = FileName.strip(COLstring::Both, '\'');
      FileName = FILpathSepToNative(FileName);
   }
   return FileName;
}

// converts the absolute path FileName to one that is relative to the current working dir,
// which should be the iguana working dir.
// If FileName is above the current working dir, no modification is made
static COLstring IWEBfileMakeRelative(COLstring FileName){
   COLstring WorkingDir = FILworkingDir();
   COLstring Output, OutputErr;
   FILpathSimplify(FileName, &Output, &OutputErr);
   FileName = Output;
   size_t Position = FileName.find(WorkingDir);
   if (npos == Position) { return FileName; }
   return FileName.substr(WorkingDir.size());
}

void IWEBconfig::setHttpsPrivateKeyFile(const COLstring& FilePath){
   m_HttpsPrivateKeyFile = IWEBfileMakeRelative(IWEBfileCorrectFileName(FilePath));
}

void IWEBconfig::setHttpsCertificateKeyFile(const COLstring& FilePath){
   m_HttpsCertificateFile = IWEBfileMakeRelative(IWEBfileCorrectFileName(FilePath));
}

COLostream& operator<<(COLostream& Stream, const IWEBconfig& Config){
   Stream << "Web config:" << newline
          << Config.webPort();
   // TODO - more.
   return Stream;
}
