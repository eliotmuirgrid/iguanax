#ifndef __html_diff_H__
#define __html_diff_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: htmldiff
//
// Description:
//
// An adaption off the htmldiff command line application that diffs two files and 
// gives the result in html format. 
//
//
// Author: Jon Scalise
// Date:   Tue 21 Jul 2015 12:14:47 EDT
//
// Adapted to library: Jon Scalise
// Date:   Tue 21 Jul 2015 12:14:47 EDT
//---------------------------------------------------------------------------

class COLstring;
class COLvar;

void HTOOLhtmldiff(const COLstring& FileA, const COLstring& FileB, COLvar* pDiffResult);
void HTOOLhtmldiffFileContent(COLstring FileAContent, const COLstring& FileA, COLstring FileBContent, const COLstring& FileB, COLvar* pDiffResult);
bool HTOOLreadAsciiFile(const COLstring &Name, COLstring *pString);
#endif // end of defensive include
