#ifndef __DOM_UTILS_H__
#define __DOM_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOMutils
//
// Description:
//
// DOMutils - helper functions
//
// Author: Eliot Muir
// Date:   Friday, September 25th, 2009 @ 03:57:23 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <DOM/DOMdocument.h>
#include <DOM/DOMsink.h>


class DOMnode;
class DOMelementList;

COLstring DOMattributeValue(DOMnode& Node, const COLstring& Name);
void DOMimmediateElementListByTagName(DOMnode& Node, const COLstring& FullNameSpace, const COLstring& Name, DOMelementList* List);
bool DOMisNodeNamedElement(DOMnode* pNode, const COLstring& FullNameSpace, const COLstring& Name);
COLstring DOMnameSpace(DOMnode* pNode);

// returns a COLstring() if the node is not an element or doesn't match the namespace
// otherwise it returns the 
COLstring DOMextractElementUName(DOMnode* pNode, const COLstring& FullNameSpace);
DOMnode* DOMfindChildWithName(DOMnode* pNode, const COLstring& Name);
DOMnode* DOMfindChildWithFullName(DOMnode* pNode, const COLstring& FullNameSpace, const COLstring& Name);

// Returns the first or last element child node, i.e. not text node, etc.
DOMnode* DOMfindFirstElementChild(DOMnode* pParent);
DOMnode* DOMfindLastElementChild(DOMnode* pParent);

// Expat (stream based) parser cannot detect missing closing tags, if appending all
// missing closing tags to the end of a document happens to be valid still. This function can.
COLstring DOMunbalancedTagWarning(const DOMdocument& Document, const DOMsink& Sink);

#endif // end of defensive include
