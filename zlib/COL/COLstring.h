#ifndef __COL_STRING_H__
#define __COL_STRING_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLstring
// 
// Description
//
// String class originally derived from work by Lynn Prentice, but completely
// gutted and replaced with Andrew Vajoczki's string class.
// The implementation is a non ref-counted with short string optimization which
// turns out to be a lot faster than the old ref counted implementation.
//
// Everyone who programs with C++ at iNTERFACEWARE should give this code a good
// read - it's very important.  Pay careful attention to the difference between
// size() and capacity().
//
// COLstring can be used as a binary buffer.  i.e.
//
// FILfile Config("config.conf", FILfile::Read);
// COLstring Buffer;
// Buffer.setCapacity(1024);
// int AmountRead = Config.read(Buffer.c_str(), Buffer.capacity());
// Buffer.setSize(AmountRead); // this is an important line.
//
// We should use COLstring for buffers instead of other older legacy buffer classes.
//
// Author: Eliot Muir
//-----------------------------------------------------------------------------

#include "COLminimumInclude.h"

//=============================================================================
// The COL_T "literal string" optimization to avoid calling strlen().
//
// Examples:
//
//   COLstring MyString = COL_T("abc");
//   COLostream MyStringStream(MyString);
//   MyStringStream << COL_T("def")
//   MyString = COL_T("ghi");
//   MyString += COL_T("xyz");

#define COL_T(X_) COLtextLiteral((X_),(sizeof(X_)-1))

// NEVER use COLtextLiteral directly outside of the COL library - use COL_T() instead.
struct COLtextLiteral {
   inline COLtextLiteral(const char* StringLiteral, unsigned Length) : m_String(StringLiteral), m_Length(Length) {}
   // Do not add any methods or a destructor to this struct so it will be inlined.
   const char* m_String;
   unsigned    m_Length;
};

//=============================================================================

class COLstring {
public:
   enum StripType { Leading = 1, Trailing = 2, Both = Leading|Trailing };

   COLstring();
   COLstring(const COLstring& that);
   COLstring(const char* s);
   COLstring(const char* s, int length);
   COLstring(int InitialSize, char InitializationChar);

   // Optimization for C string literals to avoid a strlen() call when constructing a COLstring.
   // For example: 
   //   COLstring str("foo");        // not optimized - calls strlen internally.
   //   COLstring str(COL_T("foo")); // optimized.
   COLstring(const COLtextLiteral& stringLiteral);

   // Windows wide strings
   COLstring(const wchar_t* WideCharacterString);
   COLstring& operator=(const wchar_t* pValue);

   ~COLstring();

   int size() const { return _length; }       // Get length of string without trailing nil byte.

   int capacity() const { return _capacity; } // Get capacity. capacity() always greater than size().
   void setCapacity(int capacity);            // Hint to set new capacity. Capacity set may be larger.

   const bool is_null(void) const { return _length == 0; }

   const char* c_str() const;
   const char* data() const;

   // ExampleString = COL_T("xyz");
   inline COLstring& operator=(const COLtextLiteral& stringLiteral) {
      set(stringLiteral.m_String, stringLiteral.m_Length); return *this;
   }
   // ExampleString += COL_T("xyz");
   inline COLstring& operator+=(const COLtextLiteral& stringLiteral) {
      append(stringLiteral.m_String, stringLiteral.m_Length); return *this;
   }

   // string assignment - overwrite original.
   COLstring& operator=(const COLstring& that);
   COLstring& operator=(const char* s);
   void set(const char*, int len);  // set a string with length

   // append operators
   inline COLstring& operator+=(const char* s)      { return append(s); }    // append string
   inline COLstring& operator+=(const COLstring& s) { return append(s); }    // append COLstring
   inline COLstring& operator+=(char b)             { return append(1, b); } // append byte

   // char array access methods
   char& operator[](int pos);
   char operator[](int pos) const;

   void clear();  // Resets string to "", retaining its capacity.
   void zero();   // Resets string to "", reduces its capacity.

   // Discard 'n' bytes from the beginning of the string buffer.
   // Capacity remains the same. COLstring::size() is reduced by 'n'.
   // 'n' must be less than or equal to size(), but not negative.
   void shift(int n);

   // Shrink heap use to CapacityHint if possible without altering the string.
   // Note: actual capacity() after this call will not necessarily match CapacityHint.
   // Note: Do not use old COLstring::data() and COLstring::c_str() values after this call.
   void shrink(int CapacityHint = 0);

   // swap strings and capacities
   void swap(COLstring* pString);

