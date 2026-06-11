//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLvar
//
// Author:  Andrew Vajoczki
//
// Description:
//
// A variant object that can hold a number, string, vector or map.
//-----------------------------------------------------------------------------

#include "COLvar.h"
#include "COLvarJson.h"
#include "COLlog.h"
COL_LOG_MODULE;

#include <stdio.h>

#include <string.h>

static void adjustRangeForByte2(const unsigned char LeadByte, unsigned int* pLower, unsigned int* pUpper) {

  /* Syntax of valid UTF-8 Byte Sequences found at https://tools.ietf.org/html/rfc3629#section-4
   * which is why this range shifting must be done.
   *
   * UTF8-octets = *( UTF8-char )
   * UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
   * UTF8-1      = %x00-7F
   * UTF8-2      = %xC2-DF UTF8-tail
   * UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) / %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )
   * UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) / %xF4 %x80-8F 2( UTF8-tail )
   * UTF8-tail   = %x80-BF
   */

   unsigned int& Lower = *pLower;
   unsigned int& Upper = *pUpper;
   if (LeadByte == 0xE0) {
      Lower = 0xA0;
   } else if (LeadByte == 0xF0) {
      Lower = 0x90;
   }

   if (LeadByte == 0xED) {
      Upper = 0x9F;
   } else if (LeadByte == 0xF4) {
      Upper = 0x8F;
   }
}

static signed int checkNextChars(int HowMany, const unsigned char FirstByte, const char* Source, int SourceIndex, char* Buffer, unsigned int* pBufferCount) {
   COL_FUNCTION(checkNextChars);
   unsigned int& BufferCount = *pBufferCount;
   unsigned char Two =   Source[SourceIndex + 1];
   unsigned char Three = Source[SourceIndex + 2];
   unsigned char Four =  Source[SourceIndex + 3];

   // These are the standard lower and upper possible values for non-leading bytes in UTF-8 characters.
   // In some cases, the range for the second byte is different.
   unsigned int Lower  = 0x80;
   unsigned int Upper =  0xBF;
   unsigned int Lower2 = Lower;
   unsigned int Upper2 = Upper;
   switch (HowMany) {
      case 1:
         COL_TRC("Two bytes");
         if (Lower <= Two && Two <= Upper) {
            Buffer[BufferCount++] = FirstByte;
            Buffer[BufferCount++] = Two;
            return 1;
         } else {
            Buffer[BufferCount++] = '.';
            return 0;
         }
         break;
      case 2:
         adjustRangeForByte2(FirstByte, &Lower2, &Upper2);
         if ( (Lower2  <= Two && Two <= Upper2) &&
            (Lower <= Three && Three <= Upper) ) {
               Buffer[BufferCount++] = FirstByte;
               Buffer[BufferCount++] = Two;
               Buffer[BufferCount++] = Three;
               return 2;
         } else {
            Buffer[BufferCount++] = '.';
            return 0;
         }
         break;
      case 3:
         adjustRangeForByte2(FirstByte, &Lower2, &Upper2);
         if ( (Lower2  <= Two && Two <= Upper2)  &&
            (Lower <= Three && Three <= Upper) &&
            (Lower <= Four  && Four  <= Upper) ) {
               Buffer[BufferCount++] = FirstByte;
               Buffer[BufferCount++] = Two;
               Buffer[BufferCount++] = Three;
               Buffer[BufferCount++] = Four;
               return 3;
         } else {
            Buffer[BufferCount++] = '.';
            return 0;
         }
         break;
      default:
         COLASSERT(false);
         return -1;
         break;
   }
}


