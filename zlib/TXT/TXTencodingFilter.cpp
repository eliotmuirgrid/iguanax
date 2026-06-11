//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTencodingFilter
//
// Description:
//
// Implementation
//
// Author: Akshay Ganeshen
// Date:   Thu  4 Dec 2014 17:14:40 EST
//
//---------------------------------------------------------------------------
#include "TXTprecomp.h"
#pragma hdrstop

#include <ICONV/iconv.h>
#include "TXTencodingFilter.h"
#include "errno.h"            // julian added to try and fix make error "TXTencodingFilter.cpp:228:55: error: use of undeclared identifier 'errno'"

#include <COL/COLsink.h>
#include <COL/COLfilter.h>
#include <COL/COLstring.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Taken from iconv's aliases.h file: The internal buffer kept is of size MAX_WORD_SIZE(45) + 10 + 1
static const int LeftoverBufferSize = 45 + 10 + 1;

// Keep around an array of encodings that are available - determined at static initialization time

// This form is just a convenience - assumes the target is UTF8
TXTencodingFilter::TXTencodingFilter(COLsink* pNext, const COLstring& FromEncoding) : COLfilter(pNext) {
   COL_METHOD(TXTencodingFilter::TXTencodingFilter);
   m_IconvHandle = NULL;
   m_FromEncoding = "null";
   m_ToEncoding = "null";
   m_LeftoverBuffer = COLstring();
   m_LeftoverBuffer.setCapacity(LeftoverBufferSize);
   initIconvHandle(FromEncoding, COL_T("UTF-8"));
}

TXTencodingFilter::TXTencodingFilter(COLsink* pNext, const COLstring& FromEncoding, const COLstring& ToEncoding) : COLfilter(pNext) {
   COL_METHOD(TXTencodingFilter::TXTencodingFilter);
   m_IconvHandle = NULL;
   m_FromEncoding = "null";
   m_ToEncoding = "null";
   m_LeftoverBuffer = COLstring();
   m_LeftoverBuffer.setCapacity(LeftoverBufferSize);
   initIconvHandle(FromEncoding, ToEncoding);
}

TXTencodingFilter::~TXTencodingFilter() {
   COL_METHOD(TXTencodingFilter::~TXTencodingFilter);
   freeIconvHandle();
}

void TXTencodingFilter::initIconvHandle(const COLstring& FromEncoding, const COLstring& ToEncoding) {
   COL_METHOD(TXTEncodingFilter::initIconvHandle);
   COL_VAR2(FromEncoding, ToEncoding);

   // Note : iconv_open has the target encoding as the first argument - careful!!!
   m_IconvHandle = ICONViconv_open(ToEncoding.c_str(), FromEncoding.c_str());
   // Don't throw here if the handle creation failed - this might have been created with a call to 'new'

   // Only copy over the strings if they're unequal, to protect against self-assignment
   if (m_FromEncoding != FromEncoding) {
      m_FromEncoding = FromEncoding;
   }
   if (m_ToEncoding != ToEncoding) {
      m_ToEncoding = ToEncoding;
   }
}

void TXTencodingFilter::freeIconvHandle() {
   COL_METHOD(TXTencodingFilter::freeIconvHandle);

   if (m_IconvHandle && m_IconvHandle != (iconv_t)(-1)) {
      ICONViconv_close(m_IconvHandle);
      m_IconvHandle = NULL;
   }
}

void TXTencodingFilter::resetFilter() {
   COL_METHOD(TXTencodingFilter::resetFilter);
   freeIconvHandle();
   // This doesn't throw, and is adequately protected against self-assignment
   initIconvHandle(m_FromEncoding, m_ToEncoding);
   m_LeftoverBuffer.clear();  // retain the capacity that was hinted from construction though
}

bool TXTencodingFilter::isSupported() const {
   COL_METHOD(TXTencodingFilter::isSupported);
   return (m_IconvHandle && m_IconvHandle != (iconv_t)(-1));
}

