#ifndef __EDI_FORMAT_H__
#define __EDI_FORMAT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIformat
//
// Description:
//
// Experiment in an HL7 formatter for nodes
//
// Author: Eliot Muir 
// Date:   Wednesday 05 October 2022 - 08:29PM
// ---------------------------------------------------------------------------

class COLstring;
class EDIdata;
class EDIcollection;
void EDIescapeData(COLstring* pOut, const COLstring& Data);
void EDIformatLeaf(const EDIdata& Data, COLstring* pOut);

void EDIformatSubField      (const EDIdata& Data, COLstring* pOut);
void EDIformatField         (const EDIdata& Data, COLstring* pOut);
void EDIformatRepeatingField(const EDIdata& Data, COLstring* pOut);
void EDIformatSegment       (const EDIdata& Data, COLstring* pOut);
void EDIformatMessage       (const EDIdata& Data, COLstring* pOut);



#endif // end of defensive include