//
// Emit JSON strings with or without surrounding double quotes.
// Exceedingly non-standard - emits special characters as \xHH, *WHICH IS NOT JSON*.
// Encoding: StrEncCpp - encodes ctrl chars and non-ascii as \xXX
// Encoding: StrEncJs  - encodes ctrl chars. Replaces invalid UTF-8 bytes with "."
//   THIS IS THE ONLY encoding that produces valid JSON.
//
// Encoding: StrEncRaw - does not encode anything. Will pass data through. Careful with that one.
//
void COLappendJsonString(COLstring* Out, const COLstring& In, bool WithQuotes, int encoding) {
   COL_FUNCTION(COLappendJsonString);
   const int BufferSize = 256;
   char      Buffer[BufferSize];    // Minimize expensive calls to COLstring::append.
   const int BufferHighWater = sizeof(Buffer) - 7; // Pad by 2 extra just to be safe.
   unsigned int       BufferCount = 0;
   if (encoding==COLvar::StrEncRaw) {
      if (WithQuotes) { Out->append("\""); }
      Out->append(In);
      if (WithQuotes) { Out->append("\""); }
   } else {
      if (WithQuotes) { Buffer[BufferCount++] = '\"'; }
      const char* p = In.c_str();
      const int   N = In.size();
      for (int i = 0; i < N; ++i) {
         COL_VAR(i);
         const unsigned char Ch = p[i];
         switch (Ch) {
            case '\\': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = Ch ; break;
            case '\"': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = Ch ; break;
            case '\n': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = 'n'; break;
            case '\r': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = 'r'; break;
            case '\t': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = 't'; break;
            case '\f': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = 'f'; break;
            case '\b': Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = 'b'; break;

            // \v is not allowed in JSON, so if this serialization is going near a browser,
            // this escaping will get caught by the general handler for control characters.
            case '\v': if(encoding != COLvar::StrEncJs) {
                            Buffer[BufferCount++] = '\\'; Buffer[BufferCount++] = 'v'; break;
                       }
            default  : if (0x20 <= Ch && Ch < 0x7F) {
                          // Printable ASCII
                          Buffer[BufferCount++] = Ch; // Emit as-is
                       } else if (encoding == COLvar::StrEncJs) {

                          // ASCII control characters are not allowed in JSON.
                          if (Ch < 0x20 || Ch == 0x7F) {
                             COL_TRC("Escaping control char.");
                             BufferCount += ::snprintf(Buffer + BufferCount, BufferSize - BufferCount, "\\u%04X", Ch);

                          } else if (Ch == 0xC0 || Ch == 0xC1 || Ch >= 0xF5) {
                             COL_TRC("Removing illegal byte.");
                             Buffer[BufferCount++] = '.';

                          } else if (0x80 <= Ch  && Ch <= 0xBF) {
                             COL_TRC("Looks like a standalone second or later byte of UTF-8 sequence. Removing.");
                             Buffer[BufferCount++] = '.';

                          // Multi-byte UTF-8
                          } else if (0xC2 <= Ch && Ch <= 0xDF) {
                             unsigned int NumNum = (unsigned int)Ch;
                             COL_TRC(Ch  << " (int value is: " << NumNum << ")" << " looks like the first byte of a two-byte sequence");
                             i += checkNextChars(1, Ch, p, i, Buffer, &BufferCount);

                          } else if (0xE0 <= Ch && Ch <= 0xEF) {
                             unsigned int NumNum = (unsigned int)Ch;
                             COL_TRC(Ch  << " (int value is: " << NumNum << ")" << " looks like the first byte of a three-byte sequence");
                             i += checkNextChars(2, Ch, p, i, Buffer, &BufferCount);

                          } else {
                             unsigned int NumNum = (unsigned int)Ch;
                             COL_TRC(Ch  << " (int value is: " << NumNum << ")" << " looks like the first byte of a four-byte sequence");
                             i += checkNextChars(3, Ch, p, i, Buffer, &BufferCount);
                          }

                       } else { // Emit as \xXXX
                          BufferCount += ::snprintf(Buffer + BufferCount, BufferSize - BufferCount, "\\x%02X", Ch);
                       }
         }
         if (BufferCount >= BufferHighWater) {
            Out->append(Buffer, BufferCount);
            BufferCount = 0;
         }
      }
      if (WithQuotes) { Buffer[BufferCount++] = '\"'; }
      if (BufferCount>0) { Out->append(Buffer, BufferCount); }
   }
}

COLint32 COLvarKeyCompare::compare(const COLstring& A, const COLstring& B) {
   int r = A.icompare(B);
   if (r) {
      return r;
   }
   return A.compare(B);
}

COLvar::~COLvar() {
   destroy_();
}

COLvar::COLvar() {
   type_ = None;
}

COLvar::COLvar(bool value) {
   set_(value);
}

COLvar::COLvar(int value) {
   set_(value);
}