COLuint32 TXTencodingFilter::write(const void* vpBuffer, COLuint32 SizeOfBuffer) {
   COL_METHOD(TXTencodingFilter::write);

   if (!isSupported()) {
      // TODO : Differentiate between 'unsupported' and 'unrecognized' encodings
      COL_ERROR_STREAM("Encoding '" << m_FromEncoding << "' cannot be converted to encoding '" << m_ToEncoding << "'.", COLerror::PreCondition);
   }
   COLsink& OutSink = next();

   // Create temporary buffers for iconv to read from and write into, and then write that into the sink
   // This method is essentially just acting as a middleman for iconv to throw data into the COLsink
   static const int TempBufferSize = 1024;   // anything greater than LeftoverBufferSize will work, but higher numbers avoid calls to iconv() & next().write() too
   char OutBuffer[TempBufferSize];
   char InBuffer[TempBufferSize];
   size_t OutBufferBytes = 0; // needs to be size_t to work properly with iconv
   size_t InBufferBytes = 0;

   const char* pBuffer = (const char*)vpBuffer;
   int BytesProcessed = 0;   // nice to have around for reporting errors

   // Enter a conversion loop until all the bytes in the buffer are handled (either converted, or kept around in the leftover-buffer)
   while (m_LeftoverBuffer.size() || BytesProcessed < SizeOfBuffer) {
      int AvailableBytes;  // need to declare before any potential goto statements
      if (InBufferBytes == TempBufferSize) {
         goto InBufferReady;
      }
      // First carry over any leftover bytes
      if (m_LeftoverBuffer.size()) {
         // Having TempBufferSize be a constant and a power of two makes this check fast, and for practical purposes, leftover size should always be smaller
         AvailableBytes = COL_MIN(TempBufferSize - InBufferBytes, m_LeftoverBuffer.size());
#ifdef COL_ENABLE_TRACING
         if (m_LeftoverBuffer.size() > TempBufferSize) {
            COL_WRN("Text encoding filter's temp buffer size might be a bottleneck to performance - try increasing it");
         }
#endif /* COL_ENABLE_TRACING */
         ::memcpy(InBuffer + InBufferBytes, m_LeftoverBuffer.c_str(), AvailableBytes);
         m_LeftoverBuffer.shift(AvailableBytes);
         InBufferBytes += AvailableBytes;
      }

      if (InBufferBytes == TempBufferSize) {
         goto InBufferReady;
      }
      // Next, start processing data from the buffer
      AvailableBytes = COL_MIN(TempBufferSize - InBufferBytes, SizeOfBuffer - BytesProcessed);
      ::memcpy(InBuffer + InBufferBytes, pBuffer + BytesProcessed, AvailableBytes);
      BytesProcessed += AvailableBytes;
      InBufferBytes += AvailableBytes;

InBufferReady:
      // Being pretty lazy about writing the buffer out, and trying to work with powers of 2 as much as possible
      int TotalBytesWritten = 0;
      while (OutBufferBytes - TotalBytesWritten > TempBufferSize / 2) {   // L.H.S. & ~((TempBufferSize >> 1) - 1)
         int BytesWritten = OutSink.write(OutBuffer + TotalBytesWritten, OutBufferBytes - TotalBytesWritten);
         TotalBytesWritten += BytesWritten;
      }
      if (TotalBytesWritten && TotalBytesWritten != OutBufferBytes) {
         // Ugh, didn't write everything, so have to do some copying over of the remainder
         ::memmove(OutBuffer, OutBuffer + TotalBytesWritten, TempBufferSize - TotalBytesWritten);
         OutBufferBytes -= TotalBytesWritten;
      } else if (TotalBytesWritten) {
         // Everything was written out
         OutBufferBytes = 0;
      }

      // Need to set up some temporaries, as iconv works with pointers and updates the information in them
      const char* InBufferReadPoint = InBuffer;
      size_t InBufferSize = InBufferBytes;
      char* OutBufferWritePoint = OutBuffer + OutBufferBytes;
      size_t OutBufferSize = TempBufferSize - OutBufferBytes;
      // Let the converting begin!
      COL_DUMP("iconv on", InBuffer, InBufferBytes);
      size_t Result = ICONViconv(m_IconvHandle, &InBufferReadPoint, &InBufferSize, &OutBufferWritePoint, &OutBufferSize);

      // Result is set with the number of non-reversible conversions, not number of bytes outputted
      // So update the bytes available in the output regardless
      OutBufferBytes = TempBufferSize - OutBufferSize;
      COL_DUMP("iconv result", OutBuffer, OutBufferBytes);
      
      // Input will be updated based on the result of conversion
      if (Result == (size_t)(-1)) {
         COL_TRC("Result was -1, there might be an error");
         // Not necessarily an error - iconv sets errno with more information
         if (errno == EINVAL || errno == E2BIG) {
            // These aren't actually errors in conversion, just that we need to manipulate the buffers for iconv
            // EINVAL: There wasn't enough data to form a full sequence, so get some more data
            // E2BIG: The output buffer is not big enough to hold the result, so output before converting more
            // In either case, iconv will have gone up to the value currently in InBufferReadPoint
            COL_VAR(InBufferSize);
            if (InBufferSize) {  // carry the remainder forward for the next iteration
               ::memmove(InBuffer, InBufferReadPoint, InBufferSize);
               InBufferBytes = InBufferSize;
            } else {
               // Weird, but ok, just follow what iconv says and set the input buffer as cleared
               COL_TRC("Weird - iconv claims to have converted all the bytes in the read buffer but still returned -1 - continuing on...");
               InBufferBytes = 0;
            }

            if (errno == EINVAL) {
               COL_TRC("EINVAL: Waiting for more data");
               // Continue onto the next loop iteration
            } else {
               COL_TRC("E2BIG: Will write out buffer before continuing");
               goto InBufferReady;
            }
         } else if (errno == EILSEQ) {
            // A true error - illegal sequence in the from-encoding
            // Gather a bit of information and then error it out
            size_t BytePosition = BytesProcessed - InBufferSize;
            // Give the hex values of a few bytes after the start of the illegal sequence
            COLstring Context;
            COLsinkString ContextSink(Context);
            COLostream ContextStream(ContextSink);
            int ContextBytes = COL_MIN(5, InBufferSize);
            const char* pRead = InBufferReadPoint;
            ContextStream << "Illegal byte sequence encountered at position " << BytePosition;
            ContextStream << ". Showing " << ContextBytes << " byte";
            ContextStream << ((ContextBytes == 1) ? "" : "s") << ":";
            hex(ContextStream);
            while (ContextBytes--) {
               ContextStream << " " << (unsigned)(*pRead & 0xFF);
               pRead++;
            }
            COL_ERROR_STREAM(Context, COLerror::SystemError);
         } else {
            // A more involved C error, let strerror handle that one
            COL_THROW_STRERROR("Error during iconv:");
         }
      } else {
         // Assume everything was processed
         COL_TRC("Result was alright - assuming everything was processed");
         InBufferBytes = 0;
      }
   }  // end while loop => (BytesProcessed == SizeOfBuffer)
   COLPOSTCONDITION(!m_LeftoverBuffer.size());
   COLPOSTCONDITION(BytesProcessed == SizeOfBuffer);
   // Write out the rest of the data in OutBuffer
   if (OutBufferBytes) {
      int TotalBytesWritten = 0;
      while (TotalBytesWritten != OutBufferBytes) {
         int BytesWritten = OutSink.write(OutBuffer + TotalBytesWritten, OutBufferBytes - TotalBytesWritten);
         TotalBytesWritten += BytesWritten;
      }
      OutBufferBytes = 0;
   }
   COLPOSTCONDITION(!OutBufferBytes);
   // If InBuffer still has anything in it, it's to be carried over to the next call of write
   if (InBufferBytes) {
      m_LeftoverBuffer = COLstring(InBuffer, InBufferBytes);
      COL_VAR(m_LeftoverBuffer);
   }
   return BytesProcessed;
}

int TXTencodingFilter::unprocessedCharacterCount() const {
   COL_METHOD(TXTencodingFilter::unprocessedCharacterCount);

   return m_LeftoverBuffer.size();
}

void TXTencodingFilter::onEndStream() {
   COL_METHOD(TXTencodingFilter::onEndStream);

   if (unprocessedCharacterCount()) {
      COL_ERROR_STREAM("Encoding conversion is expecting more characters.", COLerror::SystemError);
   }

   next().onEndStream();
}

COLstring TXTencode(void* pBuffer, int Size, const COLstring& FromEncoding, const COLstring& ToEncoding) {
   COL_FUNCTION(TXTencode);
   COLstring Result;
   COLsinkString Sink(Result);
   TXTencodingFilter Encoder(&Sink, FromEncoding, ToEncoding);
   Encoder.write(pBuffer, Size);
   Encoder.onEndStream();  // throw and let throw
   return Result;
}

COLstring TXTencode(const COLstring& Data, const COLstring& FromEncoding, const COLstring& ToEncoding) {
   return TXTencode((void*)Data.c_str(), Data.size(), FromEncoding, ToEncoding);
}
