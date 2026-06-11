// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITgenerateKeyPair
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 22 November 2023 - 01:37PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GIT/GITgenerateKeyPair.h>
#include <GITU/GITUsshkeygenDetect.h>
#include <PRO/PROenv.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

COLstring GITCkeyTypeOverride(){
   COL_FUNCTION(GITCkeyTypeOverride);
   COLstring OverrideType = PROgetenv("IFW_SSH_GEN_TYPE");
   if (!OverrideType.is_null()) { return OverrideType; }
   return "";
}

bool GITgenerateKeyPair(const COLstring& PrivateKey, COLstring* pOutput, const COLstring& KeyType){
   COL_FUNCTION(GITgenerateKeyPair);
   COL_VAR2(PrivateKey, KeyType);
   COLstring KeyFormat = GITCkeyTypeOverride().is_null() ? KeyType : GITCkeyTypeOverride();
   COL_TRC("Generating " << KeyFormat << " type ssh key pair");
   COL_TRC("Running command...");
   COLstring PrivateKeyFile = GITUstripPathsQuotes(PrivateKey);
   FILcreateDirectoryTreeToFile(PrivateKeyFile, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   COLstring SshBinary = GITUsshkeygenDetect();
   if (SshBinary.is_null()){
      *pOutput = "Could not find ssh-keygen to generate SSH key.  Please make sure git is installed and on your system path.";
      return false;
   }
   if (SshBinary.find(" ") != npos){
      SshBinary = "\"" + SshBinary + "\"";
   }
   COL_VAR(SshBinary);
   COLstring Command = SshBinary + " -t " + KeyFormat + R"( -C "created by IguanaX" -q -N "" -f ")" + PrivateKeyFile + "\"";
   COLstring Err;
   COL_VAR(Command);
   int ExitCode = PROexecuteString(".", Command, pOutput, &Err);
   COL_VAR2(ExitCode, *pOutput);
   bool Success = ExitCode == 0;
   if(!Success) { pOutput->append("Problem generating the ssh key: " + Err); }
   return Success;
}
