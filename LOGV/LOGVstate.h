#ifndef LOGVSTATE_H
#define LOGVSTATE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LOGVstate.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/25 1:55 PM
//  ---------------------------------------------------------------------------
class TRNannotationState;

// use on worker thread
bool LOGVinitState(const COLvar& RequestData, TRNannotationState* pState, COLvar* pOut, COLstring* pErr);

#endif // end of defensive include
