#ifndef __FIL_FILE_H__
#define __FIL_FILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILfile
//
// Description:
//
// Abstraction over unbuffered files.
//
// Author: Eliot Muir
// Date:   Sun 21 Mar 2021 21:52:19 EDT
//---------------------------------------------------------------------------

#include <COL/COLsink.h>
#include <COL/COLstring.h>

class FILfile : public COLsink{
public:
   enum FILmode{
      Read,   // reading only
      Write,  // reading and writing     
      Append, // open and seek to the end
      Rewrite // open, truncate any existing file
   };
   FILfile(const COLstring& FileName, FILmode Mode);
   FILfile();
   ~FILfile();

   void open(const COLstring& FileName, FILmode Mode);
   void close();
   
   bool isOpen() const;

   int position() const;
   void setPosition(int NewPosition);

   int read(void* pBuffer, int SizeOfBuffer);
   int read(COLstring* pString);

   // COLsink interface method used to write the passed in data to the sink object
   virtual COLuint32 write(const void* pBuffer, COLuint32 SizeOfBuffer);

   int write(const COLstring& Content);
   // Note that there is also a FILfileSize function in FILutils.h.  That
   // works fine so long as the operating system does not have the file open.
   // However if you have the file open then only this method is guaranteed
   // to give the correct size of the file.
   int size() const;

   virtual void flush();
private:
   int m_FileNo;
   COLstring m_FileName;

   FILfile(const FILfile& Orig); // not allowed
   FILfile& operator=(const FILfile& Orig); // not allowed
};

#endif // end of defensive include
