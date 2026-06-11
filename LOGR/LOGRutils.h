#ifndef LOGRUTILS_H
#define LOGRUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGRutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/07/25 12:00 PM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;

COLstring LOGRgetDefaultDir(const COLvar& LogConfig);
bool LOGRreadConfig(COLvar* pConfig, COLstring* pError);
#endif // end of defensive include