COLvar::COLvar(long long value) {
   set_(value);
}

COLvar::COLvar(double value) {
   set_(value);
}

COLvar::COLvar(const COLstring& value) {
   set_(value);
}

COLvar::COLvar(const char* value) {
   set_(value);
}

COLvar::COLvar(const char* str, int len) {
   set_(str, len);
}

// deep copy
COLvar::COLvar(const COLvar& that) {
   copy_(that);
}

void COLvar::set(bool value) {
   destroy_();
   set_(value);
}

void COLvar::set(int value) {
   destroy_();
   set_(value);
}

void COLvar::set(long long value) {
   destroy_();
   set_(value);
}

void COLvar::set(double value) {
   destroy_();
   set_(value);
}

void COLvar::set(const COLstring& value) {
   destroy_();
   set_(value);
}

void COLvar::set(const char* value) {
   destroy_();
   set_(value);
}

void COLvar::set(const char* str, int len) {
   destroy_();
   set_(str, len);
}

void COLvar::swap(COLstring* pString)
{
   COLstring* this_colstring = (COLstring*)u_.colstring_;
   if (type_ != String) {
      destroy_();
      type_ = String;
      new (this_colstring) COLstring();
   }
   this_colstring->swap(pString);
}

void COLvar::append(const COLstring& value) {
   append(value.c_str(), value.size());
}

void COLvar::append(const char* value) {
   append(value, ::strlen(value));
}

void COLvar::append(const char* str, int len) {
   if (!isString()) {
      if (isBool() || isInt() || isDouble()) {
         // convert number to string
         set(asString());
      } else {
         setType(COLvar::String);
      }
   }
   //COLASSERT(isString());
   COLstring* s = (COLstring*)u_.colstring_;
   s->append(str, len);
}

COLvar& COLvar::operator=(bool value) {
   set(value);
   return *this;
}

COLvar& COLvar::operator=(int value) {
   set(value);
   return *this;
}

COLvar& COLvar::operator=(long long value) {
   set(value);
   return *this;
}

COLvar& COLvar::operator=(double value) {
   set(value);
   return *this;
}

COLvar& COLvar::operator=(const COLstring& value) {
   set(value);
   return *this;
}

COLvar& COLvar::operator=(const char* value) {
   set(value);
   return *this;
}

COLvar& COLvar::operator=(const COLvar& that)
{
   if (this != &that) {
      switch (type_) {
      case Array:
      case Map:
         // must use a temp value for arrays and maps just
         // in case we are copying a child element into this
         {
            COLvar tmp(that);
            destroy_();
            swap(&tmp);
         }
         break;
      case String:
         destroy_();
         // no break - fall through
      default:
         copy_(that);
         break;
      }
   }
   return *this;
}

bool COLvar::asBool() const
{
   switch (type_) {
   case Integer:
   case Boolean:
      return u_.int_ != 0;
   case Double:
      return u_.double_ != 0;
   case String:
      {
         const COLstring& s = *(COLstring*)u_.colstring_;
         if (s.icompare("true")  == 0){ return true;  }
         if (s.icompare("false") == 0){ return false; }
         return ::atoi(s.c_str()) != 0;
      }
      break;
   case Array:
   case Map:
   default:
      break;
   }
   return 0;
}

long long COLvar::asInt() const
{
   switch (type_) {
   case Integer:
   case Boolean:
      return u_.int_;
   case Double:
      return int(u_.double_);
   case String:
      {
         const COLstring& s = *(COLstring*)u_.colstring_;
         return ::atoll(s.c_str());
      }
      break;
   case Array:
   case Map:
   default:
      break;
   }
   return 0;
}

double COLvar::asDouble() const
{
   switch (type_) {
   case Double:
      return u_.double_;
   case Integer:
   case Boolean:
      return u_.int_;
   case String:
      {
         const COLstring& s = *(COLstring*)u_.colstring_;
         return ::atof(s.c_str());
      }
   case Array:
   case Map:
   default:
      break;
   }
   return 0;
}

