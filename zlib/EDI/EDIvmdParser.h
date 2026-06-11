#ifndef __EDI_VMD_PARSER_H__
#define __EDI_VMD_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Routine to parse vmd in XML into the new object model.
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

class COLstring;
class EDIcollection;

struct EDIuniqueNode {
   COLstring _Name;
   EDInode* _pNode;
   EDIuniqueNode(const COLstring& Name, EDInode* pNode) : _Name(Name), _pNode(pNode) {}
};

bool EDIcompare(const EDIuniqueNode& UniqueNode, EDInode* pNode);
int EDIcompareNodes(EDInode* pNode, const COLarray<EDIuniqueNode>& UniqueNodes);
COLstring EDIresolveDuplicateNames(const COLstring& MessageName, const COLstring& Name, EDInode* pNode,
                                   EDIcollection* pOutput, COLmap<COLstring, COLarray<EDIuniqueNode>>* pUniques);

bool EDItranslateVmd(const COLstring& VmdContent, EDIcollection* pOutput, COLstring* pErrorMessage);

#endif // end of defensive include
