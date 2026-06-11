#ifndef __FSB_EXTRACT_PATH_H__
#define __FSB_EXTRACT_PATH_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBvolumes
//
// Author: Matthew Nitsopoulos
//
// Description:
//
// /file_browser/extract_path API call.
//---------------------------------------------------------------------------

class COLwebRequest;

void FSBfileBrowserExtractPathImp(COLvar& Result, const COLstring& Path); // exposed for unit test
void FSBfileBrowserExtractPath(const COLwebRequest& Request);

#endif // end of defensive include