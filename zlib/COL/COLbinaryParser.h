#ifndef __COL_BINARY_PARSER_H__
#define __COL_BINARY_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: Binary parser for COLvar
//
// Description:
//
// Author: Andrew Vajoczki
// Date:   Mon 28 Feb 2022 11:55:12 EST
//---------------------------------------------------------------------------

class COLvar;
// class COLvarBinBuf;


//
// A buffering parser for a binary stream of COLvars.
//
//   // Create or reset() instance when data stream is (re)created.
//   COLvarBinaryStreamParser Parser_;
//
//   ...
//
//   // Example data stream read callback
//   Parser_.append(data, len);
//   while (Parser.shouldParse()) {
//      // Must loop over unparsed data as multiple COLvars may be present.
//      COLvar Var;
//      const COLvar::ParseState state = Parser.parse(&Var);
//      if (state == COLvar::kSUCCESS) {
//         ... process Var ...
//      } else if (state == COLvar::kFAILURE) {
//         // unrecoverable error in byte stream
//         ... close stream and break/return ...
//      }
//   }
//

class COLvarBinaryStreamParser {
public:
   COLvarBinaryStreamParser();
   ~COLvarBinaryStreamParser();

   // User of this class should append() data and then loop on shouldParse().
   bool shouldParse() const;

   // Discard any previously buffered parse data.
   void reset();

   // Append binary data to the parse buffer.
   int append(const char* data, int len);
   int append(const COLstring& data);

   //
   // Parse the buffer to see if any COLvar(s) are available to process.
   //
   // If kFAILURE is returned then you should drop the logical connection
   // and discard this data. After dropping the upstream logical connection
   // you may resuse this instance by invoking reset() if you wish.
   //
   // If kNEED_MORE_DATA is returned then *pVar is not valid and you
   // must append more data to this instance before you may attempt a 
   // subsequent parse.
   //
   // If kSUCCESS is returned then *pVar contains a valid COLvar.
   // If you passed a non-NULL pBytesConsumed argument then you
   // may interrogate *pBytesConsumed and call parse() again while
   // *pBytesConsumed is greater than zero and *pBytesConsumed value has
   // changed from the previous invocation.
   //
   COLvar::ParseState parse(COLvar* pVar, int* pBytesConsumed = NULL);

   // Return the size of the parse buffer.
   int countOfUnparsedBytes() const { return buf_.size(); }

   // for debug trace use only
   const char* peekUnparsed() const { return buf_.c_str(); }

private:
   COLvarBinaryStreamParser(const COLvarBinaryStreamParser&);
   COLvarBinaryStreamParser& operator=(const COLvarBinaryStreamParser&);

   COLstring buf_;
   bool      shouldParse_;
};


#endif // end of defensive include