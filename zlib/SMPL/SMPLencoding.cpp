//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: SMPLencoding
//
// Author: John Q
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "SMPLencoding.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <SFI/SFIhexEncodeFilter.h>
#include <SFI/SFIhexDecodeFilter.h>
COL_LOG_MODULE;


static void SMPLtokenizeByte(const unsigned char& Ch, const int& ReadPos, const char* pMessageCstr, int* pWritePos, COLstring* pBuffer, bool IsEol = false){
   COL_FUNCTION(SMPLtokenizeByte);
   COL_VAR2(ReadPos, *pWritePos);
   COL_VAR2(Ch, IsEol);

   COLstring Token("{{IFWTOKEN}}"); // This string is used in the JS.

   // Copy from last write pos up to current read pos.
   pBuffer->append(pMessageCstr + *pWritePos, ReadPos - *pWritePos);
   COL_VAR(*pBuffer);

   // Append the token
   COLstring HexValue;
   const char CastChar = (char)Ch;
   COLstring ByteAsCOLstring(&CastChar, 1);
   SFIhexEncode(ByteAsCOLstring, &HexValue);

   COL_VAR2(Ch, HexValue);

   COLstring FullToken;
   if (IsEol) {
      FullToken = Token + HexValue + Token + '\n';
   } else {
      FullToken = Token + HexValue + Token;
   }
   COL_VAR(FullToken);
   pBuffer->append(FullToken);
   COL_VAR(*pBuffer);
   *pWritePos = ReadPos + 1;
   COL_VAR2(ReadPos, *pWritePos);
}

static void SMPLadjustRangeForByte2(const unsigned char LeadByte, unsigned int* pLower, unsigned int* pUpper){
  /* Syntax of valid UTF-8 Byte Sequences found at https://tools.ietf.org/html/rfc3629#section-4
   * which is why this range shifting must be done. Note the ranges on UTF-3/4.
   *
   * UTF8-octets = *( UTF8-char )
   * UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
   * UTF8-1      = %x00-7F
   * UTF8-2      = %xC2-DF UTF8-tail
   * UTF8-3      = %xE0 %xA0-BF UTF8-tail / %xE1-EC 2( UTF8-tail ) / %xED %x80-9F UTF8-tail / %xEE-EF 2( UTF8-tail )
   * UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) / %xF1-F3 3( UTF8-tail ) / %xF4 %x80-8F 2( UTF8-tail )
   * UTF8-tail   = %x80-BF
   */

   switch (LeadByte) {
   case 0xE0:
      *pLower = 0xA0;
      break;
   case 0xED:
      *pUpper = 0x9F;
      break;
   case 0xF0:
      *pLower = 0x90;
      break;
   case 0xF4:
      *pUpper = 0x8F;
      break;
   }
}

static int SMPLcheckNextBytes(int HowMany, const unsigned char FirstByte, const char* Msg, int Pos){
   unsigned char Two    = Msg[Pos + 1];
   unsigned char Three  = Msg[Pos + 2];
   unsigned char Four   = Msg[Pos + 3];
   unsigned int  Lower  = 0x80;
   unsigned int  Upper  = 0xBF;
   unsigned int  Lower2 = Lower;
   unsigned int  Upper2 = Upper;

   switch (HowMany) {
   case 1:
      if (Lower <= Two && Two <= Upper) {
         return 1;
      }
      break;
   case 2:
      SMPLadjustRangeForByte2(FirstByte, &Lower2, &Upper2);
      if ( (Lower2 <= Two   && Two   <= Upper2)
        && (Lower  <= Three && Three <= Upper ) ) {
         return 2;
      }
      break;
   case 3:
      SMPLadjustRangeForByte2(FirstByte, &Lower2, &Upper2);
      if ( (Lower2 <= Two   && Two   <= Upper2)
        && (Lower  <= Three && Three <= Upper )
        && (Lower  <= Four  && Four  <= Upper ) ) { 
         return 3;
      }
      break;
   }
   return 0;
}

