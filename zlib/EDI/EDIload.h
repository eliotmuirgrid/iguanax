#ifndef __EDI_LOAD_H__
#define __EDI_LOAD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Routine to load grammar from JSON input as would come from serialized JSON file
// on disc.
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

class COLstring;
class EDIcollection;

bool EDIisBinaryVmd(const COLstring& Content);
bool EDIload(const COLstring& JsonContent, EDIcollection* pOutput, COLstring* pError);

// Really just for testing.  This function strips off Javascript parsely we have 
// in the files like ../Iguana/app/web/HL7/HL7_2_7.js
bool EDIloadWeb(const COLstring& FileName, EDIcollection* pCollection, COLstring* pError);

// This can handle vmd and both JSON formats
bool EDIsmartLoad(const COLstring& Content, EDIcollection* pOutput, COLstring* pError, COLstring* pWarning, bool* pLegacyWarning);

#endif // end of defensive include
