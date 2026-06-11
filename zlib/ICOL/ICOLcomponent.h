#ifndef ICOLCOMPONENT_H
#define ICOLCOMPONENT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLcomponent.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 3:52 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

void ICOLcomponentInit();
void ICOLcomponentFree();
void ICOLcomponentToVar(COLvar* pOut);

// Intentionally not const COLstring& since we call them in a closure
void ICOLcomponentAdd(const COLstring Component, const COLstring Name, const COLstring Value);
void ICOLcomponentRemove(const COLstring Component, const COLstring Name);

void ICOLcomponentGet(const COLstring& Component, COLvar* pOut);


#endif // end of defensive include
