#ifndef __INST_SERVICE_INSTALL_H__
#define __INST_SERVICE_INSTALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTserviceInstall
//
// Description: Portion of installer that installs service under local system or user account
//
// Author: Matthew Sobkowski
// Date:   Monday 19 June 2023 - 11:17AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void INSTserviceInstall(const COLwebRequest& Request);


#endif // end of defensive include
