//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LSFItextFilters
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Monday, June 13th, 2011 @ 05:53:28 PM
//---------------------------------------------------------------------------

#include "LSFItextFilters.h"

#include <COL/COLauto.h>
#include <COL/COLfilter.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;
#include <GZIP/GZIPcompressFilter.h>
#include <GZIP/GZIPdecompressFilter.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAcheck.h>
#include <LUA/LUAtimeout.h>

#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>



#include <MZIP/MZIPvar.h>
#include <SFI/SFIaesDecryptor.h>
#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SFI/SFIbase64EncodeFilter.h>
#include <SFI/SFIhexDecodeFilter.h>
#include <SFI/SFIhexEncodeFilter.h>
#include <SFI/SFIhtmlEscape.h>
#include <SFI/SFIunixToUnix.h>
#include <SFI/SFIuriEscapeFilter.h>
#include <SFI/SFIuriUnescapeFilter.h>
#include <STMZIP/STMZIPfilterCompress.h>
#include <STMZIP/STMZIPfilterDecompress.h>
#include <time.h>
// #include <new>
//
// Shared Utilities
//
#undef ERROR
#define ERROR(X) \
   do{ COL_ERROR_STREAM_PLAIN(X, COLerror::PreCondition); }while(0)

static void LSFIwriteTo(COLsink& Sink, const char* pData, size_t Length){
   COL_FUNCTION(LSFIwriteTo);
   COL_VAR2(pData, Length);
   if(Sink.write(pData, Length) != Length) {
      ERROR("filter failed: invalid input data");  // Probably.
   }
   Sink.flush();
   Sink.onEndStream();
}

static inline void pushBuffer(lua_State* L, COLstring& Buf){
   lua_pushlstring(L, (const char*)Buf.c_str(), Buf.size());
}

// These objects aren't visible to users, so the name can be funky.
#undef TNDL_TEXT_FILTER_META_TABLE
#define TNDL_TEXT_FILTER_META_TABLE "LSFItextFilter"

LSFItextFilter::LSFItextFilter(COLauto<COLfilter>& pFilter) : m_pFilter(pFilter), m_Finished(false) {
   COL_METHOD(LSFItextFilter::LSFItextFilter);
   COLPRECONDITION(m_pFilter.get() && m_pFilter.isOwner());
}

// This requires LSFItextFilter::reader()'s closure and args.
//
int LSFItextFilter::filterChunk(lua_State* L, int nArgs){
   if(m_Finished) {  // Already done, just return nil.
      lua_pushnil(L);
      return 1;
   }
   COLstring Out;
   COLsinkString Sink(Out);
   m_pFilter->setNext(&Sink);
   try {
      // Get data from our reader, passing the arguments we received along.
      lua_pushvalue(L, lua_upvalueindex(1));
      for(int i=1; i<=nArgs; i++) {
         lua_pushvalue(L,i);
      }
      lua_call(L,nArgs,1);
      if(lua_isnil(L,-1)) {  // No more data; EOS might yield output.
         m_Finished = true;
         m_pFilter->onEndStream();
      } else if(lua_type(L,-1) == LUA_TSTRING) {  // Got data.
         size_t len;
         const char* s = lua_tolstring(L, -1, &len);
         if(m_pFilter->write(s, len) != len) {
            m_Finished = true;  // Treat as fatal; no retries.
            ERROR("filter failed: invalid input data");  // Probably.
         }
         m_pFilter->flush();
      } else {
         // Don't set m_Finished, they can try again.
         ERROR("reader returned non-string/non-nil value");
      }
      if(m_Finished && Out.size() == 0) {
         lua_pushnil(L);
      } else {
         pushBuffer(L, Out);
      }
   } catch(...) {
      m_pFilter->setNext(NULL);
      throw;
   }
   m_pFilter->setNext(NULL);
   LUAcheckTimeout(L);
   return 1;
}

