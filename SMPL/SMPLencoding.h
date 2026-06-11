#ifndef __SMPL_ENCODING_H__
#define __SMPL_ENCODING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SMPLencoding
//
// Author: John Q
//
// Description:
//
// Utility functions fro encoding and decoding special characters in sample
// data.
//---------------------------------------------------------------------------

class COLstring;
class COLvar;

// Encode raw message data so it can be used by the front end
void SMPLencode(const COLstring& Message, COLstring* pOut);

// Decode message data from the front end so it can be saved to disk
void SMPLdecode(const COLstring& Message, COLstring* pOut);

#endif // end of defensive include
