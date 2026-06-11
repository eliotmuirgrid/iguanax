#ifndef __FSB_RESOLVE_PATH_H__
#define __FSB_RESOLVE_PATH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBresolvePath
//
// Author: Matthew Nitsopoulos
//
// Description: This is used to resolve the command line commands used
//              in the file browser
//
// /file_browser/resolve_path API call.
//---------------------------------------------------------------------------

class COLwebRequest;

void FSBfileBrowserResolvePath(const COLwebRequest& Request);

#endif // end of defensive include