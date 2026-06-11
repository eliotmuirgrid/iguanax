#ifndef __NOB_LABELS_H__
#define __NOB_LABELS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBlabels
//
// Description:
//
// Defines labels for use in NOBserialize() and NOBparse().
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 11:53:42 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <TSM/TSMlabel.h>

// Legacy labels - for NOBnode, the "all-in-one" class, NOBnode.
extern const TSMlabel NOBlegacyNodeLabel;
extern const TSMlabel NOBlegacyDummyItemLabel;

extern const TSMlabel NOBcompositeNodeLabel;
extern const TSMlabel NOBsimpleNodeLabel;
extern const TSMlabel NOBdummyCompositeNodeLabel;
extern const TSMlabel NOBdummySimpleNodeLabel;
extern const TSMlabel NOBsimpleEmptyNodeLabel;

extern const TSMlabel NOBgrammarTypeLabel;
extern const TSMlabel NOBnodeTypeLabel;
extern const TSMlabel NOBprotocolTypeLabel;
extern const TSMlabel NOBnodeTypeNameLabel;
extern const TSMlabel NOBprotocolNameLabel;
extern const TSMlabel NOBnameLabel;
extern const TSMlabel NOBisNullLabel;
extern const TSMlabel NOBvalueLabel;
extern const TSMlabel NOBisHomogeneousLabel;
extern const TSMlabel NOBfavorIndexByNameLabel;
extern const TSMlabel NOBgracefulNoSuchMemberLabel;
extern const TSMlabel NOBisNamedLabel;
extern const TSMlabel NOBshouldEscapeValue;
extern const TSMlabel NOBflagsLabel;

extern const TSMlabel NOBmemoizerLabel;
extern const TSMlabel NOBmemoizerEntryLabel;
extern const TSMlabel NOBmemoizerEntryIdLabel;
extern const TSMlabel NOBmemoizerEntryValueLabel;
extern const TSMlabel NOBmemoizerProtocolLabel;
extern const TSMlabel NOBmemoizerNodeTypeLabel;

enum NOBserialCode{
   NOB_NODE = 0,
   NOB_DUMMY_NODE = 1,
   NOB_COMPOSITE_NODE = 2,
   NOB_SIMPLE_NODE = 3,
   NOB_DUMMY_COMPOSITE_NODE = 4,
   NOB_DUMMY_SIMPLE_NODE = 5,
   NOB_SIMPLE_EMPTY_NODE = 6,
   NOB_GRAMMAR_TYPE=100,
   NOB_NODE_TYPE=101,
   NOB_PROTOCOL_TYPE=102,
   NOB_NODE_TYPE_NAME=103,
   NOB_PROTOCOL_TYPE_NAME=104,
   NOB_NAME=105,
   NOB_IS_NULL=107,
   NOB_VALUE=108,
   NOB_IS_HOMOGENEOUS=109,
   NOB_FAVOR_INDEX_BY_NAME=111,
   NOB_GRACEFUL_NO_SUCH_MEMBER=112,
   NOB_IS_NAMED=113,
   NOB_SHOULD_ESCAPE_VALUE=114,
   NOB_FLAGS=115,
   NOB_MEMOIZER=200,
   NOB_MEMOIZER_ENTRY=201,
   NOB_MEMOIZER_ENTRY_ID=202,
   NOB_MEMOIZER_ENTRY_VALUE=203,
   NOB_MEMOIZER_PROTOCOL=204,
   NOB_MEMOIZER_NODE_TYPE=205
};

#endif // end of defensive include
