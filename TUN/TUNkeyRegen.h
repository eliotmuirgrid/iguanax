#ifndef TUNKEYREGEN_H
#define TUNKEYREGEN_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TUNkeyRegen.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   27/01/25 1:42 PM
//  ---------------------------------------------------------------------------
class TUNstateClient;
class COLwebRequest;

// this api only updates the key on disk -- an instance restart is called directly after
void TUNkeyRegen(const COLwebRequest& Request, TUNstateClient* pClient);

#endif // end of defensive include
