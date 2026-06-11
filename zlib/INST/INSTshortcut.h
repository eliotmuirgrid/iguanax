#ifndef __INST_SHORTCUT_H__
#define __INST_SHORTCUT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTshortcut
//
// Description:
//
// Install a desktop shortcut for opening Iguana dashboard in browser
//
// Author: Eliot Muir
// Date:   Tuesday 11 July 2023 - 08:31AM
// ---------------------------------------------------------------------------

class COLstring;

bool INSTcreateDesktopShortcut(const COLstring& ExePath);

#endif // end of defensive include
