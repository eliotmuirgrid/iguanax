#ifndef __DB_VMD_PARSER_H__
#define __DB_VMD_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Routine to parse vmd in XML into the new object model.
//
// Author:   Vismay Shah
// Date:     13th March 2023
//---------------------------------------------------------------------------

class COLstring;
class DBcollection;

bool DBtranslateVmd(const COLstring& VmdContent, DBcollection* pOutput);

#endif // end of defensive include
