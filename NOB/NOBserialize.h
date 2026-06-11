#ifndef __NOB_SERIALIZE_H__
#define __NOB_SERIALIZE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBserialize
//
// Description:
//
// NOBserialize() will serialize any NOD tree into a binary format readable
// by NOBparse().
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 11:43:03 AM
//---------------------------------------------------------------------------

class NODplace;
class COLstring;

void NOBserialize(const NODplace& RootPlace, COLstring& Destination);

#endif // end of defensive include
