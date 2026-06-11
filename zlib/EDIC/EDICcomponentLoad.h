#ifndef __EDIC_COMPONENT_LOAD_H__
#define __EDIC_COMPONENT_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDICcomponentLoad
//
// Description:
//
// Loads EDI file, because of dependencies on CMP we have it in this library.
//
// Author: Eliot Muir 
// Date:   Wednesday 19 October 2022 - 01:49PM
// ---------------------------------------------------------------------------

class COLwebRequest;

void EDICcomponentLoad(const COLwebRequest& Request);

#endif // end of defensive include
