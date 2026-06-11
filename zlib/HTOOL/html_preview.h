#ifndef __html_preview_H__
#define __html_preview_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: html_preview
//
// Description:
//
// Parses a file off disk and gives the result back as formatted HTML.
//
//
// Author: Jon Scalise
// Date:   Thu 14 Jan 2016 17:21:43 EST
//---------------------------------------------------------------------------

class COLstring;

void htmlpreview(const COLstring& FullFilePath, COLstring* pResult);

#endif // end of defensive include
