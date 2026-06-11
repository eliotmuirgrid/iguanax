//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLostream
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 01/16/2000
//---------------------------------------------------------------------------
#include "COLostream.h"
#include "COLerror.h"
#include "COLsinkStandard.h"
#include "COLsinkString.h"
#include "COLlocker.h"

// Eliot at the moment I don't think this code is working.  Does it matter?
// Probably not since we don't have a lot of static objects with destructors that
// we need to trace though - I try not to have these at all since they are a headache - Eliot

#ifdef COL_ENABLE_TRACING
// COLcerr and COLcout resources must be leaked to accommodate
// COLlog tracing after exit() in static object destructors.
COLostream COLcerr(new COLsinkStandardErr(), false, false);
COLostream COLcout(new COLsinkStandardOut(), false, false);
#else
COLostream COLcerr(new COLsinkStandardErr(), true);
COLostream COLcout(new COLsinkStandardOut(), true);
#endif

COLostream COLclog(new COLsinkStandardOut(), true);

class COLostreamPrivate {
public:
   COLostreamPrivate(COLsink* ipSink, bool Owner)
    : IndentContent("   "),
#ifdef _WIN32 // #6155
      NewLine("\r\n"),
      NewLineAndIndent("\r\n"),
#else // POSIX
      NewLine("\n"),
      NewLineAndIndent("\n"),
#endif
      CountOfIndent(0),
      pSink(ipSink),
      NumberBase_(COLostream::Decimal),
#ifdef COL_ENABLE_TRACING
      m_Delete_pMember(true),
#endif
      IsSinkOwner(Owner)
   {
   }
   ~COLostreamPrivate(){}

   COLostream::ENumberBase NumberBase_;
   COLstring NewLine;
   COLstring IndentContent;
   COLstring NewLineAndIndent;
   COLsink*  pSink;
   COLuint32 CountOfIndent;
   bool      IsSinkOwner;

#ifdef COL_ENABLE_TRACING
   bool m_Delete_pMember;
#endif

   inline bool isDec() const { return NumberBase_ == COLostream::Decimal; }

   void makeIndentString() {
      // TODO - this is not at all optimized - Eliot
      NewLineAndIndent = NewLine;
      for (COLuint32 IndentIndex = 0; IndentIndex < CountOfIndent; IndentIndex++) {
         NewLineAndIndent += IndentContent;
      }
   }
};

COLostream::COLostream(COLsink* pSink, bool Owner)
{
   pMember = new COLostreamPrivate(pSink, Owner);
}

COLostream::COLostream(COLsink& Sink)
{
   pMember = new COLostreamPrivate(&Sink, false); // not the owner
}

COLostream::COLostream(COLstring& String){
   pMember = new COLostreamPrivate(new COLsinkString(String), true);
}

COLostream::COLostream(COLstring* pString, bool Owner){
   pMember = new COLostreamPrivate(new COLsinkString(pString, Owner), true);
}

#ifdef COL_ENABLE_TRACING
COLostream::COLostream(COLsink* pSink, bool Owner, bool Delete_pMember)
{
   pMember = new COLostreamPrivate(pSink, Owner);
   pMember->m_Delete_pMember = false;
}
#endif

COLostream::~COLostream()
{
#ifdef COL_ENABLE_TRACING
   // this may be needed by the leaked COLcout and COLcerr instances?
   // if (!pMember->m_Delete_pMember) { pMember->pSink->flush(); }
#endif
   if (pMember->IsSinkOwner)
   {
      delete pMember->pSink;
   }
#ifdef COL_ENABLE_TRACING
   if (pMember->m_Delete_pMember)
#endif
   {
      delete pMember;
   }
}

void COLostream::newline()
{
   // TODO - one of these days we should performance tune COLostream...Eliot
   pMember->pSink->write((COLuint8*)pMember->NewLineAndIndent.c_str(), pMember->NewLineAndIndent.size());
}

void COLostream::setNewLine(const char* NewLine)
{
   pMember->NewLine = NewLine;
   pMember->makeIndentString();
}

const char* COLostream::getNewLine() const
{
   return pMember->NewLine.c_str();
}

void COLostream::setIndentContent(const COLstring& IndentContent)
{
   pMember->IndentContent = IndentContent;
   pMember->makeIndentString();
}

const COLstring& COLostream::indentContent() const
{
   return pMember->IndentContent;
}

void COLostream::indent()
{
   pMember->CountOfIndent++;
   pMember->makeIndentString();
}

void COLostream::unindent()
{
   if (pMember->CountOfIndent > 0)
   {
      pMember->CountOfIndent--;
   }
   pMember->makeIndentString();
}

void COLostream::resetIndent()
{
   setCountOfIndent(0);
}

void COLostream::setCountOfIndent(COLuint32 NewValue)
{
   pMember->CountOfIndent = NewValue;
   pMember->makeIndentString();
}

COLuint32 COLostream::countOfIndent() const
{
   return pMember->CountOfIndent;
}

void COLostream::flush()
{
   pMember->pSink->flush();
}

bool COLostream::isSinkOwner() const
{
   return pMember->IsSinkOwner;
}

COLsink* COLostream::sink()
{
   return pMember->pSink;
}

COLsink* COLostream::setSink(COLsink* pSink, bool IsOwner)
{
   COLPRECONDITION(pSink != NULL);
   if (pMember->IsSinkOwner)
   {
      delete pMember->pSink;
   }
   pMember->pSink = pSink;
   pMember->IsSinkOwner = IsOwner;
   return pSink;
}