COLstring COLvar::asString() const
{
   char buf[256];
   switch (type_) {
   case String:
      return *(COLstring*)u_.colstring_;
      break;
   case Integer:
      ::snprintf(buf, 256, "%lld", u_.int_);
      return buf;
   case Boolean:
      return u_.int_ ? "true" : "false";
   case Double:
      {
         const int n = int(u_.double_);
         if (n == u_.double_) {
            ::snprintf(buf, 256, "%d", n);
         } else {
            ::snprintf(buf, 256, "%g", u_.double_);
         }
         return buf;
      }
      break;
   case Array:
   case Map:
   default:
      break;
   }
   return COLstring();
}

COLstring COLvar::asEscapedString(int encoding) const {
   COLstring out;
   COLappendJsonString(&out, asString(), false, encoding);
   return out;
}

static const COLstring s_EmptyString;

const COLstring& COLvar::str() const
{
   if (String == type_) {
      return *(COLstring*)u_.colstring_;
   }
   return s_EmptyString;
}

bool COLvar::getBool(const COLstring& key) const
{
   if (const COLvar* child = get(key)) {
      return child->asBool();
   }
   return false;
}

long long COLvar::getInt(const COLstring& key) const
{
   if (const COLvar* child = get(key)) {
      return child->asInt();
   }
   return 0;
}

double COLvar::getDouble(const COLstring& key) const
{
   if (const COLvar* child = get(key)) {
      return child->asDouble();
   }
   return 0;
}

COLstring COLvar::getString(const COLstring& key) const
{
   if (const COLvar* child = get(key)) {
      return child->asString();
   }
   return COLstring();
}

bool COLvar::getBool(int idx) const
{
   if (const COLvar* child = get(idx)) {
      return child->asBool();
   }
   return false;
}

long long COLvar::getInt(int idx) const
{
   if (const COLvar* child = get(idx)) {
      return child->asInt();
   }
   return 0;
}

double COLvar::getDouble(int idx) const
{
   if (const COLvar* child = get(idx)) {
      return child->asDouble();
   }
   return 0;
}

COLstring COLvar::getString(int idx) const
{
   if (const COLvar* child = get(idx)) {
      return child->asString();
   }
   return COLstring();
}

static void indent(COLstring* out, int N)
{
   const int INDENT = 4;
   const int max = 1000;
   static int s_Initialized;
   static char buf[INDENT*max+1];
   if (0 == s_Initialized) {
      memset(buf, ' ', sizeof(buf));
      buf[sizeof(buf)-1] = 0;
      s_Initialized = 1;
   }
   *out += '\n';
   if (N > max) {
      N = max;
   }
   N *= INDENT;
   out->append(buf, N);
}

COLstring COLvar::escape(const COLstring& in, int encoding)
{
   COLstring out;
   COLappendJsonString(&out, in, false, encoding);
   return out;
}

COLstring COLvar::json(int N, int encoding) const
{
   COLstring s;
   char buf[256];
   switch (type_) {
   case None:
      s = "null";
      break;
   case String:
      COLappendJsonString(&s, *(COLstring*)u_.colstring_, true, encoding);
      break;
   case Integer:
      {
         const int len = ::snprintf(buf, 256, "%lld", u_.int_);
         s.set(buf, len);
      }
      break;
   case Boolean:
      s = u_.int_ ? "true" : "false";
      break;
   case Double:
      {
         int len = 0;
         const int n = int(u_.double_);
         if (n == u_.double_) {
            len = ::snprintf(buf, 256, "%d", n);
         } else {
            len = ::snprintf(buf, 256, "%g", u_.double_);
         }
         s.set(buf, len);
      }
      break;
   case Array:
      {
         if (u_.vec_ && u_.vec_->size() > 0) {
            bool isFirst = true;
            s += '[';
            if (N) { indent(&s,N); }
            for (COLvarVec::iterator it = u_.vec_->begin(); it != u_.vec_->end(); ++it) {
               if (!isFirst) {
                  s += ',';
                  if (N) { indent(&s,N); }
               }
               isFirst = false;
               s.append(it->json(N>0 ? N+1 : 0, encoding));
            }
            if (N) { indent(&s,N-1); }
            s += ']';
         } else {
            // empty vector
            s += "[]";
         }
      }
      break;
   case Map:
      {
         if (u_.map_ && u_.map_->size() > 0) {
            bool isFirst = true;
            s += '{';
            if (N) { indent(&s,N); }
            for (COLvarMap::iterator it = u_.map_->begin(); it != u_.map_->end(); ++it) {
               if (!isFirst) {
                  s += ',';
                  if (N) { indent(&s,N); }
               }
               isFirst = false;
               COLappendJsonString(&s, it->first, true, encoding);
               s += ':';
               if (N) s += ' ';
               s.append(it->second.json(N>0 ? N+1 : 0, encoding));
            }
            if (N) { indent(&s,N-1); }
            s += '}';
         } else {
            // empty map
            s += "{}";
         }
      }
      break;
   default:
      break;
   }
   return s;
}

