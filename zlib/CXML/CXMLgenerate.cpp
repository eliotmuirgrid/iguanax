//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: CXMLgenerate
//
// Author: Eliot Muir
//
// Description:
//
// Code to generate XML from COLvar in format from CXMLparse.
// Implementation of COLvar::xml.
//---------------------------------------------------------------------------

#include <SFI/SFIxmlAttributeDataEncodeFilter.h>
#include <SFI/SFIxmlDataEncodeFilter.h>
#include <SFI/SFIxmlTagEncodeFilter.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void CXMLoutputXmlAttributes(const COLvar& Attributes, int Depth, COLstring* pOutput){
   COL_FUNCTION(CXMLoutputXmlAttributes);
   if(!Attributes.isMap()) { return; }
   const COLvar::COLvarMap Map = Attributes.map();
   COLvar::COLvarMap::const_iterator it = Map.cbegin(), E = Map.cend();
   for (; it != E; ++it) {
      COL_VAR2(it->first,it->second);
      if (Depth){
         pOutput->append("\n",1);
      }else{
         pOutput->append(" ",1);
      }
      pOutput->append(Depth, ' ');
      pOutput->append(it->first);
      pOutput->append("=\"",2);
      pOutput->append(SFIxmlAttributeDataEncode(it->second));
      pOutput->append("\"",1);
   }
}

static void CXMLoutputXmlElement(const COLstring& Name, const COLvar& Element, int Depth, int Indent, COLstring* pOutput){
   COL_FUNCTION(CXMLoutputVarElement);
   if(!Element.isMap()) { return; }
   pOutput->append(Depth, ' ');
   *pOutput += "<" + SFIxmlTagEncode(Name);
   if (Element.exists("@") && Element["@"].isMap()){
      CXMLoutputXmlAttributes(Element["@"], Depth+Indent, pOutput);
   }
   bool HasChildren = false;
   COLvar::COLvarMap::const_iterator it = Element.map().cbegin(), E = Element.map().cend();
   for (; it != E; ++it) {
      if (it->first != "@"){
         COL_VAR2(it->first, it->second);
         if (!HasChildren){
            if (Indent){
               pOutput->append(">\n",2);
            } else {
               pOutput->append(">",1);
            }  
         }
         HasChildren = true;
         if(!it->second.isArray()) { continue; }
         COLvar::COLvarVec::const_iterator it2 = it->second.array().begin(), E2 = it->second.array().end();
         for (; it2 != E2; ++it2){
            CXMLoutputXmlElement(it->first, *it2, Depth+Indent, Indent, pOutput);
         } 
         
      }
   }
   if (!HasChildren){
      // Self contained tag.
      if (Indent){
         pOutput->append("/>\n",3); 
      } else {
         pOutput->append("/>",2);
      }
   } else {      
      pOutput->append(Depth, ' '); 
      pOutput->append("</",2);
      pOutput->append(Name);
      if (Indent){
         pOutput->append(">\n",2);
      }else{
         pOutput->append(">",1);
      }
   }   
}

COLstring COLvar::xml(int indent){
   COL_METHOD(COLvar::xml);
   COLstring Output;
   COLvar& Me = *this;
   CXMLoutputXmlElement(map().cbegin()->first, Me[map().cbegin()->first], 0, indent, &Output);
   COL_VAR(Output);
   return Output;
}


