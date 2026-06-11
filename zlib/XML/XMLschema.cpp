#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschema.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschemaElement.h>
#include <LEG/LEGvector.h>

struct XMLschemaPrivate
{
   LEGvector< XMLschemaPointer< XMLschemaType > > Types;
   LEGvector< XMLschemaPointer< XMLschemaElement > > Elements;
};

XMLschema::XMLschema()
{
   pMember = new XMLschemaPrivate();
}

XMLschema::~XMLschema()
{
   delete pMember;
}

//type collection
XMLschemaType& XMLschema::typeAt( COLuint32 Index )
{
   COLPRECONDITION( Index < pMember->Types.size() );
   return *pMember->Types[ Index ];
}

const XMLschemaType& XMLschema::typeAt( COLuint32 Index ) const
{
   COLPRECONDITION( Index < pMember->Types.size() );
   return *pMember->Types[ Index ];
}

void XMLschema::attachType( XMLschemaType* pType )
{
   *pMember->Types.push_back( pType );
}

COLuint32 XMLschema::countOfTypes() const
{
   return pMember->Types.size();
}

XMLschemaType* XMLschema::findType( const COLstring& Name )
{
   COLuint32 i;
   for( i = 0; i < pMember->Types.size(); i++ )
   {
      if( pMember->Types[i]->name() == Name )
      {
         return pMember->Types[i];
      }
   }
   return 0;
}

//element collection
XMLschemaElement& XMLschema::elementAt( COLuint32 Index )
{
   COLPRECONDITION( Index < pMember->Elements.size() );
   return *pMember->Elements[ Index ];
}

const XMLschemaElement& XMLschema::elementAt( COLuint32 Index ) const
{
   COLPRECONDITION( Index < pMember->Elements.size() );
   return *pMember->Elements[ Index ];
}

void XMLschema::attachElement( XMLschemaElement* pElement )
{
   pMember->Elements.push_back( pElement );
}

COLuint32 XMLschema::countOfElements() const
{
   return pMember->Elements.size();
}

XMLschemaElement* XMLschema::findElement( const COLstring& Name )
{
   COLuint32 i;
   for( i = 0; i < pMember->Elements.size(); i++ )
   {
      if( pMember->Elements[i]->name() == Name )
      {
         return pMember->Elements[i];
      }
   }
   return 0;
}