void COLvar::set_(const COLstring& s)
{
   type_ = String;
   COLstring* this_colstring = (COLstring*)u_.colstring_;
   new (this_colstring) COLstring(s);
}

void COLvar::set_(const char* s)
{
   type_ = String;
   COLstring* this_colstring = (COLstring*)u_.colstring_;
   new (this_colstring) COLstring(s);
}

void COLvar::set_(const char* s, int n)
{
   type_ = String;
   COLstring* this_colstring = (COLstring*)u_.colstring_;
   new (this_colstring) COLstring(s,n);
}

void COLvar::swap(COLvar* that)
{
   char tmp[sizeof(COLvar)];
   memcpy((void*)tmp,  this, sizeof(COLvar));
   memcpy((void*)this, that, sizeof(COLvar));
   memcpy((void*)that, tmp,  sizeof(COLvar));
}

void COLvar::copy_(const COLvar& that)
{
   switch (that.type_) {
   case Integer:
   case Boolean:
      u_.int_ = that.u_.int_;
      break;
   case Double:
      u_.double_ = that.u_.double_;
      break;
   case String:
      {
         COLstring* that_colstring = (COLstring*)that.u_.colstring_;
         COLstring* this_colstring = (COLstring*)u_.colstring_;
         new (this_colstring) COLstring(*that_colstring);
      }
      break;
   case Array:
      u_.vec_ = 0;
      if (that.u_.vec_) {
         u_.vec_ = new COLvarVec(*that.u_.vec_);
      }
      break;
   case Map:
      u_.map_ = new COLvarMap(*that.u_.map_);
      break;
   default:
      break;
   }
   type_ = that.type_;
}

void COLvar::destroy_()
{
   switch (type_) {
   case String:
      {
         COLstring* colstring = (COLstring*)u_.colstring_;
         colstring->~COLstring();
      }
      break;
   case Array:
      delete u_.vec_;
      break;
   case Map:
      delete u_.map_;
      break;
   default:
      break;
   }
   type_ = None;
   u_.double_ = 0;
   //rule_ = 0;
   //span_.reset();
}

COLvar& COLvar::operator[](int i)
{
   switch (type_) {
   case None:
   case Integer:
   case Boolean:
   case Double:
   case String:
      // destroy the scalar value and make this object an array
      destroy_();
      u_.vec_ = 0;
      type_ = Array;
      // no break - fall through
   case Array:
      // Only use a vector representation if 0 <= i <= 1000
      // OR index i is less or equal twice the previous vector size.
      if (i >= 0) {
         // Later have to make a spase array class
         if (i <= 1000 || (u_.vec_ && i <= u_.vec_->size() * 2)) {
            // we're dealing with an array element.
            if (0 == u_.vec_) {
               // create vector if null.
               u_.vec_ = new COLvarVec();
               u_.vec_->setCapacity(8);
            }
            if (u_.vec_->size() < i+1) {
               // fill array with empty COLvars if required
               // in case it is accessed in a sparse manner.
               u_.vec_->resize(i+1);
            }
            // return the array element
            return (*u_.vec_)[i];
         }
      }
      // index is out of range for a vector - convert the vector to a map
      convertToMap_();
      break;
   case Map:
      break;
   }
   // if we got this far, we must be dealing with a map
   COLASSERT(type_==Map);
   COLASSERT(u_.map_);
   return (*u_.map_)[COLintToString(i)];
}

COLvar& COLvar::operator[](const COLstring& key)
{
   convertToMap_();
   return (*u_.map_)[key];
}