   // Return substring from `begin' position to the end of the string.
   COLstring substr(int begin) const;

   // Return substring from `begin' position max of `length' characters.
   // NOTE: length can be `npos', in which case return substr(begin).
   // For future code, do not rely on this "length could be npos" trick.
   COLstring substr(int begin, int length) const;

   COLstring& toUpperCase();  // Modifies string: converts COLstring to uppercase via stdlib toupper().
   COLstring& toLowerCase();  // Modifies string: converts COLstring to lowercase via stdlib tolower().

   COLstring& append(int AppendCharRepeatCount, char AppendChar);       // binary safe
   COLstring& append(const COLstring& AppendString);                    // binary safe
   COLstring& append(const char* AppendString, int AppendStringLength); // binary safe
   COLstring& append(const char* AppendString);

   // true if the COLstring is only whitespace (spaces, tabs, carriage returns, or newlines), false otherwise
   bool isWhitespace() const;

   // Splits the COLstring using the token provided as the delimiter and returns the values in the provided strings. 
   // rpslit uses the last occurance of the token in the string  as the split point.
   // If no token is found rpslit returns the input as the second argument and returns false.
   // eg if the string is "path/file1/file2", with token "/" then
   // Out1 = path/file1, Out2 = file2
   bool rsplit(COLstring& Out1, COLstring& Out2, const char* Token) const;
  
   // Splits the COLstring using the token provided as the delimiter and returns the values in the provided strings. 
   // split uses the first occurrence of the token in the string as the split point.
   // If no token is found split returns the input as the first argument and returns false.
   // eg if the string is "path/file1/file2", with token "/" then
   // Out1 = path,  Out2 = file1/file2
   bool split(COLstring& Out1, COLstring& Out2, const char* Token) const;

   // Splits the COLstring at the specified index, and returns the values in the provided strings. 
   // The character at index SplitIndex is the first character of Out2.
   // If SplitIndex is greater than the length of the COLstring object, splitAtIndex returns the
   // input as the first argument, and the second argument is empty.
   //   eg. if the string is "Physician, heal thyself." and Split Index is 11, then 
   // Out1 = "Physician, ",  Out2 = "heal thyself."
   void splitAtIndex(COLstring& Out1, COLstring& Out2, int SplitIndex) const;

   // Returns a pointer to the actual char buffer used by the COLstring
   const char* get_buffer() const;
   char*       get_buffer();

   // Sets the string to be of length Length and filled with the specified FillChar character.
   // Returns the COLstring instance after assignment.
   COLstring& assign(int Length, char FillChar);

   // Does a forward search of the COLstring for the specified character starting at the specified Position.
   // If the character is found then the position (starting index) of the character in the COLstring is returned.
   // If the character is not found npos is returned.
   // DEPRECATED: See bool find(int* FoundPosition, const char Character, size_t Position = 0) const
   size_t find(const char Character, size_t Position = 0) const;

   bool find(int* FoundPosition, const char Character, size_t Position = 0) const;

   // Does a forward search of the COLstring for the specified Pattern.
   // If the Pattern is found then the position (starting index) of the Pattern in the COLstring is returned.
   // If the Pattern is not found npos is returned.
   // DEPRECATED: See bool find(int* FoundPosition, const char* Pattern) const
   // DEPRECATED: See bool find(int* FoundPosition, const COLstring& Pattern) const
   size_t find(const char*      Pattern) const;
   size_t find(const COLstring& Pattern) const;

   bool find(int* FoundPosition, const char*      Pattern) const;
   bool find(int* FoundPosition, const COLstring& Pattern) const;

   // Does a forward search of the COLstring for the specified Pattern string.
   // If the Pattern string is found then the position (starting index) of the Pattern in the COLstring is returned.
   // If the Pattern string is not found npos is returned.
   // DEPRECATED: See bool find(int* FoundPosition, const char* Pattern, size_t Position) const
   // DEPRECATED: See bool find(int* FoundPosition, const COLstring& Pattern, size_t Position) const
   size_t find(const char*      Pattern, size_t Position) const;
   size_t find(const COLstring& Pattern, size_t Position) const;

   bool find(int* FoundPosition, const COLstring& Pattern, size_t Position) const;
   bool find(int* FoundPosition, const char*      Pattern, size_t Position) const;

   // Does a reverse search of the COLstring for the specified pattern.
   // If the pattern is found the position (starting index) of the Pattern in the COLstring is returned.
   // If the Pattern is not found npos is returned.
   // DEPRECATED: See bool rfind(int* FoundPosition, const COLstring& Pattern) const
   size_t rfind(const COLstring& Pattern) const;

