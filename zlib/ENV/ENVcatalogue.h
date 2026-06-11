#ifndef __ENV_CATALOGUE_H__
#define __ENV_CATALOGUE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVcatalogue
//
// Description:
//
// This returns a COLvar of environmental variables.
//
// Author: Eliot Muir 
// Date:   Thursday 23 March 2023 - 11:50AM
// ---------------------------------------------------------------------------

class COLvar;
class COLwebRequest;
void ENVcatalogue(const COLwebRequest& Request);

// Populates the var with the local and system vars with local vars taking priority
void ENVretrieveAll(COLvar* pResult);

#endif // end of defensive include