void COLvar::convertToMap_()
{
   switch (type_) {
   case Map:
      break;
   case Array:
      {
         COLvarVec* pVec = u_.vec_;
         COLvarMap* pMap = new COLvarMap();
         if (pVec) {
            const int vec_size = pVec->size();
            for (int i = 0; i < vec_size; ++i) {
               const COLvar& var = (*pVec)[i];
               if (var.type() != COLvar::None) {
                  // map keys are the array indexes converted to strings
                  (*pMap)[COLintToString(i)] = var;
               }
            }
            delete pVec;
         }
         u_.map_ = pMap;
         type_ = Map;
      }
      break;
   case String:
      destroy_();
      // no break - fall through
   case Integer:
   case Boolean:
   case Double:
   case None:
      u_.map_ = new COLvarMap();
      type_ = Map;
      break;
   }
   COLASSERT(type_==Map);
}

COLvar* COLvar::get(int i)
{
   //COL_METHOD(COLvar::get(int));
   switch (type_) {
   case Array:
      if (u_.vec_ && i >= 0 && i < u_.vec_->size()) {
         return &(*u_.vec_)[i];
      }
      // no break - fall through
   case Map:
      return get(COLintToString(i));
   default:
      break;
   }
   return NULL;
}

const COLvar* COLvar::get(int i) const
{
   //COL_METHOD(COLvar::get(int) const);
   return const_cast<COLvar*>(this)->get(i);
}

COLvar* COLvar::get(const COLstring& key)
{
   //COL_METHOD(COLvar::get(const COLstring&));
   if (type_ == COLvar::Map && u_.map_) {
      COLvarMap::iterator it = u_.map_->find(key);
      if (it != u_.map_->end()) {
         return &it->second;
      }
   }
   return NULL;
}

const COLvar* COLvar::get(const COLstring& key) const
{
   //COL_METHOD(COLvar::get(const COLstring&) const);
   return const_cast<COLvar*>(this)->get(key);
}

void COLvar::erase(const COLstring& key)
{
   if (type_ == Map && u_.map_) {
      u_.map_->erase(key);
   }
}

void COLvar::remove(int idx)
{
   if (type_ == Array && u_.vec_){
      u_.vec_->remove(idx);
   }
}

static const COLvar s_NullVar;

const COLvar& COLvar::operator[](int i) const
{
   if (const COLvar* pVar = get(i)) {
      // Found the element matching index i.
      return *pVar;
   }
   // Index i not found - return a global static null COLvar.
   COLASSERT(s_NullVar.isNull()); // Make sure no one alters this instance.
   return s_NullVar;
}

const COLvar& COLvar::operator[](const COLstring& key) const
{
   if (const COLvar* pVar = get(key)) {
      // Found the element matching key.
      return *pVar;
   }
   // Key not found - return a global static null COLvar.
   COLASSERT(s_NullVar.isNull()); // Make sure no one alters this instance.
   return s_NullVar;
}

void COLvar::resize(int NewArraySize)
{
   if (type_ == Array) {
      if (0 == u_.vec_) {
         u_.vec_ = new COLvar::COLvarVec();
      }
      u_.vec_->resize(NewArraySize);
   }
}

int COLvar::size() const
{
   switch (type_) {
   case Array:
      if (u_.vec_) {
         return u_.vec_->size();
      }
      return 0;
   case Map:
      return u_.map_->size();
   default:
      break;
   }
   return 0; // scalar elements have no children
}

const COLvar::COLvarVec& COLvar::array() const
{
   //COLASSERT(type_ == Array && u_.vec_);
   COLPRECONDITION(type_ == Array && u_.vec_);
   return *u_.vec_;
}

COLvar::COLvarVec& COLvar::array()
{
   COLPRECONDITION(type_ == Array && u_.vec_);
   return *u_.vec_;
}

const COLvar::COLvarMap& COLvar::map() const
{
   COLPRECONDITION(type_ == Map);
   return *u_.map_;
}

COLvar::COLvarMap& COLvar::map()
{
   COLPRECONDITION(type_ == Map);
   return *u_.map_;
}

COLvar& COLvar::push_back(const COLvar& value)
{
   if (type_ == None) {
      type_ = Array;
      u_.vec_ = 0;
   }
   COLPRECONDITION(type_ == Array);
   if (u_.vec_ == 0) {
      u_.vec_ = new COLvar::COLvarVec();
   }
   return u_.vec_->push_back(value);
}

