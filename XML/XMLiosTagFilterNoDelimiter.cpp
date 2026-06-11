#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLiosTagFilterNoDelimiter.h>
#include <XML/XMLiosTagFilterPrivate.h>

XMLiosTagFilterNoDelimiter::XMLiosTagFilterNoDelimiter()
: XMLiosTagFilter('.')
{
  //remove the null escape for the delimiter
  pMember->EscapeFunctions[pMember->XmlDelimiter] = XMLiosTagFilterPrivate::NoEscape;
}

