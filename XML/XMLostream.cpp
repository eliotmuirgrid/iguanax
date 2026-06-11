#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLostream.h>
#include <LEG/LEGvector.h>

//xml values
const char* XMLostream::pXMLxml = "xml";
const char* XMLostream::pXMLxmlns = "xmlns";
const char* XMLostream::pXMLversion = "version";
const char* XMLostream::pXMLversionNum = "1.0";
const char* XMLostream::pXMLencoding = "encoding";

//xml delimiters
const char XMLostream::XMLstartTag = '<';
const char XMLostream::XMLendTag = '>';
const char XMLostream::XMLprocessingInstruction = '?';
const char XMLostream::XMLspace = ' ';
const char XMLostream::XMLequal = '=';
const char XMLostream::XMLquote = '\"';
const char XMLostream::XMLnamespace = ':';
const char XMLostream::XMLtab = '\t';
const char XMLostream::XMLend = '/';
const char XMLostream::XMLexclamation = '!';

struct XMLostreamPrivate
{
   XMLostreamPrivate( COLostream& Stream_ ):Stream(Stream_),Level(0){}
   COLostream& Stream;
   COLuint32 Level;
   COLstring Tabs;
   LEGvector< COLstring > TagStack;
};

XMLostream::XMLostream( COLostream& Stream_ )
{
   pMember = new XMLostreamPrivate( Stream_ );
}

XMLostream::~XMLostream()
{
   delete pMember;
}
void XMLostream::outputEntityTag( const char* pName )
{ 
   pMember->Stream << XMLstartTag << XMLexclamation << pName; 
}

void XMLostream::outputProcessingInstructionTag( const char* pName )
{ 
   pMember->Stream << XMLstartTag << XMLprocessingInstruction << pName; 
}

void XMLostream::outputProcessingInstructionTagEnd()
{ 
   pMember->Stream  << XMLprocessingInstruction << XMLendTag << newline; 
}

void XMLostream::outputTag( const char* pName )
{ 
   pMember->Stream << pMember->Tabs << XMLstartTag << pName; 
   pMember->TagStack.push_back( pName ); 
}

void XMLostream::outputTag( const char* pNamespace, const char* pName )
{ 
   pMember->Stream << pMember->Tabs << XMLstartTag << pNamespace << XMLnamespace << pName; 
   pMember->TagStack.push_back( COLstring( pNamespace ) + XMLnamespace + pName ); 
}

void XMLostream::outputTagEnd()
{ 
   pMember->Stream << XMLendTag << newline;
   ++pMember->Level;
   pMember->Tabs.assign( pMember->Level, XMLtab );
}

void XMLostream::outputTagEndClose()
{ 
   pMember->Stream << XMLend << XMLendTag << newline; 
   pMember->TagStack.pop_back(); 
}

void XMLostream::outputCloseTag()
{ 
   --pMember->Level;
   pMember->Tabs.assign( pMember->Level, XMLtab );
   pMember->Stream << pMember->Tabs << XMLstartTag << XMLend << pMember->TagStack.back().c_str() << XMLendTag << newline; 
   pMember->TagStack.pop_back(); 
}

void XMLostream::outputAttribute( const char* pName, const char* pValue )
{ 
   pMember->Stream << XMLspace << pName <<  XMLequal << XMLquote << pValue << XMLquote;
}

void XMLostream::outputAttribute( const char* pNamespace, const char* pName, const char* pValue )
{ 
   pMember->Stream << XMLspace << pNamespace << XMLnamespace << pName <<  XMLequal << XMLquote << pValue << XMLquote;
}

void XMLostream::outputAttribute( const char* pName, COLuint32 Value )
{
   pMember->Stream << XMLspace << pName <<  XMLequal << XMLquote << Value << XMLquote;
}

void XMLostream::outputAttribute( const char* pNamespace, const char* pName, COLuint32 Value )
{
   pMember->Stream << XMLspace << pNamespace << XMLnamespace << pName <<  XMLequal << XMLquote << Value << XMLquote;
}
