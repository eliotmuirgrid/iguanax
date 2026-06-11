#ifndef __EDI_RANDOM_POPULATE_H__
#define __EDI_RANDOM_POPULATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIrandomPopulate
//
// Description:
//
// Code which randomly populates a EDIdata tree with data
//
// Author: Eliot Muir 
// Date:   Tuesday 15 October 2024 - 02:17PM
// ---------------------------------------------------------------------------

class EDIdata;

void EDIrandomPopulate(EDIdata* pData);

void EDIsegmentPopulate(EDIdata* pData);

#endif // end of defensive include
