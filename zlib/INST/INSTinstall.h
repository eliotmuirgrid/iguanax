#ifndef __INST_INSTALL_H__
#define __INST_INSTALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTinstall
//
// Description:
//
// Single helper to install IguanaX - this is so we are consistent with our
// choices of description, name etc.
//
// Author: Eliot Muir 
// Date:   Sunday 13 August 2023 - 07:12AM
// ---------------------------------------------------------------------------

class COLstring;

bool INSTinstall(const COLstring& Exe, const COLstring& User, const COLstring& Password, COLstring* pError);

#endif // end of defensive include
