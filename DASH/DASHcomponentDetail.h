#ifndef __DASH_COMPONENT_DETAIL_H__
#define __DASH_COMPONENT_DETAIL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DASHcomponentDetail
//
// Description:
//    Gets a smaller list of component data for the card
//    Implementation
//
// Author: Matthew Nitsopoulos
// Date:   Wednesday August 24 2022
//---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class COLstring;

void DASHcomponentDetail(const COLwebRequest& Request, CFGmap* pMap);

#endif // End of defensive include
