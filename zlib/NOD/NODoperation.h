#ifndef __NOD_OPERATION_H__
#define __NOD_OPERATION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODoperation
//
// Description:
//
// NODoperation - routines to operate on nodes.
//
// Author: Kevin Senn
// Date:   Thursday, May 13th, 2010 @ 02:25:46 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;
class NODaddress;

enum NODerrorCode
{
   NOD_ESUCCESS=0,
   NOD_NO_SUCH_MEMBER=1,
   NOD_NOT_COMPLEX_TYPE=2, //means this is a vector of leaves or a vector of vectors
   NOD_IS_LEAF=3,
   NOD_IS_VECTOR=4, // Attempt made to assign to a vector.
   NOD_INDEX_OUT_OF_BOUNDS=5,
   NOD_IS_READ_ONLY=6, // Attempt to modify a read-only tree
   // Should be thrown with a simple comma-delimited list of expected arguments. e.g. "index, type".
   // If the function accepts no arguments, then the description should be an empty string.
   NOD_BAD_ARGUMENTS=7
};

//COLstring NODerrorCodeAsString(NODerrorCode Code);

// Will throw if the source and destination nodes are incompatible (if they don't have
// have the exact same grammar).
void NODcopyNode(const NODplace& Source, NODplace& Destination);

// Will map data from Source to Destination, even if grammars don't match.
// Does not clear branches in the destination that are null in the source.
void NODmapTree(const NODplace& Source, NODplace& Destination);

// Basically the same as mapTree, but Source and Destination must be Vector and/or Complex.
// Only map a certain range of children.  
void NODclearRange(NODplace& Place, int Start, int End=-1);
void NODmapRange(const NODplace& Source, NODplace& Destination, int Start, int End = -1);

// Node can be Vector or Complex.  For Vector nodes, children will be removed.
// For Complex nodes, they will be nullified.
void NODclearChild(NODplace& Current, int ChildIndex);

// Node must be Complex.
int NODcountOfNamedChildRepeat(const NODplace& Place, const COLstring& ChildName);
// Returns the index of the specified repeat of a child with the specified name.
// Will throw if no child with the specified name exists, or if there are not
// enough repeats.
int NODgetNamedChildRepeat(const NODplace& Place, const COLstring& ChildName, int RepeatIndex);

#endif // end of defensive include
