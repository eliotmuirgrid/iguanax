#ifndef __CXML_PARSE_H__
#define __CXML_PARSE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CXMLparse
//
// Author: Eliot Muir
//
// Description:
//
// Code to parse XML using expat into a COLvar
//---------------------------------------------------------------------------

class COLstring;
class COLvar;

bool CXMLparse(const char* pXml, int Size, COLvar* pResult);

#endif // end of defensive include
