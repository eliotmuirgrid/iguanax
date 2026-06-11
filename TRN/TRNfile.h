#ifndef __TRN_FILE_H__
#define __TRN_FILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNfile
//
// Description:
//
// TRNfile - represents a Lua file in the annotation data.
//
// Author: Eliot Muir
// Date:   Friday, November 19th, 2010 @ 10:34:07 AM
//---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLhashmap.h>

class TRNfunction;
class TRNfile {
public:
   TRNfile(const COLstring& FilePath, const COLstring& Label);
   TRNfile() = default;
   ~TRNfile();

   COLmap<int, TRNfunction*> m_FunctionList;
   COLstring m_Source;
   COLstring m_FilePath;
   COLstring m_Label;
   bool m_FileMissing; //If true, project file not found and a placeholder loaded instead.
private:
   TRNfile(const TRNfile& Orig); // not allowed
   TRNfile& operator=(const TRNfile& Orig); // not allowed
};

const TRNfunction* TRNfileFunctionAtLine(const TRNfile& File, int Start);

void TRNclearFunctionMap(TRNfile* pFile);
 
// This function will find or create a function at the right line
TRNfunction* TRNfindOrCreateFunction(TRNfile* pFile, int Start);

// This one just finds and may return null.
TRNfunction* TRNfunctionAtLine(TRNfile* pFile, int Start);

bool      TRNcheckUnreachableBlocks(TRNfile* pFile);
void      TRNcalcMaxLineLength     (TRNfile* pFile);
COLstring TRNblankFile();
COLstring TRNunsupportedFile();
bool      TRNisSampleData(const COLstring& FileName);

COLostream& operator<<(COLostream& Stream, const TRNfile& pFile);
COLostream& operator<<(COLostream& Stream, const COLhashmap<COLstring, TRNfile*>& FileList);
#endif // end of defensive include