//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CSVreader
//
// Description:
//
// Implementation
//
// Author: Ramanan Sivasothinathan
// Date:   Thursday, October 8th, 2009 @ 01:28:39 PM
//----------------------------------------------------------------------
#include "CSVprecomp.h"
#pragma hdrstop

#include <CSV/CSVreader.h>
#include <COL/COLstring.h>
#include <LEG/LEGvector.h>
/*
#include <COL/COLreadable.h>
#define ZR 0x00
#define LF 0x0A
#define CR 0x0D
#define QT 0x22
#define CM 0x2C

CSVreader::CSVreader() { pMember = NULL; }
CSVreader::~CSVreader() { }

char CSVreader::next()
{
   char c;
   if (pMember->isEndOfReadable() || pMember->read(&c,1) != 1) { return 0; }
   return c;
}

void CSVreader::parse(COLreadable* pReadable)
{
   pMember = pReadable;
   for(char c=next() ; c!=ZR ; )                           // For each line/row
   {
      LEGvector<COLstring> Row;
      while(c!=ZR)                                         // For each column/field/value
      {
         COLstring Value;
         if (c==QT)                                        // Value is enclosed in quotes
         {
            bool quote = false;
            for (c=next() ; c!=ZR ; c=next())              // For each char in column/field/value
            {
               if(c==QT)
               {
                  if (quote) { quote = false;           }  // escaped quote
                  else       { quote = true ; continue; }
               }
               else if(quote) { break; }                   // field terminating quote
               Value += c;
            }
         }
         else                                              // Raw value (without quotes)
         {
            for ( ; c!=ZR && c!=LF && c!=CR && c!=CM ; c=next()) { Value += c; }
         }
         Row.push_back(Value);                             // Note: Value is already null terminated
         if      (c==ZR) {                                    break; }
         else if (c==LF) { c=next();                          break; }
         else if (c==CR) { c=next(); if (c==LF) { c=next(); } break; }
         else if (c==CM) { c=next();                                 }
      }
      load(Row.size(), Row.get());
   }
   pMember = NULL;
}

*/
