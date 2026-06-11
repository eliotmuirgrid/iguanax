//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTencoding
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Thursday, May 28th, 2009 @ 02:25:41 PM
//---------------------------------------------------------------------------
#include "TXTprecomp.h"
#pragma hdrstop

#include "TXTencoding.h"
#include "errno.h"            // julian added to try and fix make error "TXTencoding.cpp:228:55: error: use of undeclared identifier 'errno'"
//#include "TXTiconvDll.h"
#include <ICONV/iconv.h>
//#include <localcharset.h>

#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

struct BuiltInEncoding_t
{
   const char* pName;
   const char* pDescription;
};

static const BuiltInEncoding_t BuiltInEncodings[] =
{
#ifdef _WIN32
   { "Windows-1252", "Western (Windows-1252)" },  // Default for Windows.
   { "ISO-8859-1",   "Western (ISO-8859-1)" },
#else
   { "ISO-8859-1",   "Western (ISO-8859-1)" },    // Default elsewhere.
   { "Windows-1252", "Western (Windows-1252)" },  // Might be supported.
#endif

{ "ISO-8859-2", "Central European (ISO-8859-2)" },
{ "ISO-8859-3", "South European (ISO-8859-3)" },
{ "ISO-8859-4", "Central European (ISO-8859-4)" },
{ "ISO-8859-5", "Cyrillic (ISO-8859-5)" },
{ "ISO-8859-6", "Arabic (ISO-8859-6)" },
{ "ISO-8859-7", "Greek (ISO-8859-7)" },
{ "ISO-8859-8", "Hebrew (ISO-8859-8)" },
{ "ISO-8859-9", "Turkish (ISO-8859-9)" },
// #endif

   // The lowercase among these are for HPUX compatibility.   // JULIAN-CHM_XXX - JULIAN-CHECK - search for using lowercase (and uppercase)

   // { "big5",       "Chinese (Big 5)" },                    // JULIAN-CHM_XXX -- might be function params in the library = KEEP 
   { "BIG5-HKSCS", "Chinese (Big 5 HKSCS)" },
   { "GB2312",     "Chinese (GB 2312)" },
   { "GBK",        "Chinese (GBK)" },
   { "GB18030",    "Chinese (GB 18030)" },

   // { "eucJP",       "Japanese (EUC-JP)" },                 // JULIAN-CHM_XXX  = review agian later
   // { "sjis",        "Japanese (Shift JIS)" },              // JULIAN-CHM_XXX 
   { "ISO-2022-JP", "Japanese (ISO-2022-JP)" },

   { "ISO-2022-KR", "Korean (ISO-2022-KR)" },

   #define TXT_UTF8_NAME "UTF-8" 

#define TXT_UTF8_DESCRIPTION  "Unicode (UTF-8)"
   { TXT_UTF8_NAME, TXT_UTF8_DESCRIPTION },  // UTF-8 must be last.
   { 0 }
};

static const BuiltInEncoding_t *const pUtf8
   = &BuiltInEncodings[sizeof BuiltInEncodings / sizeof(BuiltInEncoding_t) - 2];

TXTencoding::TXTencoding(const COLstring& iName, const COLstring& iDescription)
   : Name(iName)
   , Description(iDescription)
   , IsBuiltIn(false)
{
   if( Name.is_null() )
   {
      throw COLerror("Cannot create a TXTencoding with an empty name.");
   }
   else if( Description.is_null() )
   {
      Description = "Custom (" + Name + ')';
   }
}

TXTencoding::TXTencoding(const COLstring& iName)
   : Name(iName)
   , Description("Custom (" + iName + ')')
   , IsBuiltIn(false)
{
   if( Name.is_null() )
   {
      throw COLerror("Cannot create a TXTencoding with an empty name.");
   }
}


static const BuiltInEncoding_t* pDefaultEncoding;

TXTencoding::TXTencoding()
   : IsBuiltIn  (true)
{
   if( pDefaultEncoding )
   {
      Name        = pDefaultEncoding->pName;
      Description = pDefaultEncoding->pDescription;
   }
   else
   {
      const BuiltInEncoding_t* pEncoding = &BuiltInEncodings[0]; // Latin-1
      Name        = pEncoding->pName;
      Description = pEncoding->pDescription;

      if( !isSupported() )
      {
         pEncoding   = pUtf8;
         Name        = pEncoding->pName;
         Description = pEncoding->pDescription;
      }

      pDefaultEncoding = pEncoding;
   }
}