int LSFItextFilter::reader(lua_State* L)
{
   try {
      int nArgs = lua_gettop(L);  // We just pass them on.
      lua_pushvalue(L, lua_upvalueindex(2));
      LSFItextFilter* pSelf = (LSFItextFilter*)LUAcheckUdata(L, -1, TNDL_TEXT_FILTER_META_TABLE,
         "Invalid 'LSFItextFilter'");

      return pSelf->filterChunk(L, nArgs);
   } LUA_CATCH();
   return 0;
}

int LSFIapplyTextFilter(lua_State* L, int i, COLauto<COLfilter> pFilter) {
   COL_FUNCTION(LSFIapplyTextFilter);
   COL_VAR(i);
   try {
      const char* Data;
      size_t DataLength;
      if (LUAextractString(L, i, &Data, &DataLength)) {
         // String case - run it through the filter and push the result onto the stack
         COLsinkString Out;
         pFilter->setNext(&Out);
         LSFIwriteTo(*pFilter, Data, DataLength);
         COL_VAR(Out.string());
         LUApushString(L, Out.string());
      } else if (lua_isfunction(L, i)) {
         // Stream case - provide a closure that acts as a write into the filter
         lua_pushvalue(L,i);
         LUApushObject<LSFItextFilter>(L, pFilter, TNDL_TEXT_FILTER_META_TABLE);
         lua_pushcclosure(L, &LSFItextFilter::reader, 2);
      } else {
         luaL_argerror(L, i, "string or stream expected");
      }
      return 1;
   }  LUA_CATCH();
   return 0;
}

int LSFIapplyTextFilter(lua_State* L, COLauto<COLfilter> pFilter) {
   COL_FUNCTION(LSFIapplyTextFilter);
   // Implied i == 1
   LUAcheckArgCount(L, 1, -1);
   return LSFIapplyTextFilter(L, 1, pFilter);
}

//
// New Filters for Simple Block Processors
//

class LSFItextFilterHtmlTextEncoder : public COLfilter{
public:
   LSFItextFilterHtmlTextEncoder(COLsink* pOut): COLfilter(pOut) {}
   void resetFilter() {}
   COLuint32 write(const void *pIn, COLuint32 Size) {
      SFIhtmlEscapeNoNewline((const char*)pIn, Size, next());
      return Size;
   }
};

//
// Simple Filters
//
static int encodeBase64(lua_State* L) { return LSFIapplyTextFilter(L, new SFIbase64EncodeFilter(NULL, 0)); } // 0 = one line
static int decodeBase64(lua_State* L) { return LSFIapplyTextFilter(L, new SFIbase64DecodeFilter(NULL)); }

static int encodeBase16(lua_State* L) { return LSFIapplyTextFilter(L, new SFIhexEncodeFilter(NULL)); }
static int decodeBase16(lua_State* L) { return LSFIapplyTextFilter(L, new SFIhexDecodeFilter(NULL)); }

static int encodeHtmlText(lua_State* L) { return LSFIapplyTextFilter(L, new LSFItextFilterHtmlTextEncoder(NULL)); }

static int encodeUriComponent(lua_State* L) { return LSFIapplyTextFilter(L, new SFIuriEscapeFilter(NULL)); }
static int decodeUriComponent(lua_State* L) { return LSFIapplyTextFilter(L, new SFIuriUnescapeFilter(NULL)); }
//
// Compression
//
static int deflateGZip(lua_State* L) { return LSFIapplyTextFilter(L, new GZIPcompressFilter(NULL)); }
static int inflateGZip(lua_State* L) { return LSFIapplyTextFilter(L, new GZIPdecompressFilter(NULL)); }