void COLvar::pop_back()
{
   COLPRECONDITION(type_ == Array && u_.vec_ != NULL && u_.vec_->size() > 0);
   u_.vec_->pop_back();
}

COLvar& COLvar::setType(COLvar::Type type)
{
   COL_METHOD(COLvar::setType);
   COL_VAR(type);

   if (type_ != type) {
      if (type == Map) {
         convertToMap_();
      }
      else {
         destroy_();
         type_ = type;
         if (type == Array) {
            u_.vec_ = new COLvar::COLvarVec();
         }
      }
   }
   COLASSERT(type == type_);
   return *this;
}

// Note: String payload can be binary.
// Ordering is not lexical. "Z" is less than "ABC" due to shorter length.
static inline int fastBinaryStringCompare(const COLstring& A, const COLstring& B) {
   const int sizeA = A.size();
   const int sizeB = B.size();
   if (sizeA == sizeB) {
      return ::memcmp(A.get_buffer(), B.get_buffer(), sizeA);
   }
   return sizeA - sizeB;
}

int COLvar::fastCompare(const COLvar& A, const COLvar& B) {
   if (&A == &B) { return 0; }
   if (A.type_ != B.type_) { return int(A.type_) - int(B.type_); }
   switch(A.type_) {
   case None:
      return 0;
   case String: {
         const COLstring* pA = (COLstring*)A.u_.colstring_;
         const COLstring* pB = (COLstring*)B.u_.colstring_;
         return fastBinaryStringCompare(*pA, *pB);
      }
   case Boolean:
   case Integer:
      return A.u_.int_ - B.u_.int_;
   case Double:
      return (A.u_.double_ < B.u_.double_)? -1 : (A.u_.double_ > B.u_.double_);
   case Array: {
         const int as = A.size(), bs = B.size();
         if (as != bs) { return as - bs; }
         COLvarVec::const_iterator itrA=A.array().begin(), itrB=B.array().begin(), endA=A.array().end();
         for (; itrA!=endA ; ++itrA, ++itrB) {
            int ret = fastCompare(*itrA, *itrB);
            if(ret) { return ret; }
         }
      }
      return 0;
   case Map: {
         const int as = A.size(), bs = B.size();
         if (as != bs) { return as - bs; }
         COLvarMap::const_iterator itrA=A.map().begin(), itrB=B.map().begin(), endA=A.map().end();
         for (; itrA!=endA ; ++itrA, ++itrB) {
            // Assume that Map keys are binary
            int ret = fastBinaryStringCompare(itrA->first, itrB->first);
            if (ret) { return ret; }
            ret = fastCompare(itrA->second, itrB->second);
            if (ret) { return ret; }
         }
      }
      return 0;
   }
   return 0;
}

COLostream& operator<<(COLostream& Stream, const COLvar& Var){
   Stream << Var.json(0,COLvar::StrEncCpp);
   return Stream;
}

#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
   enum { s_IsLitteEndianMachine = 1 };
#else
   enum { s_IsLitteEndianMachine = 0 };
#endif

static inline void AppendByte(COLstring* s, unsigned char v) {
   *s += (char)v;
}

static void AppendVarInt(COLstring* s, COLint64 v)
{
   char Buffer[16];
   if (v <= 0x7f) {
      AppendByte(s, (unsigned char)(unsigned)v);
   } else {
      unsigned COLint64 u = v;
      int i = 0;
      while (u) {
         Buffer[i++] = (((unsigned)u) & 0x7f) | 0x80;
         u >>= 7;
      }
      COLASSERT(i>0);
      Buffer[i-1] &= 0x7f;
      s->append(Buffer, i);
   }
}

static void AppendDoubleLE(COLstring* s, double v)
{
   char* p = (char*)&v;
   char buf[8];
   if (s_IsLitteEndianMachine) {
      buf[0] = p[0];
      buf[1] = p[1];
      buf[2] = p[2];
      buf[3] = p[3];
      buf[4] = p[4];
      buf[5] = p[5];
      buf[6] = p[6];
      buf[7] = p[7];
   } else {
      buf[0] = p[7];
      buf[1] = p[6];
      buf[2] = p[5];
      buf[3] = p[4];
      buf[4] = p[3];
      buf[5] = p[2];
      buf[6] = p[1];
      buf[7] = p[0];
   }
   s->append(buf, 8);
}