// static void ANNbyteToHexToken(const COLstring& Message, COLstring* pOut) {
void SMPLencode(const COLstring& Message, COLstring* pOut) {
   COL_FUNCTION(SMPLencode);
   COL_VAR2(Message.size(), pOut);

   unsigned char  Ch             = 0;
   unsigned char  NextCh         = 0;
   const char*    pMessageCstr   = Message.c_str();
   COLstring      &Buf           = *pOut;
   int            ToSkip         = 0;
   int            ReadPos        = 0;
   int            WritePos       = 0;
   int            MessageSize    = Message.size();

   for (; ReadPos < MessageSize; ++ReadPos) {
      ToSkip = 0;
      Ch = Message[ReadPos];
      NextCh = (ReadPos + 1 == MessageSize) ? false : Message[ReadPos + 1];

      COL_TRC(Ch  << " (int value: " << (unsigned int)Ch << ")");
      COL_VAR(Ch);

      if (Ch == 0x0D && NextCh == 0x0A) {
         COL_TRC("Tokenize a CR, then an LF (Windows EOL)");
         SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
         ReadPos++;
         SMPLtokenizeByte(Message[ReadPos],  ReadPos, pMessageCstr, &WritePos, &Buf, true);
      }
      else if (Ch == 0x0D || Ch == 0x0A) {
         COL_TRC("Tokenize a " << ((Ch == 0x0D) ? "CR" : "LF"));
         SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf, true);
      }
      else if (Ch >= 0x20 && Ch < 0x7F) {
         COL_TRC("Printable ASCII: " << Ch);
      }
      else if ((Ch < 0x20 && Ch != 0x0A && Ch != 0x0D) || Ch == 0x7F) {
         COL_TRC("Tokenize control character, but not a CR or LF");
         SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
      }
      else if (Ch == 0xC0 || Ch == 0xC1 || Ch >= 0xF5) {
         COL_TRC("Tokenize illegal byte.");
         SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
      }
      else if (Ch >= 0x80  && Ch <= 0xBF) {
         COL_TRC("Tokenize standalone second or later byte of UTF-8 sequence.");
         SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
      }
      else if (Ch >= 0xC2 && Ch <= 0xDF) {
         COL_TRC(Ch  << " (int value is: " << (unsigned int)Ch << ")" << " looks like the first byte of a two-byte sequence");
         ToSkip += SMPLcheckNextBytes(1, Ch, pMessageCstr, ReadPos);

         if (ToSkip == 0) {
            COL_TRC("Not a valid 2 byte sequence");
            SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
         } else if (ToSkip > 0) {
            COL_TRC("Valid 2 byte sequence, skipping " << ToSkip << " bytes");
            ReadPos += ToSkip;
         }
      }
      else if (Ch >= 0xE0 && Ch <= 0xEF) {
         unsigned int NumNum = (unsigned int)Ch;
         COL_TRC(Ch  << " (int value is: " << NumNum << ")" << " looks like the first byte of a three-byte sequence");
         ToSkip += SMPLcheckNextBytes(2, Ch, pMessageCstr, ReadPos);

         if (ToSkip == 0) {
            COL_TRC("Not a valid 3 byte sequence");
            SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
         }
         else if (ToSkip > 0) {
            COL_TRC("Valid 3 byte sequence, skipping " << ToSkip << " bytes");
            ReadPos += ToSkip;
         }
      }
      else if (Ch >= 0xF0 && Ch <= 0xF4) {
         unsigned int NumNum = (unsigned int)Ch;
         COL_TRC(Ch  << " (int value is: " << NumNum << ")" << " looks like the first byte of a four-byte sequence");
         ToSkip += SMPLcheckNextBytes(3, Ch, pMessageCstr, ReadPos);

         if (ToSkip == 0) {
            COL_TRC("Not a valid 4 byte sequence");
            SMPLtokenizeByte(Ch, ReadPos, pMessageCstr, &WritePos, &Buf);
         }
         else if (ToSkip > 0) {
            COL_TRC("Valid 4 byte sequence, skipping " << ToSkip << " bytes");
            ReadPos += ToSkip;
         }
      }
   }

   Buf.append(pMessageCstr + WritePos);
   COL_VAR(Buf);
}


