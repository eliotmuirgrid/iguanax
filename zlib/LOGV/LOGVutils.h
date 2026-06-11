#ifndef LOGVUTILS_H
#define LOGVUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/04/25 11:56 AM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;

bool LOGVisolateHL7Message(const COLstring& Input, COLstring* pMessage);
void LOGVparseId(COLvar* pResult);

#endif // end of defensive include
