#include <XML/XMLschemaNode.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschema.h>
#include <COL/COLstring.h>


struct XMLschemaNodePrivate
{
   XMLschemaNodePrivate( ):
      MinOccurs(1),
      MaxOccurs(1){}
   ~XMLschemaNodePrivate(){}
      
   COLstring Name;
   XMLschemaPointer< XMLschemaType > pType;
   COLuint32 MinOccurs;
   COLuint32 MaxOccurs;
};