static int LSFIzipDecompress(lua_State* L){
   COL_FUNCTION(LSFIzipDecompress);

   // Function argument may either be a Map or a String containing the zip data.
   // If a Map, it must contain a "data" element and optionally a
   // "meta" element if user wants ::date and ::mode file attributes.

   if (lua_gettop(L) != 1){
      COL_ERR("no argument supplied to LSFIzipDecompress");
      luaL_error(L, "Function requires a zip data string argument.");
      return 0;
   }

   COLvar Var;
   Var.fromLua(L,-1);
   COL_VAR(Var);
   COLstring Data;
   bool WantMetaData = false;
   if (Var.isString()) {
      Var.swap(&Data);
   } else if (Var.isMap()) {
      WantMetaData = Var.getBool("meta");
      if (COLvar* pDataVar = Var.get("data")) {
         pDataVar->swap(&Data);
      } else {
         COL_ERR("no COLvar Map 'data' element provided to LSFIzipDecompress");
         luaL_error(L, "Need to provide a table with element 'data' containing binary zip data.");
         return 0;
      }
   } else {
      COL_ERR("wrong argument type supplied to LSFIzipDecompress: " << Var);
      luaL_error(L, "Function argument type is incorrect: expecting zip data in a string.");
      return 0;
   }

   // Uncompress the zip file data into a COLvar Map and
   // push it onto the top of the Lua stack as a table.

   COLstring Error;
   if (0 != MZIPunzipToVar(Data.c_str(), Data.size(), NULL, WantMetaData, &Var, &Error)){
      COL_ERR("Error raised from MZIPunzipToVar: " << Error);
      luaL_error(L, Error.c_str());
      return 0;
   }
   Var.toLua(L);
   return 1;
}

//
// Return a binary zip string from a table representing a directory file structure.
//
static int LSFIzipCompress(lua_State* L){
   COL_FUNCTION(LSFIzipCompress);
   if (lua_gettop(L) != 1 || !lua_istable(L,-1)) {
      COL_ERR("no table argument supplied");
      luaL_error(L, "Need to provide zip data in table form.");
      return 0;
   }
   COLvar Var;
   Var.fromLua(L, -1);
   COL_VAR(Var);
   COLstring BinaryZipContents;
   COLstring Error;
   if (0 != MZIPzipVar(Var, &BinaryZipContents, &Error)){
      COL_ERR("Error raised from MZIPzipVar: " << Error);
      luaL_error(L, Error.c_str());
      return 0;
   }
   COL_VAR(BinaryZipContents.size());
   //COL_DUMP("BinaryZipContents", BinaryZipContents.c_str(), BinaryZipContents.size());
   LUApushString(L, BinaryZipContents);
   return 1;
}

static int deflateBZip2(lua_State* L) { return LSFIapplyTextFilter(L, new STMZIPfilterCompress(NULL)); }
static int inflateBZip2(lua_State* L) { return LSFIapplyTextFilter(L, new STMZIPfilterDecompress(NULL)); }

//
// AES Encryption
//

#define AES_BLOCK_SIZE 16

class LSFItextFilterAes : public COLfilter
{
   char m_aCache[AES_BLOCK_SIZE];
   int m_CacheSize;
   virtual void process(const char*, char*) = 0;
   void resetFilter() { COLMSGPRECONDITION(false, "cannot reset filter"); }
public:
   LSFItextFilterAes(): COLfilter((COLsink*)NULL), m_CacheSize(0) {}
   COLuint32 write(const void*, COLuint32);
   void onEndStream();
};

COLuint32 LSFItextFilterAes::write(const void* pBuf, COLuint32 Size) {
   const COLuint32 OriginalSize = Size;
   const char* pIn = (const char*)pBuf;
   char aOut[AES_BLOCK_SIZE];
   if(m_CacheSize) {
      int More = COL_MIN((int)Size, AES_BLOCK_SIZE - m_CacheSize);
      memcpy(m_aCache + m_CacheSize, pIn, More);
      pIn += More; Size -= More;
      m_CacheSize += More;
      if(m_CacheSize >= AES_BLOCK_SIZE) {
         m_CacheSize = 0;
         process(m_aCache, aOut);
         next().write(aOut, AES_BLOCK_SIZE);
      }
   }
   int Extra = Size % AES_BLOCK_SIZE;
   const char* pEnd = pIn + (Size - Extra);
   for(; pIn < pEnd; pIn += AES_BLOCK_SIZE) {
      process(pIn, aOut);
      next().write(aOut, AES_BLOCK_SIZE);
   }
   if(Extra) {
      memcpy(m_aCache, pIn, Extra);
      m_CacheSize = Extra;
   }
   return OriginalSize;
}

