#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include "COLbinaryParser.h"

COL_LOG_MODULE;

#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
   enum { s_IsLitteEndianMachine = 1 };
#else
   enum { s_IsLitteEndianMachine = 0 };
#endif

// require lightweight non-COLerror exceptions for parsing
struct COLvarBinBufMoreDataEx {};
struct COLvarBinBufFailureEx {};

class COLvarBinBuf {
public:
   COLvarBinBuf(const void* str, int len)
   : str_((unsigned char*)str)
   , size_(len)
   , readPos_(0) {
      COLASSERT(str_);
      COLASSERT(size_ >= 0);
   }
   int size() const {
      return size_;
   }
   int pos() const {
      return readPos_;
   }
   const unsigned char* read(int bytes) {
      const unsigned char* p = available(bytes);
      readPos_ += bytes;
      return p;
   }
   inline unsigned char readByte() {
      const unsigned char* p = read(1);
      const char ch = *p;
      return ch;
   }
   int readVarInt64() {
      unsigned COLint64 u = 0;
      unsigned b;
      for (int i = 0; (b = readByte()) && i < 10; ++i) {
         u |= (b & 0x7f) << (i * 7);
         if ((b & 0x80) == 0)
            break;
      }
      return u;
   }
   inline int readVarInt() {
      return (int)readVarInt64();
   }
   double readDoubleLE() {
      double v = 0;
      const unsigned char* p = read(8);
      unsigned char* buf = (unsigned char*)&v;
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
      return v;
   }
private:
   // If bytes available then return a pointer to the read position
   // otherwise throw COLvarBinBufMoreDataEx
   const unsigned char* available(int bytes) const {
      COLASSERT(bytes >= 0);
      if (size_ - readPos_ - bytes >= 0) {
         return str_ + readPos_;
      } else {
         throw COLvarBinBufMoreDataEx();
      }
      return 0;
   }
   const unsigned char* str_;
   const int size_;
   int readPos_;
};

COLvar::ParseState COLvar::parseBinary(COLvarBinBuf* bin)
{
   const unsigned char Type = bin->readByte();
   const unsigned HiBits = (Type & 0xf0) >> 4;
   switch (Type & 0xf) {
   case None:
      reset();
      break;
   case String: {
         const int slen = (HiBits <= 14) ? HiBits : bin->readVarInt();
         if (slen < 0 || slen > 25000000) {
            // string length is negative or too large
            throw COLvarBinBufFailureEx();
         }
         if (const char* p = (const char*)bin->read(slen)) {
            set(p, slen);
         }
      }
      break;
   case VarIntPositive: {
         const long long n = (HiBits <= 14) ? HiBits : bin->readVarInt();
         if (n < 0) {
            // integer payload must not be negative
            throw COLvarBinBufFailureEx();
         }
         set(n);
      }
      break;
   case VarIntNegative: {
         const long long n = (HiBits <= 14) ? HiBits : bin->readVarInt();
         if (n < 0) {
            // raw value must not be negative as the sign is assigned later
            throw COLvarBinBufFailureEx();
         }
         set(-n);
      }
      break;
   case Boolean: {
         // technically we don't need to process the Boolean type
         const char ch = bin->readByte();
         const bool b = ch ? true : false;
         set(b);
      }
      break;
   case BooleanTrue:
      set(true);
      break;
   case BooleanFalse:
      set(false);
      break;
   case Double: {
         const double n = bin->readDoubleLE();
         set(n);
      }
      break;
   case Array: {
         setArrayType();
         const int n = (HiBits <= 14) ? HiBits : bin->readVarInt();
         if (n < 0 || n > 1000000) {
            // Array size is negative or too large
            throw COLvarBinBufFailureEx();
         }
         for (int i = 0; i < n; ++i) {
            COLvar& e = push_back();
            const COLvar::ParseState state = e.parseBinary(bin);
            if (state != kSUCCESS) {
               return state;
            }
         }
      }
      break;
   case Map: {
         setMapType();
         const int n = (HiBits <= 14) ? HiBits : bin->readVarInt();
         if (n < 0 || n > 10000) {
            // Map size is negative or too large
            throw COLvarBinBufFailureEx();
         }
         for (int i = 0; i < n; ++i) {
            const int klen = bin->readVarInt();
            if (klen < 0 || klen > 10000) {
               // Map key string length is negative or too large
               throw COLvarBinBufFailureEx();
            }
            if (const char* p = (const char*)bin->read(klen)) {
               COLvar& e = (*this)[COLstring(p,klen)];
               const COLvar::ParseState state = e.parseBinary(bin);
               if (state != kSUCCESS) {
                  return state;
               }
            } else {
               return kFAILURE;
            }
         }
      }
      break;
   default:
      // invalid COLvar binary protocol type
      throw COLvarBinBufFailureEx();
      //return kFAILURE;
   }
   return kSUCCESS;
}

