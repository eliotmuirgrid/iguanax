#ifndef __INI_DATA_H__
#define __INI_DATA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIdata
//
// Description:
//
// POD datastructure to represent a INI/GIT style configuration file.
//
// Author: Eliot Muir 
// Date:   Friday 24 March 2023 - 08:43AM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLstring.h>

class INIdata;

// It's best to use these public accessor functions to read/write to a configuration file since they
// result in cleaner less error prone code.
// GIT ini files use:
// [section "subsection"]
//    key = value      
COLstring INIdataKeyGet   (const INIdata& Data, const COLstring& Section, const COLstring& Subsection, const COLstring& Key); // will return empty string if not present
void      INIdataKeySet   (INIdata* pData,      const COLstring& Section, const COLstring& Subsection, const COLstring& Key, const COLstring& Value); 
void      INIdataKeyRemove(INIdata* pConfig,    const COLstring& Section, const COLstring& SubSection, const COLstring& Key);
void INIdataSectionRemove (INIdata* pConfig,    const COLstring& Section, const COLstring& SubSection="");

class INIdata{
public:
   COLmap<COLstring, COLmap<COLstring, COLstring> > PrivateData;
};

COLostream& operator<<(COLostream& Stream, const INIdata& Data);

void INIformat(const INIdata& Config, COLstring* pOut);
void INIparse(const COLstring& Input, INIdata* pConfig);

#endif // end of defensive include