void TXTencoding::getBuiltInEncodings(COLarray<TXTencoding>& Output)
{
   COL_FUNCTION(getBuiltInEncodings);

   for(const BuiltInEncoding_t* pEncoding = BuiltInEncodings; pEncoding->pName; ++pEncoding)
   {
      TXTencoding Encoding(pEncoding->pName, pEncoding->pDescription);
      Encoding.IsBuiltIn = true;

      if( Encoding.isSupported() )
      {
         Output.push_back(Encoding);
      }
      else
      {
         COL_WRN("Not supported: " << Encoding.description());
      }
   }
}


TXTencoding TXTencoding::utf8()
{
   return TXTencoding(TXT_UTF8_NAME, TXT_UTF8_DESCRIPTION);
}

bool TXTencoding::isSupported() const
{
   COL_METHOD(TXTencoding::isSupported);
   iconv_t Handle;

   // Solaris (and probably others) doesn't like X -> X conversions, so
   // UTF-8 is always considered supported.  If it isn't really, all other
   // encodings won't be supported either (since we use UTF-8 as a
   // reference).  In that case, we still want to present at least one
   // encoding option to the user; UTF-8 is used because that's what
   // we send/receive from web browsers.
   //
   if( Name == TXT_UTF8_NAME )
   {
      return true;
   }

   COL_VAR2(Name, TXT_UTF8_NAME);
   Handle = ICONViconv_open(Name.c_str(), TXT_UTF8_NAME);
   if( Handle == (iconv_t)(-1) )
   {
      return false;
   }
   else
   {
      ICONViconv_close(Handle);

      COL_VAR2(TXT_UTF8_NAME, Name);
      Handle = ICONViconv_open(TXT_UTF8_NAME, Name.c_str());
      if( Handle == (iconv_t)(-1) )
      {
         return false;
      }

      ICONViconv_close(Handle);
   }

   return true;
}


static void convert(COLstring& Target, const char* pSource, size_t SourceSize, const char* pSourceEncoding, const char* pTargetEncoding)
{
   COL_FUNCTION(convert);
   COL_VAR2(pSourceEncoding, pTargetEncoding);

   Target.clear();

   iconv_t Handle = ICONViconv_open(pTargetEncoding, pSourceEncoding);

   if( Handle == (iconv_t)(-1) )
   {
      COL_WRN("No conversion available: " << strerror(errno));
      Target.write(pSource, SourceSize);
   }
   else try
   {
      int Utf8 = -1; // Maybe (<0), no (0), or yes (>0).

      Target.setCapacity(SourceSize + 32);

      size_t BytesAvailable = Target.capacity() - 1;
      size_t BytesToGo      = SourceSize;

      char* pWritePoint = (char*)Target.c_str();
      const char* pReadPoint = pSource;

      while(BytesToGo)
      {
         size_t Result = ICONViconv(Handle, &pReadPoint, &BytesToGo, &pWritePoint, &BytesAvailable);

         if( Result == (size_t)(-1) && BytesToGo && BytesAvailable > 3 )
         {
            COL_TRC("Found invalid multi-byte.");

            // Append the Unicode replacement character: U+FFFD, if we're outputting
            // UTF-8; otherwise, append a question-mark (not ASCII SUB \x1A).
            //
            if( (Utf8 < 0 && (Utf8 = !strcmp(pTargetEncoding, TXT_UTF8_NAME))) || Utf8 )
            {
               *pWritePoint++ = '\xEF';
               *pWritePoint++ = '\xBF';
               *pWritePoint++ = '\xBD';
               BytesAvailable -= 3;
            }
            else
            {
               *pWritePoint++ = '?';
               --BytesAvailable;
            }

            // Skip the offending character.
            ++pReadPoint;
            --BytesToGo;

            if( BytesToGo )
            {
               continue;
            }
         }

         Target.setSize(pWritePoint - (char*)Target.c_str());

         if( BytesToGo )
         {
            COL_TRC("Resizing output buffer.");
            Target.setCapacity(Target.capacity() + (BytesToGo < 16 ? 32 : BytesToGo*2));

            pWritePoint = (char*)Target.c_str() + Target.size();
            BytesAvailable = (size_t)Target.capacity() - Target.size() - 1;
         }
      }

      ICONViconv_close(Handle);
   }
   catch(...)
   {
      ICONViconv_close(Handle);
      throw;
   }
}


