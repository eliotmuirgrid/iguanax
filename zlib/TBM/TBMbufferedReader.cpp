//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMbufferedReader
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, February 13th, 2007 @ 01:51:31 PM
//
//---------------------------------------------------------------------------
#include "TBMprecomp.h"
#pragma hdrstop

#include "TBMbufferedReader.h"
#include "TBMreader.h"


enum TBMbufferState
{
   TBM_BUFFER_EMPTY=0,
   TBM_BUFFER_NEED_MORE_DATA=1,
   TBM_BUFFER_NEED_STRING_SIZE=2
};

class TBMbufferedReaderPrivate
{
public:
   TBMbufferedReaderPrivate() 
    : BufferState(TBM_BUFFER_EMPTY),
      AmountNeeded(0)
   {}
   COLstring TokenBuffer;
   TBMreader Reader;
   TBMbufferState BufferState;

   COLuint32 AmountNeeded;

   TBMresult write(const char* cpBuffer, COLuint32 SizeOfBuffer, COLuint32& ParsedUntil);

   void reset();
};

TBMbufferedReader::TBMbufferedReader()
{
   pMember = new TBMbufferedReaderPrivate;
}

TBMbufferedReader::~TBMbufferedReader()
{
   delete pMember;
}

// This is exposed so we can initialize it.
TSMhandlerStack& TBMbufferedReader::handlerStack()
{
   return pMember->Reader.handlerStack();
}

TBMresult TBMbufferedReader::write(const char* cpBuffer, COLuint32 SizeOfBuffer, COLuint32& ParsedUntil)
{
   return pMember->write(cpBuffer, SizeOfBuffer, ParsedUntil);
}

void TBMbufferedReader::reset()
{
   pMember->reset();
}

void TBMbufferedReaderPrivate::reset()
{
   BufferState = TBM_BUFFER_EMPTY;
   AmountNeeded = 0;
   TokenBuffer.setSize(0);
}

// The code here looks a bit hairy but appears to work from the unit testing
// I have performed on it.  At least the functionality of this class is reasonably
// well defined so it could be ripped out and replaced with another implementation
// should this implementation prove problematic in the future - Eliot