void LSFItextFilterAes::onEndStream() {
   if(m_CacheSize) {
      // Automatically pad the end with NUL characters.
      char aBuf[AES_BLOCK_SIZE] = {0};
      write(aBuf, AES_BLOCK_SIZE - m_CacheSize);
   }
   next().onEndStream();
}

class LSFItextFilterAesEncryptor : public LSFItextFilterAes {
   SFIaesEncryptor m_Encryptor;
   void process(const char* pIn, char* pOut) {
      m_Encryptor.ProcessBlock((const COLuint8*)pIn, (COLuint8*)pOut);
   }
public:
   explicit LSFItextFilterAesEncryptor(const COLstring& Key)
      : m_Encryptor((const COLuint8*)Key.c_str(), Key.size())
   { }
};

class LSFItextFilterAesDecryptor : public LSFItextFilterAes {
   SFIaesDecryptor m_Decryptor;
   void process(const char* pIn, char* pOut) {
      m_Decryptor.ProcessBlock((const COLuint8*)pIn, (COLuint8*)pOut);
   }
public:
   explicit LSFItextFilterAesDecryptor(const COLstring& Key)
      : m_Decryptor((const COLuint8*)Key.c_str(), Key.size())
   { }
};

static COLstring getAesKey(lua_State* L, int i, const COLstring& Name){
   COLstring Key = LUAexpectedStringParam(L, i, Name);
   switch(Key.size()) {
      case 16: case 24: case 32: break;
      default:
         LUA_ERROR_STREAM("AES keys must be 16, 24 or 32 bytes long.");
   }
   return Key;
}

static void pushAesData(lua_State* L, int i, const COLstring& Name){
   LUApushString(L, Name);
   lua_gettable(L, i);
   if(lua_isnil(L,-1)) {
      LUA_ERROR_STREAM("Parameter '" << Name << "' is required.");
   }
   if (lua_isfunction(L, -1)) {
      return;
   }
   COLstring Extracted;
   if (!LUAextractString(L, -1, &Extracted)) {
      LUA_ERROR_STREAM
         ('\'' << Name << "' must be a string or function.");
   }
   LUApushString(L, Extracted);
}

static int encryptAes(lua_State* L){
   const COLstring& Key = getAesKey(L, 1, "key");
   pushAesData(L, 1, "data");
   lua_insert(L, 1);
   lua_settop(L, 1);
   return LSFIapplyTextFilter(L, new LSFItextFilterAesEncryptor(Key));
}

static int decryptAes(lua_State* L){
   const COLstring& Key = getAesKey(L, 1, "key");
   pushAesData(L, 1, "data");
   lua_insert(L, 1);
   lua_settop(L, 1);
   return LSFIapplyTextFilter(L, new LSFItextFilterAesDecryptor(Key));
}

//
// Unix to Unix Encoding
//
#define UUDECINFO_META "uudecode_info"
static int getUUDecodeInfo(lua_State* L){
   SFIunixToUnixDecodeInfo* pSelf = (SFIunixToUnixDecodeInfo*)LUAcheckUdata(L, 1, UUDECINFO_META, "Invalid 'uudecode info'");
   const char* pKey = lua_tostring(L,2);
   if(pKey && !strcmp(pKey, "filename")) {
      LUApushString(L, pSelf->FileName);
   } else if(pKey && !strcmp(pKey, "mode")) {
      // We need to output the mode as an octal number.  Since Lua has
      // no notation for octals, we reinterpret the octal number as
      // a decimal.  E.g., 0644 in C becomes 644 (decimal) in Lua.
      char Buf[32]; snprintf(Buf, 32, "%o", pSelf->Mode);
      lua_pushinteger(L, strtol(Buf, NULL, 10));
   } else {
      lua_pushnil(L);
   }
   return 1;
}