static bool isByteIdentifyingToken(const COLstring& Message, const COLstring& Token, int Pos){
   COL_FUNCTION(isByteIdentifyingToken);
   COL_VAR2(Message[Pos], Message[Pos + 1]);
   if (Message[Pos] != '{' || Message[Pos + 1] != '{') {
      COL_TRC("It's not a token.");
      return false;
   }
   COL_TRC("FOUND '{{', Checking if it is a token.");
   for (int i = 0, n = Token.size(); i < n; i++, Pos++) {
      COL_VAR2(Message[Pos], Token[i]);
      if (Message[Pos] != Token[i]) {
         COL_TRC("It's not a token.");
         return false;
      }
   }
   COL_TRC("It's a token.");
   return true;
}

// static void ANNhexTokenToByte(const COLstring& Message, COLstring* pOut){
void SMPLdecode(const COLstring& Message, COLstring* pOut) {
   COL_FUNCTION(SMPLdecode);
   const char* MessageCstr = Message.c_str();
   COLstring &Buf = *pOut;
   const COLstring Token("{{IFWTOKEN}}");
   char Cur = 0;
   char Pre = 0;
   int   ReadPos     = 0;
   int   WritePos    = 0;
   int   MessageSize = Message.size();
   int   TokenSize   = Token.size();
   while (ReadPos < MessageSize) {
      Cur = Message[ReadPos];
      if (ReadPos + 1 < MessageSize && isByteIdentifyingToken(Message, Token, ReadPos)) {
         COL_VAR2(ReadPos, WritePos);
         // Copy up to the last char before the token into Buf.
         Buf.append(MessageCstr + WritePos, ReadPos - WritePos);
         COL_VAR(Buf);
         // Skip the first bit of the token.
         // Hat{{IFWTOKEN}}FF{{IFWTOKEN}}{{IFWTOKEN}}EE{{IFWTOKEN}}
         // ---^-----------^
         ReadPos += TokenSize;
         // Grab the Hex Value
         COLstring HexValue = Message.substr(ReadPos, 2);
         if      (HexValue == "CR") { HexValue = "0D"; }
         else if (HexValue == "LF") { HexValue = "0A"; }
         COL_VAR(HexValue);
         COLstring DecodedHex;
         if(!SFIhexDecode(HexValue, &DecodedHex)){
            COL_TRC("It was a valid token, but not a valid hex code: " << DecodedHex);
            // Copy the invalid token to the buffer.
            int FullTokenLength = TokenSize * 2 + 2;
            // Go back to before we advanced past the first half of the token.
            COLstring BadToken = Message.substr(ReadPos - TokenSize, FullTokenLength);
            Buf.append(BadToken);
            COL_VAR(Buf);
         }
         else{
            // Append the HexValue to Buf
            COL_VAR(DecodedHex);
            Buf.append(DecodedHex);
            COL_VAR(Buf);
         }
         // Advance past the Hex Value and the closing token.
         // Hat{{IFWTOKEN}}FF{{IFWTOKEN}}{{IFWTOKEN}}EE{{IFWTOKEN}}
         // ---------------^------------^
         ReadPos += TokenSize + HexValue.size() - 1;  // Minus one because the loop will add one more.
         WritePos = ReadPos   + 1;                    // ReadPos and WritePos should be equal at end.
      }                                               // Add one more since it's behind by one momentarily.
      ReadPos++;
   }
   // Copy from the last token to the end. If no tokens, this is the whole message.
   Buf.append(MessageCstr + WritePos);
   COL_VAR(Buf);
}

