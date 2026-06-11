#ifndef __CON_CHANNEL_H__
#define __CON_CHANNEL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 2024 Eliot Muir  All Rights Reserved
//
// Module: CNVchannel
//
// Description:
//
// Iterates through channel files.
//
// Author: Eliot Muir 
// Date:   Sunday 15 December 2024 - 05:43PM
// ---------------------------------------------------------------------------

class COLostream;
class COLvar;
class COLstring;

COLstring CNVchannelName       (const COLvar& Channel);
COLstring CNVchannelDescription(const COLvar& Channel);
bool      CNVstartAutomatically(const COLvar& Channel);

void CNVchannelIterate(COLostream& Stream);

#endif // end of defensive include
