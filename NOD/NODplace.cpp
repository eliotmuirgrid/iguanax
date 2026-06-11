//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODplace
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, September 21st, 2010 @ 11:58:54 AM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODplace.h"
#include "NODoperation.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;


bool NODplace::isSimple() const{
   return grammarType() == NOD_SIMPLE;
}

bool NODplace::isVector() const{
   COL_METHOD(NODplace::isVector());
   return grammarType() == NOD_VECTOR;
}

bool NODplace::isComplex() const{
   return grammarType() == NOD_COMPLEX;
}

bool NODplace::canCreateDummyItem() const{
   COL_METHOD(NODplace::canCreateDummyItem);
   // Very important that these are short circuit evalations - if we accidentally try to call isHomogeneous() on a NODcomplex type node, we crash
   return isVector() && isNull() && isHomogeneous();
}

bool NODplace::setTreeValue(const COLstring& TreeValue){
   if (isReadOnly()){
      COL_ERROR_STREAM_PLAIN(COL_T("This tree is read-only"), NOD_IS_READ_ONLY);
   }
   return false;
}

const COLstring NODplace::NOD_EMPTY_SIMPLE_VALUE("");

COLostream& operator<<(COLostream& Stream, const NODplace& Place){
   switch (Place.grammarType()){
   case NOD_COMPLEX:
      {
         Stream << indent;
         int CountOfMember = Place.size();
         for (int i=0; i < CountOfMember; i++)
         {
            const NODplace& Child = Place.child(i);
            Stream << newline << (i + 1) << COL_T("] ") << Child.name() << Child;
         }
         Stream << unindent << newline;
      }
      break;
   case NOD_SIMPLE:
      Stream << COL_T(" \'") << Place.value() << '\'';
      break;
   case NOD_VECTOR:{
         Stream << "[]";
         int CountOfItem = Place.size();
         for (int i=0; i < CountOfItem; i++){
            const NODplace& Child = Place.child(i);
            Stream << Child;
         }
      }
      break;
   };
   return Stream;
}

COLstring NODmemberList(const NODplace& Place){
   int NamedMembers = Place.size();
   int i;
   for (i = NamedMembers-1; i >= 0; i--){
      if (!Place.child(i).isNamed()) NamedMembers--;
   }

   switch(NamedMembers){
   case 0:
      return COLstring();
   case 1:
      return COLstring("Named member is ") + Place.child(0).name();
   }

   COLstring List; 
   COLostream Stream(List);
   Stream << COL_T("Named members are ");
   for (i = 0; i < Place.size(); i++){
      if (i) Stream << COL_T(", ");
      Stream << Place.child(i).name();        
   }
   Stream << '.';
   return List;
}

void NODthrowNoSuchMember(const NODplace& Place, const COLstring& MemberName){
   COL_ERROR_STREAM_PLAIN('\'' << MemberName << COL_T("' member does not exist. ") << NODmemberList(Place), NOD_NO_SUCH_MEMBER);
}