TBMresult TBMbufferedReaderPrivate::write(const char* cpBuffer, COLuint32 SizeOfBuffer, COLuint32& ParsedUntil)
{
   switch(BufferState)
   {
   case TBM_BUFFER_EMPTY:
      {
         ParsedUntil = 0;
         AmountNeeded = 0;
         TBMresult Result = Reader.parse(cpBuffer, SizeOfBuffer, ParsedUntil, AmountNeeded);
         switch(Result)
         {         
         case TBM_COMPLETE:
         case TBM_UNKNOWN_BINARY_TOKEN:         
            reset();
            return Result;
            break;            
         case TBM_INCOMPLETE_TOKEN:
            TBMASSERT(AmountNeeded > 0);
            TokenBuffer.write(cpBuffer + ParsedUntil, SizeOfBuffer - ParsedUntil);            
            BufferState = TBM_BUFFER_NEED_MORE_DATA;
            ParsedUntil = SizeOfBuffer;
            return Result;            
         case TBM_INCOMPLETE:
            // but not an incomplete token
            TBMASSERT(AmountNeeded == 0);
            TBMASSERT(BufferState == TBM_BUFFER_EMPTY);
            TBMASSERT(ParsedUntil == SizeOfBuffer);
            return Result;
         case TBM_NEED_STRING_SIZE:
            // need to buffer the additional data
            TBMASSERT(AmountNeeded > 0);
            TokenBuffer.write(cpBuffer + ParsedUntil, SizeOfBuffer - ParsedUntil);
            TBMASSERT(AmountNeeded > TokenBuffer.size());
            BufferState = TBM_BUFFER_NEED_STRING_SIZE;
            ParsedUntil = SizeOfBuffer;
            return Result;
            break;
         default:
            TBMASSERT(1==0);  // unknown return code
            return TBM_INTERNAL_ERROR;
         }
      }
      break;
   case TBM_BUFFER_NEED_MORE_DATA:
      {
         if (SizeOfBuffer + TokenBuffer.size() < AmountNeeded)
         {
            // not quite enough data
            TokenBuffer.write(cpBuffer, SizeOfBuffer);
            ParsedUntil = SizeOfBuffer;
            return TBM_INCOMPLETE;
         }
         else if (SizeOfBuffer + TokenBuffer.size() == AmountNeeded)
         {
            // we've got just enough data
            TokenBuffer.write(cpBuffer, SizeOfBuffer);
            ParsedUntil = 0;
            TBMresult Result = Reader.parse((const char*)TokenBuffer.data(), TokenBuffer.size(), ParsedUntil, AmountNeeded);
            TBMASSERT(AmountNeeded == 0);
            switch(Result)
            {         
            case TBM_COMPLETE:
            case TBM_UNKNOWN_BINARY_TOKEN:
            case TBM_INCOMPLETE:
               ParsedUntil = SizeOfBuffer;
               reset();
               return Result;
               break;            
            case TBM_INCOMPLETE_TOKEN:
            case TBM_NEED_STRING_SIZE:
            default:            
               TBMASSERT(1==0);  // This shouldn't happen.
               return TBM_INTERNAL_ERROR;            
            }  
         }
         else
         {
            // we have more data than we need for the next token.
            COLuint32 BiteOutOfBuffer = AmountNeeded-TokenBuffer.size();

            TokenBuffer.write(cpBuffer, BiteOutOfBuffer);
            ParsedUntil = 0;
            TBMresult Result = Reader.parse((const char*)TokenBuffer.data(), TokenBuffer.size(), ParsedUntil, AmountNeeded);
            TBMASSERT(AmountNeeded == 0);   
            switch(Result)
            {         
            case TBM_COMPLETE:
               ParsedUntil = SizeOfBuffer;
            case TBM_UNKNOWN_BINARY_TOKEN:
               reset();
               return Result;
               break;            
            case TBM_INCOMPLETE_TOKEN:
            case TBM_NEED_STRING_SIZE:
               TBMASSERT(1==0); // shouldn't happen
               return TBM_INTERNAL_ERROR;
               break;
            case TBM_INCOMPLETE:
               // now we handle the rest of the data.
               {
                  reset();
                  ParsedUntil = 0;
                  TBMresult SecondResult = write(cpBuffer + BiteOutOfBuffer, SizeOfBuffer-BiteOutOfBuffer, ParsedUntil);
                  ParsedUntil += BiteOutOfBuffer;  // we have to correct this value
                  return SecondResult;
               }
            default:
               TBMASSERT(1==0);  // unknown return code
               return TBM_INTERNAL_ERROR;
            }  
         }
      }
   case TBM_BUFFER_NEED_STRING_SIZE:
      {
         if (TokenBuffer.size() + SizeOfBuffer < AmountNeeded)
         {
            // not quite enough data
            TokenBuffer.write(cpBuffer, SizeOfBuffer);
            ParsedUntil = SizeOfBuffer;
            return TBM_NEED_STRING_SIZE;
         }
         else if (TokenBuffer.size() + SizeOfBuffer == AmountNeeded)
         {
            // we've got just enough data to find the size of the string
            TokenBuffer.write(cpBuffer, SizeOfBuffer);
            ParsedUntil = 0;
            TBMresult Result = Reader.parse((const char*)TokenBuffer.data(), TokenBuffer.size(), ParsedUntil, AmountNeeded);
            switch(Result)
            {         
            case TBM_COMPLETE:
            case TBM_UNKNOWN_BINARY_TOKEN:         
            case TBM_NEED_STRING_SIZE:
               TBMASSERT(1==0);  // none of the above should have happened  
               return TBM_INTERNAL_ERROR;
               break;           
            case TBM_INCOMPLETE_TOKEN: 
               ParsedUntil = SizeOfBuffer;
               BufferState = TBM_BUFFER_NEED_MORE_DATA;
               return TBM_INCOMPLETE_TOKEN;
            case TBM_INCOMPLETE:  // empty string
               // This is what we expect
               ParsedUntil = SizeOfBuffer;
               reset();
               return Result;
            default:
               TBMASSERT(1==0);  // unknown return code
               return TBM_INTERNAL_ERROR;
            }  
         }
         else
         {
            // We have enough data for the size of the string - it's not certain how much more we'll need.
            COLuint32 BiteOfBuffer = AmountNeeded-TokenBuffer.size();
            TokenBuffer.write(cpBuffer, BiteOfBuffer);
            ParsedUntil = 0;
            TBMresult Result = Reader.parse((const char*)TokenBuffer.data(), TokenBuffer.size(), ParsedUntil, AmountNeeded);
            switch(Result)
            {         
            case TBM_COMPLETE:
            case TBM_UNKNOWN_BINARY_TOKEN:         
            case TBM_NEED_STRING_SIZE:
               TBMASSERT(1==0);  // none of the above should have happened  
               return TBM_INTERNAL_ERROR;
               break;            
            case TBM_INCOMPLETE_TOKEN:
               if (AmountNeeded > SizeOfBuffer - BiteOfBuffer + TokenBuffer.size())
               {
                  // we still need more data
                  BufferState = TBM_BUFFER_NEED_MORE_DATA;
                  TokenBuffer.write(cpBuffer+BiteOfBuffer, SizeOfBuffer-BiteOfBuffer);
                  ParsedUntil = SizeOfBuffer;
                  return TBM_INCOMPLETE_TOKEN;
               }
               else
               {
                  ParsedUntil = 0;
                  COLuint32 ExtraBiteNeededForStringBody = AmountNeeded - TokenBuffer.size();
                  TokenBuffer.write(cpBuffer+BiteOfBuffer, ExtraBiteNeededForStringBody);
                  BufferState = TBM_BUFFER_NEED_MORE_DATA;
                  TBMresult Result2 = Reader.parse((const char*)TokenBuffer.data(), TokenBuffer.size(), ParsedUntil, AmountNeeded);
                  TBMASSERT(AmountNeeded == 0);
                  switch(Result2)
                  {
                  case TBM_COMPLETE:
                  case TBM_UNKNOWN_BINARY_TOKEN:
                  case TBM_NEED_STRING_SIZE:
                  case TBM_INCOMPLETE_TOKEN:
                     TBMASSERT(1==0);  // none of the above should have happened  
                     return TBM_INTERNAL_ERROR;
                     break;          
                  case TBM_INCOMPLETE:
                     {
                        // We need more data but there should be none left over.
                        ParsedUntil = 0;
                        reset();
                        if (SizeOfBuffer-BiteOfBuffer-ExtraBiteNeededForStringBody > 0)
                        {                       
                           TBMresult SecondResult = write(cpBuffer+BiteOfBuffer+ExtraBiteNeededForStringBody, SizeOfBuffer-BiteOfBuffer-ExtraBiteNeededForStringBody, ParsedUntil);
                           if(SecondResult == TBM_COMPLETE)
                           {
                              // #5504 - It's entirely possible for there to be extra data in the buffer once a parse is complete,
                              // such as when you have two messages back-to-back.
                              TBMASSERT(ParsedUntil + BiteOfBuffer + ExtraBiteNeededForStringBody <= SizeOfBuffer);
                              ParsedUntil += BiteOfBuffer + ExtraBiteNeededForStringBody;
                           }  
                           else
                           {
                              TBMASSERT(ParsedUntil + BiteOfBuffer + ExtraBiteNeededForStringBody == SizeOfBuffer);
                              ParsedUntil = SizeOfBuffer;
                           }                           
                           return SecondResult;
                        }
                        else
                        {
                           ParsedUntil = SizeOfBuffer;
                           return TBM_INCOMPLETE;
                        }
                     }
                     break; 
                  default:
                     TBMASSERT(1==0);  // unexpected return code
                     return TBM_INTERNAL_ERROR;
                  }
               }
            case TBM_INCOMPLETE:
               {
                  // we basically got a zero length string
                  reset();
                  ParsedUntil = 0;
                  TBMresult SecondResult = write(cpBuffer+BiteOfBuffer, SizeOfBuffer-BiteOfBuffer, ParsedUntil);
                  ParsedUntil += BiteOfBuffer;
                  return SecondResult;
               }
               break;
            default:
               TBMASSERT(1==0);  // unknown return code
               return TBM_INTERNAL_ERROR;
            }  
         }
      }
      break;
   default:
      TBMASSERT(1==0);
      break;
   }
   
   return TBM_COMPLETE;
}