static void LSFIaddMethod(lua_State* L, const char* pName, lua_CFunction pFunction){
   COL_FUNCTION(LSFIaddMethod);
   lua_pushstring(L, pName);
   lua_pushcfunction(L, pFunction);
   lua_settable(L, -3);
}

static SFIunixToUnixDecodeInfo* pushUUDecodeInfo(lua_State* L){
   SFIunixToUnixDecodeInfo* pInfo
      = LUApushObject<SFIunixToUnixDecodeInfo>(L, UUDECINFO_META);
   lua_getmetatable(L, -1);
   LSFIaddMethod(L, "__index", getUUDecodeInfo);
   lua_pop(L,1);
   return pInfo;
}

static int uuencode(lua_State* L){
   int Mode = 0644, Top = lua_gettop(L);
   if(lua_type(L,2) != LUA_TSTRING) {
      luaL_error(L, "*** 'filename' must be a string.");
   } else if(strchr(lua_tostring(L,2), '\n')) {
      luaL_error(L, "*** 'filename' must not contain newlines.");
   }
   const COLstring& Name = LUAtoString(L,2);
   if(Top > 2) {
      // Lua doesn't have a syntax for octal numbers; we interpret decimal
      // numbers as octal to avoid confusion with typical mode notation.
      // E.g., 644 (decimal) in Lua becomes 0644 in C.
      char *s = (char*)lua_tostring(L,3);
      if(!s || !*s || (Mode = strtol(s,&s,8), *s) != '\0') {
         luaL_error(L, "*** 'mode' must be an octal number.");
      }
   }
   return LSFIapplyTextFilter(L, 1, new SFIunixToUnixEncode(Mode, Name));
}

static int uudecode(lua_State* L){
   SFIunixToUnixDecodeInfo* pInfo = pushUUDecodeInfo(L);
   int n = LSFIapplyTextFilter(L, 1, new SFIunixToUnixDecode(pInfo));
   lua_pushvalue(L,2);  // pInfo
   return n + 1;
}

//
// The "filter" Namespace
//
void LSFItextFiltersAddNamespace(lua_State* L){
   lua_newtable(L);  // "filter" Namespace
   {
      lua_newtable(L);
      LSFIaddMethod(L, "enc", encodeBase64);
      LSFIaddMethod(L, "dec", decodeBase64);
      lua_setfield(L, -2, "base64");
      lua_newtable(L);
      LSFIaddMethod(L, "enc", encodeBase16);
      LSFIaddMethod(L, "dec", decodeBase16);
      lua_setfield(L, -2, "hex");
      lua_newtable(L);
      LSFIaddMethod(L, "enc", encodeHtmlText);
      //LSFIaddMethod(L, "dec", decodeHtmlText);  // Not implmented.
      lua_setfield(L, -2, "html");
      lua_newtable(L);
      LSFIaddMethod(L, "enc", encodeUriComponent);
      LSFIaddMethod(L, "dec", decodeUriComponent);
      lua_setfield(L, -2, "uri");
      lua_newtable(L);
      LSFIaddMethod(L, "deflate", deflateGZip);
      LSFIaddMethod(L, "inflate", inflateGZip);
      lua_setfield(L, -2, "gzip");
      lua_newtable(L);
      LSFIaddMethod(L, "deflate", deflateBZip2);
      LSFIaddMethod(L, "inflate", inflateBZip2);
      lua_setfield(L, -2, "bzip2");
      lua_newtable(L);
      LSFIaddMethod(L, "deflate", LSFIzipCompress);
      LSFIaddMethod(L, "inflate", LSFIzipDecompress);
      lua_setfield(L, -2, "zip");
      lua_newtable(L);
      LSFIaddMethod(L, "enc", encryptAes);
      LSFIaddMethod(L, "dec", decryptAes);
      lua_setfield(L, -2, "aes");
      lua_newtable(L);
      LSFIaddMethod(L, "enc", uuencode);
      LSFIaddMethod(L, "dec", uudecode);
      lua_setfield(L, -2, "uuencoding");
   }
   lua_setglobal(L, "filter");
}
