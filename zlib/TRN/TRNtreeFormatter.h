#ifndef __TRN_TREE_FORMATTER_H__
#define __TRN_TREE_FORMATTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtreeFormatter
//
// Description:
//
// TRNtreeFormatter class.  For traversing and formatting (for display)
// NOD trees.
// Note that this class was moved from NWBtreeFormatter.
//
// Author: Kevin Senn
// Date:   Friday, June 25th, 2010 @ 09:27:47 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLauto.h>
#include <COL/COLarray.h>
#include <COL/COLref.h>

#include <NOD/NODplace.h>

// TODO - this interface is dumb - it should just be a virtual method on the NODplace itself
// Eliot
class TRNtreeFormatter 
{
public:
   TRNtreeFormatter();
   virtual ~TRNtreeFormatter() {}

   // GrammarIndex is the index of this node relative to its parent.
   // Use -1 when index is not applicable.
   void pushNode(const NODplace& Node, int GrammarIndex);
   // After calling popNode(), results of collapsedView() and expandedView()
   // may not be valid (until another node is pushed).
   void popNode();

   COLstring collapsedView() const { return view(false); }
   COLstring expandedView() const  { return view(true); }

protected:
   COLarray< const NODplace* > m_Stack;
   int m_TopIndex; // the "grammar index" of the top node on the stack

   virtual COLstring view(bool Expanded) const = 0;

   const NODplace& topNode() const;
   const NODplace* secondFromTopNode() const;

private:
   TRNtreeFormatter(const TRNtreeFormatter& Orig); // not allowed
   TRNtreeFormatter& operator=(const TRNtreeFormatter& Orig); // not allowed
};

COLauto<TRNtreeFormatter> TRNgetFormatter(const NODplace& Place);


// Lua Table Tree formatter
class TRNluaFormatter : public TRNtreeFormatter
{
public:
   TRNluaFormatter() : TRNtreeFormatter() {}

protected:
   COLstring view(bool Expanded) const;
};

// HL7 Tree formatter (for NOH library)                                 
class TRNnohFormatter : public TRNtreeFormatter
{
public:
   inline TRNnohFormatter() : TRNtreeFormatter() {}

protected:
   COLstring view(bool Expanded) const;
};

// XML (DOM) Tree formatter (for NOX library)                           
class TRNnoxFormatter : public TRNtreeFormatter
{
public:
   TRNnoxFormatter() : TRNtreeFormatter() {}

protected:
   COLstring view(bool Expanded) const;
};

// DB Tree formatter (for NODB library)
class TRNnodbFormatter : public TRNtreeFormatter
{
public:
   TRNnodbFormatter() : TRNtreeFormatter() {}

protected:
   virtual COLstring view(bool Expanded) const;
};

// Table Grammar Tree formatter (for NOTG library) 
class TRNnotgFormatter : public TRNtreeFormatter
{
public:
   TRNnotgFormatter() : TRNtreeFormatter() {}

protected:
   virtual COLstring view(bool Expanded) const;
};

// Table Tree formatter (for NTAB library)              
class TRNntabFormatter : public TRNtreeFormatter
{
public:
   TRNntabFormatter() : TRNtreeFormatter() {}

protected:
   virtual COLstring view(bool Expanded) const;
};

// *new* exposed
void TRNprintCountAndUnits(COLostream& Stream, int Count, const char* pUnit);

#endif // end of defensive include
