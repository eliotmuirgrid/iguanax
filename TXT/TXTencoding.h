#ifndef __TXT_ENCODING_H__
#define __TXT_ENCODING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTencoding
//
// Description:
//
// TXTencoding represents a character-set or encoding.  It keeps track of
// the official name (preferred MIME name) that is typically sent to web
// browsers, for instance, and a user-friendly description.  It also can
// be used to convert from one encoding to another.
//
// Author: Eric Mulvaney
// Date:   Thursday, May 28th, 2009 @ 02:25:41 PM
//
//---------------------------------------------------------------------------

#include <COL/COLstring.h>
template <class T> class COLarray;
class COLvar;

// POD - allow default copy constructor and assignment operator.
class TXTencoding {
public:
   // Keep descriptions short and user-friendly.  E.g., the built-in
   // ISO-8859-1 encoding is described as "Western (ISO-8859-1)".
   TXTencoding(const COLstring& iName, const COLstring& iDescription);
   explicit TXTencoding(const COLstring& iName);
   // The default is Windows-1252 on Windows and ISO-8859-1 elsewhere.
   // Windows-1252 is a popular Latin-1 superset of ISO-8859-1, where
   // some control characters are replaced with printable ones.
   TXTencoding();

   // Appends the built-in encodings to the supplied vector.
   static void getBuiltInEncodings(COLarray<TXTencoding>&);

   static TXTencoding utf8();

   const COLstring& name()        const { return Name; }
   const COLstring& description() const { return Description; }

   bool isBuiltIn() const { return IsBuiltIn; }
   bool isDefault() const { return *this == TXTencoding(); }

   bool operator == (const TXTencoding& That) const{
      return (Name == That.Name) && (Description == That.Description);
   }

   bool operator != (const TXTencoding& That) const{
      return !(*this == That);
   }

   // Returns true if strings in this encoding can be converted to/from
   // UTF-8 or some other supported encoding.
   bool isSupported() const;

   // Converts the given C-string to/from UTF-8.  If the this encoding
   // is unsupported, the string is copied unchanged.
   void convertToUtf8  (COLstring&, const char*, size_t) const;
   void convertFromUtf8(COLstring&, const char*, size_t) const;

   // Converts the given C-string to/from the given encoding.  If either
   // encoding is unsupported, the string is copied unchanged.
   void convertTo  (COLstring&, const char*, size_t, const TXTencoding& DestinationEncoding) const;
   void convertFrom(COLstring&, const char*, size_t, const TXTencoding& SourceEncoding) const;
private:
   COLstring  Name;
   COLstring  Description;
   bool IsBuiltIn;
};

// In case a user wants to just use a filter, provide a mechanism of retrieving the available encodings
// NOTE : Just because an encoding is 'available' does not mean it is 'supported'!
//    The key distinction is the ability to convert to/from UTF-8 with that encoding
const COLvar& TXTgetAvailableEncodings();
const COLvar& TXTgetAliasEncodings(const COLstring& Encoding);

#endif // end of defensive include
