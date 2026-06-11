#ifndef __XML_PRECOMP_H__
#define __XML_PRECOMP_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc. All Rights Reserved
//
// Description:
//
// Precompiled header file
//
// Author: Eliot Muir
// Date:   Fri 06/08/1999 
//
//---------------------------------------------------------------------------

#include <COL/COLerror.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef XML_DEBUG
#  define XML_LOG(Message)       { COLstring XmlLog; COLostream Stream(XmlLog); Stream << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; COLcout << XmlLog; }
#else
#  define XML_LOG(Message)
#endif


#endif // end of defensive include
