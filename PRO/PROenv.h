#ifndef __PRO_ENV_H__
#define __PRO_ENV_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROenv
//
// Description:
//
// Interact with environment variables in a cross-platform manner 
//
// Author: Nicolas Lehman
// Date:   18/07/19
//---------------------------------------------------------------------------

class COLstring;
class COLvar;

// Sets the given environment variable
void PROsetenv(const COLstring& Key, const COLstring& Value);
void PROunsetenv(const COLstring& Key);

// Gets the given environment variable if it exists, returns "" if empty
COLstring PROgetenv(const COLstring& Key);

// This function will give a complete map of all the environmental variables.
void PROfullEnv(COLvar* pOut);

#endif // End defensive include
