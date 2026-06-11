#ifndef __EDI_FORMAT_HTML_H__
#define __EDI_FORMAT_HTML_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIformatHtml
//
// Description:
//
// Format HL7 node tree with prettyish HTML
//
// Author: Eliot Muir 
// Date:   Monday 24 October 2022 - 02:44PM
// ---------------------------------------------------------------------------

class COLstring;
class EDIdata;
class EDIcollection;

void EDIformatLeaf(const EDIdata& Data, COLstring* pOut);

void EDIformatSubFieldHtml      (const EDIdata& Data, COLstring* pOut);
void EDIformatFieldHtml         (const EDIdata& Data, COLstring* pOut);
void EDIformatRepeatingFieldHtml(const EDIdata& Data, COLstring* pOut);
void EDIformatSegmentHtml       (const EDIdata& Data, COLstring* pOut);
void EDIformatMessageHtml       (const EDIdata& Data, COLstring* pOut);

#endif // end of defensive include
