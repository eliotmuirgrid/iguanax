#ifndef __TRN_HL7_FORMATTER_H__
#define __TRN_HL7_FORMATTER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhl7Formatter
//
// Description:
//
// Class to format new EDI based node tree.
//
// Author: Eliot Muir 
// Date:   Sunday 09 October 2022 - 05:05PM
// ---------------------------------------------------------------------------

#include <TRN/TRNtreeFormatter.h>

static const int TRN_HL7 = 101;

// IX-974 This doesn't feel like the best design since this feels like it should be part of NODplace virtual interface?
class TRNhl7Formatter : public TRNtreeFormatter{
public:
   COLstring view(bool Expanded) const;
   void formatTree(COLostream& ResultStream, const NODplace& Node, bool Expanded) const;
};

#endif // end of defensive include