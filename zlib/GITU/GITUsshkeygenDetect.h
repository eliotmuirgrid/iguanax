#ifndef __GITU_SSHKEYGEN_DETECT_H__
#define __GITU_SSHKEYGEN_DETECT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUsshkeygenDetect
//
// Description:
//
// Code to go and find ssh-keygen on a system
//
// Author: Eliot Muir
// Date:   Wednesday 08 March 2023 - 02:25PM
// ---------------------------------------------------------------------------

class COLstring;

// This locates the ssh-keygen utility on the system.
COLstring GITUsshkeygenDetect();

COLstring GITUsshDetect();

#endif // end of defensive include