COLostream::ENumberBase COLostream::numberBase() const
{
   return pMember->NumberBase_;
}

void COLostream::setNumberBase(ENumberBase Base)
{
   pMember->NumberBase_ = Base;
}

COLostream& COLostream::operator<<(COLostream&(*Stream)(COLostream&))
{
   Stream(*this);
   return *this;
}

COLostream& COLostream::operator<<(COLostream&(*Stream)(COLostream&, int n))
{
   return *this;
}

COLostream& COLostream::operator<<(unsigned char Character)
{
   pMember->pSink->write(&Character, 1);
   return *this;
}

COLostream& COLostream::operator<<(char Character)
{
   pMember->pSink->write((COLuint8*)&Character,1);
   return *this;
}

COLostream& COLostream::operator<<(const char* pString)
{
   if (pString)
   {
      pMember->pSink->write((COLuint8*)pString, strlen(pString));
   }
   return *this;
}

COLostream& COLostream::operator <<(const COLstring& String)
{
   pMember->pSink->write(String.c_str(), String.size());
   return *this;
}

COLostream& COLostream::write(const void* Data, int Length)
{
   pMember->pSink->write((const char*)Data, Length);
   return *this;
}

COLostream& COLostream::operator<<(COLint32 Number)
{
   char Buffer[32];
   int CountOfChar = snprintf(Buffer, 32, pMember->isDec()? "%i": "%X", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLuint32 Number)
{
   char Buffer[32];
   int CountOfChar = snprintf(Buffer, 32, pMember->isDec()? "%u": "%X", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLint16 Number)
{
   char Buffer[32];
   int CountOfChar = snprintf(Buffer, 32, pMember->isDec()? "%hi": "%hX", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLuint16 Number)
{
   char Buffer[32];
   int CountOfChar = snprintf(Buffer, 32, pMember->isDec()? "%hu": "%hX", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLint64 Number)
{
   char Buffer[64];
   int CountOfChar = snprintf(Buffer, 64, pMember->isDec()? "%lli": "%llX", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLuint64 Number)
{
   char Buffer[64];
   int CountOfChar = snprintf(Buffer, 64, pMember->isDec()? "%llu": "%llX", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLfloat32 Number)
{
   char Buffer[512]; // take no chances with outputting doubles
   int CountOfChar = snprintf(Buffer, 512, "%f", Number);
   pMember->pSink->write((COLuint8*)Buffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(COLfloat64 Number)
{
   char CharBuffer[512]; // take no chances with outputting doubles
   int CountOfChar = snprintf(CharBuffer, 512, "%f", Number);
   pMember->pSink->write(CharBuffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(const void* Pointer)
{
   char CharBuffer[32];
   // %p is portable, but doesn't output in a standard format
   // int CountOfChar = snprintf(CharBuffer, 32, "%p", Pointer);
#ifdef _WIN32 //Needed for MSVC and MSYS2 toolchains, mingw toolchain is compliant and does not need this
   int CountOfChar = snprintf(CharBuffer, 32, "0x%I64x", Pointer);
#else
   COL_STATIC_ASSERT(sizeof(Pointer)==sizeof(unsigned long));
   int CountOfChar = snprintf(CharBuffer, 32, "0x%lx", (unsigned long)Pointer);
#endif
   pMember->pSink->write(CharBuffer, CountOfChar);
   return *this;
}

COLostream& COLostream::operator<<(bool Boolean)
{
   if (Boolean) {
      return operator<<("True");
   }
   return operator<<("False");
}

COLostream& flush(COLostream& _outs) {
   _outs.flush();
   return _outs;
}

COLostream& newline(COLostream& _outs) {
   _outs.newline();
   return _outs;
}

COLostream& hex(COLostream& _outs)
{
   _outs.setNumberBase(COLostream::Hexadecimal);
   return _outs;
}

COLostream& decimal(COLostream& _outs)
{
   _outs.setNumberBase(COLostream::Decimal);
   return _outs;
}

void __set_indent(COLostream& OutStream, const COLstring& IndentContent)
{
   OutStream.setIndentContent(IndentContent);
}

COLostream& indent(COLostream& OutStream)
{
   OutStream.indent();
   return OutStream;
}

COLostream& unindent(COLostream& OutStream)
{
   OutStream.unindent();
   return OutStream;
}

COLostream& reset_indent(COLostream& OutStream)
{
   OutStream.resetIndent();
   return OutStream;
}

COLostreamManip::~COLostreamManip()
{
}

COLostreamManip* set_indent(const COLstring& TagName)
{
   return new COLostreamManip1<COLstring>(__set_indent, TagName);
}

// To avoid a heap allocated COLstring, the address of the yet-to-be-constructed
// COLostreamString.m_String data member is passed to the parent COLostream's
// constructor. This is okay because that pointer to the COLstring member
// is not directly or indirectly used by COLostreamString's ctor or dtor.
// There is a test in COLexample to verify this assumption with valgrind.
//
COLostreamString::COLostreamString()
: COLostream(m_String)
{
}

COLostreamString::~COLostreamString()
{
}

const char* COLostreamString::c_str() const
{
   return m_String.c_str();
}

const COLstring& COLostreamString::str() const
{
   return m_String;
}