   bool rfind(int* FoundPosition, const COLstring& Pattern) const;

   // This method does not likely do what you think it does.
   // It does a reverse search of the COLstring for the specified pattern starting from the specified Position. 
   // Position represents the *highest* index into this COLstring that a byte comparison against the Pattern
   // will begin and Position will decrement towards 0 until a matching string is found or the beginning of
   // this COLstring is reached.
   // For example: COLstring("0123456789").rfind("3456",4) will return 3 even though the matched Pattern
   // string length goes above/beyond Position.
   // If the Pattern is found the position (starting index) of the Pattern in the COLstring is returned.
   // If the Pattern is not found npos is returned.
   // DEPRECATED: See bool rfind(int* FoundPosition, const COLstring& Pattern, size_t Position) const
   size_t rfind(const COLstring& Pattern, size_t Position) const;

   bool rfind(int* FoundPosition, const COLstring& Pattern, size_t Position) const;

   // Does a reverse search from the specified Position and returns the the position (index) of the first
   // occurrence of ANY of the specified Characters. If no match is found npos is returned.
   // DEPRECATED: see bool find_last_of(int* FoundPosition, const COLstring& Characters, size_t Position) const
   size_t find_last_of(const COLstring& Characters, size_t Position = npos) const;

   bool find_last_of(int* FoundPosition, const COLstring& Characters, size_t Position = npos) const;
   
   // Removes all occurrences of the specified character from the COLstring
   void stripAll(char c = ' ');

   // Returns a new COLstring with consecutive occurrences of the specified character removed removed
   // from the beginning, end, or both ends (depending  on the StripType) of the original COLstring.
   COLstring strip(StripType s = Trailing, char c = ' ') const;
   
   // Returns the result of stdlib strcmp of this COLstring and the specified COLstring
   inline int compare(const COLstring& s) const { return ::strcmp(c_str(), s.c_str()); }

   // Returns the result of stdlib strcmp of this COLstring and the specified C-string
   inline int compare(const char* s) const { return ::strcmp(getStringData(), s); }

   // All comparison operators ultimately call this static method. Calls stdlib strcmp.
   static inline int compare(const char* a, const char* b) { return ::strcmp(a, b); }
   
   // case sensitive equals
   bool equals(const COLstring& s) const;
   bool equals(const char*      s) const;

   // case insensitive equals
   bool equalsIgnoreCase(const COLstring& s) const;
   bool equalsIgnoreCase(const char*      s) const;

   // Case insensitive string compare similar to stricmp/strcasecmp.
   // Returns 0 for equal, negative if this is less than s, positive if this is greater than s.
   int icompare(const COLstring& s) const;
   int icompare(const char*      s) const;

   // Case insensitive string compare similar to stricmp/strcasecmp.
   static int icompare(const char* s1, const char* s2);

   // Removes the specified NumberOfCharacters from the string starting at the specified StartIndex. 
   // A reference to the modified string is also returned.
   COLstring& remove(size_t StartIndex, size_t NumberOfCharacters = npos);

   // Replaces all occurrences of the specified substring in the COLstring with the specified ReplacementString.
   // Case sensitive. Returns number of Substrings replaced.
   size_t replace(const COLstring& Substring, const COLstring& ReplacementString);

   // Replaces all occurrences of FromChar with ToChar.
   // This version of replace() that is much faster for char replacement.
   // Works correctly with binary data with embedded nil characters up to size().
   void replace(char FromChar, char ToChar);
   
   // Replaces the specified NumberOfCharacters starting at StartIndex with ReplacementString.
   // Optionally you may specify to use a substring of the ReplacementString.
   COLstring& replace(size_t StartIndex, size_t NumberOfCharacters, const COLstring& ReplacementString, size_t ReplacementStartIndex = 0, size_t ReplacementNumberOfCharacters = npos);
   
   // Prepends the COLstring s to this COLstring
   COLstring& prepend(const COLstring &s) { return insert(0, s.c_str()); }
   
   // Prepends the c-string s to this COLstring
   COLstring& prepend(const char* s) { return insert(0, s); }
   
   // Inserts the COLstring s in to this COLstring at Position
   COLstring& insert(int Position, const COLstring& s) { return insert(Position, s.c_str()); }
   
   // Inserts the c-string s in to this COLstring at Position
   COLstring& insert(int Position, const char* s);

   // Append binary or string data to COLstring. Returns SizeOfBuffer.
   // This was the COLsink interface implementation until we took COLsink out of COLstring.
   size_t write(const void* Buffer, COLuint32 SizeOfBuffer);

