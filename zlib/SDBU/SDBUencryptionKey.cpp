//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUencryptionKey.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   11/07/25 2:24 PM
//  ---------------------------------------------------------------------------
#include <SDBU/SDBUencryptionKey.h>
#include <COL/COLstring.h>

COLstring s_SDBU_ENCRYPTION_KEY = "";

void SDBUsetEncryptionKeyGlobal(const COLstring& EncryptionKey) { s_SDBU_ENCRYPTION_KEY = EncryptionKey; }
COLstring SDBUencryptionKeyGlobal()                             { return s_SDBU_ENCRYPTION_KEY; }