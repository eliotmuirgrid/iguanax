#ifndef __GITW_IS_WRITABLE_H__
#define __GITW_IS_WRITABLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWisWritable
//
// Description:
//
// API to determine if the user has write access to a given repo.
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 09:42AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITWisWritable(const COLwebRequest& Request);

#endif // end of defensive include
