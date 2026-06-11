#ifndef __WEB_LOGOUT_H__
#define __WEB_LOGOUT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBlogout
//
// Description:
//
// Seperated logout concern for easier tracing
//
// Author: Eliot Muir
// Date:   Thu  1 Sep 2022 12:00:18 EDT
//---------------------------------------------------------------------------

#include <COL/COLmap.h>

class COLwebRequest;
class COLstring;
class USRuser;
class WEBsession;

void WEBlogout(const COLwebRequest& RequestData, COLmap<COLstring, WEBsession>* pSessionMap, COLmap<COLstring, USRuser>* pUserMap);

#endif // end of defensive include