   // Sets the size of the COLstring and null terminates it.
   // Will throw an exception if NewSize is greater than or equal to the string capacity.
   // This is intended for use with operating system API calls which write directly to buffers.
   // Not for normal use - Eliot
   void setSize(int NewSize);

private:
   COLstring(const COLstring& s, size_t orig, size_t length);

   // first line in every COLstring ctor
   inline void init() {
       _length = 0;
       _capacity = sizeof(_u.internal);
       _u.internal[0] = 0;
   }

   inline bool isInternalString() const     { return _capacity <= sizeof(_u.internal); }

   inline char*       getStringData()       { return _capacity <= sizeof(_u.internal) ? _u.internal : _u.heap; }
   inline const char* getStringData() const { return _capacity <= sizeof(_u.internal) ? _u.internal : _u.heap; }

   size_t find_index(const char*, size_t start) const;
   size_t find_index(const char, size_t start) const;
   size_t rfind_index(const char*, size_t start) const;

   // Force a new length into string. NO capacity check. For internal use only.
   void overrideLength(int len);

private:
   union U {
       char* heap;         // is string data if _capacity > sizeof(internal)
       char  internal[16]; // is string data if _capacity <= sizeof(internal)
   };

   int _length;   // Length without trailing nil.
   int _capacity; // Storage available to string without reallocation.
   U   _u;        // String data - local or heap allocation depends on _capacity.
};

COLstring operator+(const char*      a, const COLstring& b);
COLstring operator+(const COLstring& a, const char*      b);
COLstring operator+(char             a, const COLstring& b);
COLstring operator+(const COLstring& a, char             b);
COLstring operator+(const COLstring& a, const COLstring& b);

// Comparison operators.
// Note: these comparisons are NOT binary compatible - will stop at first nil byte.
bool operator==(const COLstring& a, const COLstring& b);
bool operator==(const char*      a, const COLstring& b);
bool operator==(const COLstring& a, const char*      b);
bool operator!=(const COLstring& a, const COLstring& b);
bool operator!=(const char*      a, const COLstring& b);
bool operator!=(const COLstring& a, const char*      b);
bool operator<( const COLstring& a, const COLstring& b);
bool operator<( const char*      a, const COLstring& b);
bool operator<( const COLstring& a, const char*      b);
bool operator>( const COLstring& a, const COLstring& b);
bool operator>( const char*      a, const COLstring& b);
bool operator>( const COLstring& a, const char*      b);
bool operator>=(const COLstring& a, const COLstring& b);
bool operator>=(const char*      a, const COLstring& b);
bool operator>=(const COLstring& a, const char*      b);
bool operator<=(const COLstring& a, const COLstring& b);
bool operator<=(const char*      a, const COLstring& b);
bool operator<=(const COLstring& a, const char*      b);

// case conversion without altering original COLstring
COLstring COLlowerCase(const COLstring& String);
COLstring COLupperCase(const COLstring& String);

// convert signed and unsigned integers to strings
COLstring COLintToString(int);
COLstring COLint32ToString(int);
COLstring COLuint32ToString(COLuint32);
COLstring COLint64ToString(COLint64);
COLstring COLuint64ToString(COLuint64);

COLstring COLstripWhiteSpace(const COLstring& String, bool StripLeading = true, bool StripTrailing = true);
COLstring COLremoveAllWhiteSpace(const COLstring& String);
COLstring COLreplaceAllWhiteSpace(const COLstring& String, const char Replacement);
int COLreplaceAllWhiteSpaceCount(COLstring& String, const char Replacement);
COLstring COLstripChars(const COLstring& String, const COLstring& CharsToKill);
COLstring COLtoAscii(const COLstring& String, char Marker = 'x');

inline bool COLisspace(int c) { return c == 32 || (c >= 9 && c <= 13); } // see #22552
inline bool COLisdigit(int c) { return c >= 48 && c <= 57; }

bool COLstringHasPrefix(const COLstring& String, const COLstring& Prefix);
COLstring COLstringCutPrefix(const COLstring& String, const COLstring& Prefix);

inline static bool COL_is_npos(     int pos) { return npos == (size_t)(unsigned)pos; }
inline static bool COL_is_npos(unsigned pos) { return npos == (size_t)(unsigned)pos; }

bool COLstringContains(const COLstring& StringToSearch, const COLstring& Pattern);
bool COLstringContains(const char* String, const char* Pattern);

#endif // __COL_STRING_H__

