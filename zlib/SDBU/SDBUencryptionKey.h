#ifndef SDBUENCRYPTIONKEY_H
#define SDBUENCRYPTIONKEY_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUencryptionKey.h
//
//  Description: See IX-4068 for why we have this
//
//  Author: Matthew Sobkowski
//  Date:   11/07/25 2:24 PM
//  ---------------------------------------------------------------------------
class COLstring;

void SDBUsetEncryptionKeyGlobal(const COLstring& EncryptionKey);
COLstring SDBUencryptionKeyGlobal();

#endif // end of defensive include
