#ifndef __COL_OSTREAM_H__
#define __COL_OSTREAM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLostream
//
// Description:
//
// COLostream translates C and user defined types into text.  It's modeled
// on the standard iostream library.  Give it a read over - Eliot.
//
// Each COLostream object has a COLsink object which is used to
// write the actual formatted text.  Each COLstream object may or
// may not 'own' the COLsink object that it writes to.
//
// There are a number of standard global objects COLcout, COLcerr,
// COLclog which are commented on further down this file.
//
// To format numbers in hexidecimal we can do the following:
//
// COLcout << hex << Number << " (in hex) " << decimal << Number << 
//        " (in decimal) " << newline << flush;
//
// Note the use of the newline and flush operators.  These will
// add a newline and flush the sink respectively.  Since different
// operating systems have different conventions for newlines there is
// a setNewLine method which allows one to specify the string to insert
// for a newline.
//
// Sometimes I have found it necessary to write:
//
// COLcout << ::flush;  
//
// The problem here is when the above statement is in the context of
// a class which actually implements a flush method - basically anything
// which implements COLsink has this problem.
//
// COLostream can be useful way to convert numbers into strings i.e.
//
// COLuint32 MyNumber = 1221;
// COLstring MyString;
// COLostream Stream(MyString);
//
// Stream << MyNumber;
//
// Author: Eliot Muir
// Date:   Fri 01/16/2000 
//---------------------------------------------------------------------------

#include "COLstring.h"

class COLostreamPrivate;
class COLsink;

class COLostream
{
public:
   // This constructor takes ownership of the sink object.
   COLostream(COLsink* Sink, bool Owner);

   // This constructor does not take ownership of the sink object.
   COLostream(COLsink& Sink);

   // Not too proud of these constructors but they made it easier to stop
   // COLstring have a virtual interface implementing COLsink (Eliot).
   COLostream(COLstring& String); // Makes a COLsinkString which is owned by
                                  // COLostream but the sink does not own String.
   COLostream(COLstring* pString, bool Owner);

// IX-2358 - this is *okay* since adding an extra constructor doesn't alter the size of the class
// Talk to Eliot if you have questions.
#ifdef COL_ENABLE_TRACING
   COLostream(COLsink* Sink, bool Owner, bool Delete_pMember);
#endif

   virtual ~COLostream();

   void flush();
   void newline();

   void setNewLine(const char* NewLine);
   const char* getNewLine() const;

   // The indent content refers to what will be used to make an indented
   // entry.  By default this is set to "   " but one might for certain applications
   // set this to "\t" for a more space efficient indenting mechanism.
   // Most of the time it will be more elegant to use the overloaded operators
   // to invoke these indentation methods i.e.
   // Stream << set_indent("\t")
   //        << add_indent
   //        << "Blah"
   //        << remove_indent;
   // When one wants to remove all indentation one can do:
   // Stream << clear_indent;
   void setIndentContent(const COLstring& Indent);
   const COLstring& indentContent() const;

   void indent();
   void unindent();
   void resetIndent();
   void setCountOfIndent(COLuint32 NewValue);
   COLuint32 countOfIndent() const;

   // Write binary data of a fixed length - no strlen() operation performed.
   // Embedded nulls allowed.
   COLostream& write(const void* Data, int Length);

   COLostream& operator<<(COLostream&(*)(COLostream&)); 
   COLostream& operator<<(COLostream&(*)(COLostream&, int n)); 
   COLostream& operator<<(char Character);
   COLostream& operator<<(unsigned char Character);
   COLostream& operator<<(const char* pString);
   COLostream& operator<<(const COLstring& String);
   COLostream& operator<<(COLfloat64 Number);
   COLostream& operator<<(COLfloat32 Number);
   COLostream& operator<<(COLint16 Number);
   COLostream& operator<<(COLuint16 Number);
   COLostream& operator<<(COLint32 Number);
   COLostream& operator<<(COLuint32 Number);
   COLostream& operator<<(COLuint64 Number);   
   COLostream& operator<<(COLint64 Number);   
   COLostream& operator<<(bool Boolean);
   COLostream& operator<<(const void* Pointer);
   COLostream& operator<<(unsigned long Number) { return operator<<((COLuint32)Number); }   
   COLostream& operator<<(long Number) { return operator<<((COLint32)Number); }   

   // returns true if this COLostream owns its sink object.
   bool isSinkOwner() const;
   COLsink* sink();
   