void COLvar::toBinary(COLstring* Out) const
{
   switch (type_) {
   case None:
      AppendByte(Out, (unsigned char)None);
      break;
   case String: {
         const COLstring& str = *(COLstring*)u_.colstring_;
         const int strSize = str.size();
         if (strSize <= 14) {
            // Lengths of 0 - 14 can be stored in the hi bits of the Type byte.
            AppendByte(Out, (unsigned char)String | (strSize << 4));
         } else {
            // A string length of 15 in the hi bits means "read the
            // string length from the varint that follows the Type byte."
            AppendByte(Out, (unsigned char)String | 0xf0);
            AppendVarInt(Out, str.size());  // string length
         }
         Out->append(str);                  // string without nil
      }
      break;
   case Integer: {
         unsigned char Type;
         long long AbsoluteValue;
         if (u_.int_ >= 0) {
            Type          = VarIntPositive;
            AbsoluteValue = u_.int_;
         } else {
            Type          = VarIntNegative;
            AbsoluteValue = -u_.int_;
         } //? below
         if (AbsoluteValue <= 14) {
            // Absolute Values of 0 - 14 can be stored in the hi bits of
            // the Type byte.
            AppendByte(Out, Type | ((unsigned)AbsoluteValue << 4));
         } else {
            // a value of 15 in the hi bits means "read the absolute value from
            // the varint that follows the Type byte."
            AppendByte(Out, Type | 0xf0);
            AppendVarInt(Out, AbsoluteValue);
         }
      }
      break;
   case Boolean:
      if (u_.int_) {
         AppendByte(Out, (unsigned char)BooleanTrue);
      } else {
         AppendByte(Out, (unsigned char)BooleanFalse);
      }
      break;
   case Double:
      AppendByte(Out, (unsigned char)Double);
      AppendDoubleLE(Out, u_.double_);
      break;
   case Array:
      if (u_.vec_ && u_.vec_->size() > 0) {
         // non-zero number of array elements
         const unsigned NumElements = u_.vec_->size();
         if (NumElements <= 14) {
            // sizes 14 or smaller stored in hi bits of type byte
            AppendByte(Out, (unsigned char)Array | (NumElements << 4));
         } else {
            // type byte hi bits set to 15 meaning varint length following.
            AppendByte(Out, (unsigned char)Array | 0xf0);
            AppendVarInt(Out, NumElements); // varint number of elements
         }
         // append each element
         for (COLvarVec::iterator it = u_.vec_->begin(); it != u_.vec_->end(); ++it) {
            it->toBinary(Out);
         }
      } else {
         // This is an empty vector.
         // Hi bits of type byte are set to 0 indicating no elements following.
         AppendByte(Out, (unsigned char)Array);
      }
      break;
   case Map:
      if (u_.map_ && u_.map_->size() > 0) {
         // non-zero number of map pairs
         const unsigned NumPairs = u_.map_->size();
         if (NumPairs <= 14) {
            // sizes 14 or smaller stored in hi bits of type byte
            AppendByte(Out, (unsigned char)Map | (NumPairs << 4));
         } else {
            // type byte hi bits set to 15 meaning varint length following.
            AppendByte(Out, (unsigned char)Map | 0xf0);
            AppendVarInt(Out, NumPairs); // varint number of key/value pairs
         }
         // append each key/value pair
         for (COLvarMap::iterator it = u_.map_->begin(); it != u_.map_->end(); ++it) {
            AppendVarInt(Out, it->first.size()); // key length
            Out->append(it->first);              // key string without nil
            it->second.toBinary(Out);            // value
         }
      } else {
         // This is an empty map.
         // Hi bits of type byte are set to 0 indicating no pairs following.
         AppendByte(Out, (unsigned char)Map);
      }
      break;
   default:
      break;
   }
}

bool COLvar::parse(const COLstring& Data){
   COL_METHOD(COLvar::parse-COLstring);
   return COLparseJson(Data, this);
}

bool COLvar::parse(const char* pData, int Length){
   COL_METHOD(COLvar::parse);
   return COLparseJson(pData, this);
}