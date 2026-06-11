// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDlistLocalIPs
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Thu 24 Aug 2023 12:37:22 PDT
/// ---------------------------------------------------------------------------

#include <DBD/DBDlistLocalIPs.h>

#include <SCK/SCKaddress.h>
#include <SCK/SCKutils.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDlistLocalIPs() {
   COL_FUNCTION(DBDlistLocalIPs);
   COLstring HostName = SCKlocalHostName();
   COLcout << "Local host '" << HostName << "' resolves to the following IP address(es):" << newline;
   COLarray<SCKaddress> AllIps;
   SCKhostResolveAllIps(HostName, &AllIps);
   for (int i = 0; i < AllIps.size(); ++i) {
      COLcout << "   * " << AllIps[i].toString() << newline;
   }
}
