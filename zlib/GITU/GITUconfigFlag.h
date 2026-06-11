#ifndef __GITU_CONFIG_FLAG_H__
#define __GITU_CONFIG_FLAG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUconfigFlag
//
// Description:
//
// Some git commands require values saved in the gitconfig file that is stored on
// a per user basis.
//
// Author: Tyler Brown 
// Date:   Wednesday 15 November 2023 - 01:17PM
// ---------------------------------------------------------------------------

class COLstring;

COLstring GITUconfigFlag(const COLstring& Username);


#endif // end of defensive include
