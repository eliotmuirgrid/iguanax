#ifndef __NODL_PREVIEW_LISTENER_H__
#define __NODL_PREVIEW_LISTENER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLpreviewListener
//
// Description:
//
// NODLpreviewListener class.
//
// Author: Kevin Senn
// Date:   Monday, February 14th, 2011 @ 04:19:00 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
class COLstring;
struct lua_State;
class NODplace;

class NODLpreviewListener {
public:
   // Warning - these values are referenced in log.css.
   enum ePreviewType{
      DB_TABLES = 0,
      SQL_STRING = 1
   };

   NODLpreviewListener() {}
   virtual ~NODLpreviewListener() {}

   // The meaning of the ExtraArg depends on the preview type. It may be not required at all.
   virtual void onPreviewOutput(const COLstring& StringOutput, ePreviewType PreviewType, const COLstring& ExtraArg) = 0;
   virtual void onPreviewOutput(const NODplace&  TreeOutput,   ePreviewType PreviewType) = 0;

private:
   NODLpreviewListener(const NODLpreviewListener& Orig); // not allowed
   NODLpreviewListener& operator=(const NODLpreviewListener& Orig); // not allowed
};

void NODLpreview(lua_State* L, const NODplace&  TreeOutput,   NODLpreviewListener::ePreviewType PreviewType);
void NODLpreview(lua_State* L, const COLstring& StringOutput, NODLpreviewListener::ePreviewType PreviewType, const COLstring& ExtraArg);

#endif // end of defensive include
