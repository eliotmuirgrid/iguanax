#ifndef __REX_EXAMPLE_PRE_COMP_H__
#define __REX_EXAMPLE_PRE_COMP_H__
//---------------------------------------------------------------------------
// Description:
//
// Standard precompiled header include file
// 
// Author: Eliot Muir
// Date:   Fri 01/16/2004 
//
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <COL/COLerror.h>
#include <COL/COLostream.h>

// #define REX_EXAMPLE_DEBUG

#ifdef REX_EXAMPLE_DEBUG
#   define REX_EXAMPLE_LOG(X) { COLcout << X << newline; }
#else
#   define REX_EXAMPLE_LOG(X) 
#endif

#endif // end of defensive include
