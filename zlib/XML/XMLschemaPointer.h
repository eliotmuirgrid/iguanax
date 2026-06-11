#ifndef __XML_SCHEMA_POINTER_H__
#define __XML_SCHEMA_POINTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschema
//
// Description:
//
// An owning smart pointer.
//
// Author: Ryan Dewsbury
// Date:   Mon 05/28/2001 
//
//---------------------------------------------------------------------------

template< class TYPE_ >
class XMLschemaPointer
{
public:
   XMLschemaPointer():pObject( 0 ),Owner( false ){}
   XMLschemaPointer( TYPE_* pObject_, bool Owner_ = true ):pObject( pObject_ ), Owner( Owner_ ) {}
   TYPE_& operator=( TYPE_* pObject_ ){ pObject = pObject_; Owner = true; return *this;}
   XMLschemaPointer( const XMLschemaPointer< TYPE_ >& Pointer ):pObject( Pointer.pObject ), Owner( Pointer.Owner ) { ((XMLschemaPointer< TYPE_ >&)Pointer).Owner = false; }
   XMLschemaPointer< TYPE_ >& operator=( const XMLschemaPointer< TYPE_ >& Pointer )
   {
      if( &Pointer != this )
      {
         pObject = Pointer.pObject;
         Owner = Pointer.Owner;
         ((XMLschemaPointer< TYPE_ >&)Pointer).Owner = false;
      }
      return *this;
   }
   ~XMLschemaPointer(){ if( Owner ) delete pObject;  }

   //access operators
   TYPE_& operator *(){ return *pObject; }
   const TYPE_& operator *() const { return *pObject; }
   operator TYPE_&(){ return *pObject; }
   operator const TYPE_&() const { return *pObject; }  
   operator TYPE_*(){ return pObject; }
   operator const TYPE_*() const { return pObject; }  
   
   TYPE_* operator ->(){ return pObject; }
   const TYPE_* operator ->() const { return pObject; }

   bool isOwner(){ return Owner; }
private:
   bool Owner;
   TYPE_* pObject;
};

#endif // end of defensive include
