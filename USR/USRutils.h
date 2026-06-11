#ifndef USRUTILS_H
#define USRUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/12/24 10:21 AM
//  ---------------------------------------------------------------------------
class COLstring;

bool USRdoesContainDigit(const COLstring& S);
bool USRvalidPassword(const COLstring& Password, COLstring* pErr);

#endif // end of defensive include