COLvar::ParseState COLvar::parseBinary(const void* data, int len, int* pBytesConsumed)
{
   COLASSERT(sizeof(int)==4);
   reset();
   if (pBytesConsumed) {
      *pBytesConsumed = 0;
   }
   COLvar::ParseState state = kFAILURE;
   if (data && len >= 0) {
      // len is zero or more bytes
      COLvarBinBuf bin(data, len);
      try {
         state = parseBinary(&bin);
         if (state == kSUCCESS) {
            if (pBytesConsumed) {
               *pBytesConsumed = bin.pos();
            }
         }
      }
      catch (COLvarBinBufFailureEx& ex) {
         state = kFAILURE;
      }
      catch (COLvarBinBufMoreDataEx& ex) {
         state = kNEED_MORE_DATA;
      }
   }
   return state;
}

COLvarBinaryStreamParser::COLvarBinaryStreamParser() {
   COL_METHOD(COLvarBinaryStreamParser::COLvarBinaryStreamParser);
   shouldParse_ = false;
}

COLvarBinaryStreamParser::~COLvarBinaryStreamParser() {
   COL_METHOD(COLvarBinaryStreamParser::~COLvarBinaryStreamParser);
}

bool COLvarBinaryStreamParser::shouldParse() const {
   return shouldParse_;
}

void COLvarBinaryStreamParser::reset() {
   COL_METHOD(COLvarBinaryStreamParser::reset);
   buf_.clear();
   shouldParse_ = false;
}

int COLvarBinaryStreamParser::append(const char* data, int len) {
   COL_METHOD(COLvarBinaryStreamParser::append(const char*,int));
   COL_VAR(len);
   if (len > 0) {
      buf_.append(data, len);
      shouldParse_ = true;
   }
   return countOfUnparsedBytes();
}

int COLvarBinaryStreamParser::append(const COLstring& data) {
   COL_METHOD(COLvarBinaryStreamParser::append(const COLstring&));
   return append(data.data(), data.size());
}

COLvar::ParseState COLvarBinaryStreamParser::parse(COLvar* pVar, int* pBytesConsumed) {
   COL_METHOD(COLvarBinaryStreamParser::parse);
   COL_VAR2(pVar, pBytesConsumed);
   int BytesConsumed = 0;
   const COLvar::ParseState state = pVar->parseBinary(buf_.data(), buf_.size(), &BytesConsumed);
   if (pBytesConsumed) {
      *pBytesConsumed = BytesConsumed;
   }
   if (state == COLvar::kSUCCESS) {
      // We have successfully parsed a COLvar off the stream.
      // We must now shift down the unparsed bytes in the buffer.
      COLASSERT(BytesConsumed > 0);
      COLASSERT(BytesConsumed <= buf_.size());
      buf_ = COLstring(buf_.data() + BytesConsumed, buf_.size() - BytesConsumed);
      shouldParse_ = (countOfUnparsedBytes() > 0);
   }
   else {
      // state is either kFAILURE or kNEED_MORE_DATA here.
      shouldParse_ = false;
   }
   COL_VAR3(state, countOfUnparsedBytes(), shouldParse_);
   return state;
}

