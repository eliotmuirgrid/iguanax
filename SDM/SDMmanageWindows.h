#ifndef __SDM_MANAGE_WINDOWS_H__
#define __SDM_MANAGE_WINDOWS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMmanageWindows
//
// Description:
//
// Helper functions used for SDMmanage windows implementation.
//
// Author: Eliot Muir
// Date:   Thu 08/10/2023 
// ---------------------------------------------------------------------------

bool SDMopenManager(DWORD Permission, SC_HANDLE* pManager, COLstring* pError);
bool SDMopenService(SC_HANDLE Manager, const COLstring& Name, DWORD Permission, SC_HANDLE* pService, COLstring* pError);
void SDMcleanUpServiceManager(SC_HANDLE* pService, SC_HANDLE* pManager);

#endif // end of defensive include