void TXTencoding::convertToUtf8(COLstring& Target, const char* pSource, size_t SourceSize) const
{
   convert(Target, pSource, SourceSize, Name.c_str(), TXT_UTF8_NAME);
}

void TXTencoding::convertFromUtf8(COLstring& Target, const char* pSource, size_t SourceSize) const
{
   convert(Target, pSource, SourceSize, TXT_UTF8_NAME, Name.c_str());
}

void TXTencoding::convertTo(COLstring& Target, const char* pSource, size_t SourceSize, const TXTencoding& DestinationEncoding) const
{
   convert(Target, pSource, SourceSize, Name.c_str(), DestinationEncoding.Name.c_str());
}

void TXTencoding::convertFrom(COLstring& Target, const char* pSource, size_t SourceSize, const TXTencoding& SourceEncoding) const
{
   convert(Target, pSource, SourceSize, SourceEncoding.Name.c_str(), Name.c_str());
}

// The list of supported encodings is not expected to change during run-time, so only calculate once and then cache it
static COLvar TXTallEncodings;
// Also keep a table on the side that maps an encoding name to its index in the table above, so aliases can be looked up easily
static COLvar TXTaliasLookupTable;

// This is a callback used by iconv when it's providing a list of aliases
// The 'Data' parameter is passed back when we do the initial call to iconvlist
//    It is being unused (just set to NULL)
static int TXTencodingIconvCallback(unsigned int NumAliases, const char* const* Names, void* vpData) {
   COL_FUNCTION(TXTencodingIconvCallback);

   if (!NumAliases) {
      // In practice, this should never happen, but libiconv has checks against it, so we should too
      COL_WRN("iconv provided an alias list of size 0 - this might be a bug in the library - ignoring it and moving on");
      return 0;   // Just skip this input - it doesn't have any data anyway
   }

   int CurrentAliasIndex = TXTallEncodings.size();
   COLvar& CurrentAliases = TXTallEncodings.push_back();
   // Create an entry for each alias
   for (int i = 0; i < NumAliases; i++) {
      COLstring Name = COLstring(Names[i]);
      CurrentAliases.push_back(Name);
      TXTaliasLookupTable[Name] = CurrentAliasIndex;
   }

   return 0;
}

static void TXTinitEncodingList() {
   COL_FUNCTION(TXTinitEncodingList);
   TXTallEncodings = COLvar();
   // This method calls the callback above, with the 'Data' argument provided as the second parameter
   //    It is current being unused, so it's just set to NULL and not being referenced
   iconvlist(TXTencodingIconvCallback, NULL);

   COLPOSTCONDITION(TXTallEncodings.size());
}

const COLvar& TXTgetAvailableEncodings() {
   COL_FUNCTION(TXTgetAvailableEncodings);

   if (!TXTallEncodings.size()) {
      // No cached results, so calculate all the encodings
      TXTinitEncodingList();
   }

   // Chances are, nobody will want to modify the list of available encodings, so it's better to pass a reference than make a copy
   return TXTallEncodings;
}

const COLvar& TXTgetAliasEncodings(const COLstring& Encoding) {
   COL_FUNCTION(TXTgetAliasEncodings);
   if (!TXTallEncodings.size()) {
      // No cached results, so calculate all the encodings
      TXTinitEncodingList();
   }

   // Common capitalization, dashes, and underscores can be taken care of by iconv (though not all punctuations can be safely ignored)
   //    This step is pretty complicated, so just let iconv work its magic
   // If the encoding is completely unrecognized, the name is unchanged, so an appropriate error is generated
   const char* CanonicalName = ICONViconv_canonicalize(Encoding.c_str());
   COL_VAR2(Encoding, CanonicalName);
   if (!TXTaliasLookupTable.exists(CanonicalName)) {
      COL_ERROR_STREAM("Encoding '" << CanonicalName << "' is not an available encoding.", COLerror::PreCondition);
   }

   int AliasIndex = TXTaliasLookupTable[CanonicalName].asInt();
   return TXTallEncodings[AliasIndex];
}
