#ifndef __CSV_READER_H__
#define __CSV_READER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CSVreader
//
// Description:
//
// CSVreader class. Example:
//
//   #include <stdio.h>
//   #include <COL/COLstring.h>
//   #include <FIL/FILfile.h>
//   #include <CSV/CSVreader.h>
//   
//   class Reader : public CSVreader
//   {
//      virtual void load(COLuint32 Size, COLstring* Fields)
//      {
//         for(int i=0 ; i<Size ; i++) printf("%s%c", Fields[i].c_str(), (i<Size-1)?'|':'\n');
//      }
//   };
//   
//   int main()
//   {
//      Reader R;
//      FILfile F("iguana_license_export.csv", FILfile::Read);
//      R.parse(&F);
//      return 0;
//   }
//
// Author: Ramanan Sivasothinathan
// Date:   Thursday, October 8th, 2009 @ 01:28:39 PM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Commented it out since I removed COLreadable.
class COLstring;
/*
class CSVreader
{
public:
   CSVreader();
   virtual ~CSVreader();
   virtual void load(COLuint32 Size, COLstring* Fields) = 0;
   void parse(FILfile* pFile); 

private:
   char next();
   COLreadable* pMember;
   CSVreader(const CSVreader& Orig);            // not allowed
   CSVreader& operator=(const CSVreader& Orig); // not allowed
};*/

#endif // end of defensive include