   COLsink* setSink(COLsink* pSink, bool IsOwner);
   COLsink& setSink(COLsink& Sink) { return *setSink(&Sink, false); }

   enum ENumberBase { Decimal = 10, Hexadecimal = 16 };
   ENumberBase numberBase() const;

private:
   COLostreamPrivate* pMember;

   void setNumberBase( ENumberBase Base );

   friend COLostream& hex(COLostream& _outs);
   friend COLostream& decimal(COLostream& _outs);

   COLostream(const COLostream& Orig);
   COLostream& operator=(const COLostream& Orig);
};

// COLcerr, COLcout and COLclog are equivalent to the
// the standard iostream library's cerr, cout, clog 
// objects.  They are global COLostream objects that go
// to standard error, standard out and in the case of
// COLclog usually standard out.  

extern COLostream COLcerr;
extern COLostream COLcout;
extern COLostream COLclog;

COLostream& flush(COLostream& _outs);
COLostream& newline(COLostream& _outs);

COLostream& indent(COLostream& OutStream);
COLostream& unindent(COLostream& OutStream);
COLostream& reset_indent(COLostream& OutStream);

COLostream& hex(COLostream& _outs);
COLostream& decimal(COLostream& _outs);

class COLostreamManip {
public:
   virtual ~COLostreamManip();
   virtual void printOn(COLostream& Stream) const = 0;
};

// This complicated looking stuff is for implementation COLostream manipulators
// with arguments.
template<class TArgument>
class COLostreamManip1 : public COLostreamManip
{
public:
   typedef void (*TCOLostreamFunction)(COLostream&, const TArgument&);
   COLostreamManip1(TCOLostreamFunction ipFunction, const TArgument& iArgument)
   : pFunction(ipFunction),
     Argument(iArgument)
   {
   }
   virtual void printOn(COLostream& Stream) const
   {
      (*pFunction)(Stream, Argument);
   }
   TCOLostreamFunction pFunction;
   TArgument Argument;
};

template<class TArg1, class TArg2>
class COLostreamManip2 : public COLostreamManip
{
public:
   typedef void (*TCOLostreamFunction)(COLostream&, TArg1, TArg2);
   COLostreamManip2(TCOLostreamFunction pFunction, TArg1& Arg1, TArg2& Arg2)
   : m_pFunction(pFunction),
     m_Arg1(Arg1),
     m_Arg2(Arg2)
   {
   }
   virtual void printOn(COLostream& Stream) const
   {
      (*m_pFunction)(Stream, m_Arg1, m_Arg2);
   }
   TCOLostreamFunction m_pFunction;
   TArg1 m_Arg1;
   TArg2 m_Arg2;
};


template<class TArg1, class TArg2, class TArg3>
class COLostreamManip3 : public COLostreamManip
{
public:
   typedef void (*TCOLostreamFunction)(COLostream&, TArg1, TArg2, TArg3);
   COLostreamManip3(TCOLostreamFunction pFunction, TArg1& Arg1, TArg2& Arg2, TArg3& Arg3)
   : m_pFunction(pFunction),
     m_Arg1(Arg1),
     m_Arg2(Arg2),
     m_Arg3(Arg3)
   {
   }
   virtual void printOn(COLostream& Stream) const
   {
      (*m_pFunction)(Stream, m_Arg1, m_Arg2, m_Arg3);
   }
   TCOLostreamFunction m_pFunction;
   TArg1 m_Arg1;
   TArg2 m_Arg2;
   TArg3 m_Arg3;
};


inline COLostream& operator<<(COLostream& Stream, COLostreamManip* pManipulator)
{
   pManipulator->printOn(Stream);
   delete pManipulator;
   return Stream;
}

COLostreamManip* set_indent(const COLstring& TagName);

inline COLostream& operator<<(COLostream& Stream, const COLtextLiteral& Literal) {
   return Stream.write(Literal.m_String, Literal.m_Length);
}

class COLostreamString : public COLostream
{
public:
   COLostreamString();
   virtual ~COLostreamString();

   // The reference returned by str() is only valid until any subsequent
   // non-const method invocaton on the instance of COLostreamString.
   const COLstring& str() const;

   // The pointer returned by c_str() is only valid until any subsequent
   // non-const method invocaton on the instance of COLostreamString.
   const char* c_str() const;

   int size() const { return m_String.size(); }

   void clear() { m_String.clear(); }
private:
   COLstring m_String;
   COLostreamString(const COLostreamString&);            // disallow
   COLostreamString& operator=(const COLostreamString&); // disallow
};

#endif // end of defensive include

