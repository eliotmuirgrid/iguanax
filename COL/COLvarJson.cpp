// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLvarJson
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 26 January 2025 - 02:13PM
// ---------------------------------------------------------------------------

#include "COLvar.h"
#include "COLstring.h"

#include <stdio.h>
#include <ctype.h>

#include "COLlog.h"
COL_LOG_MODULE;

class COLjsonParseState{
public:
   COLstring ErrorMessage;
};

// Forward declarations of parsing functions
static bool COLparseObject (const char*& current, COLvar& value, COLjsonParseState* pState);
static bool COLparseArray  (const char*& current, COLvar& value, COLjsonParseState* pState);
static bool COLparseString (const char*& current, COLvar& value, COLjsonParseState* pState);
static bool COLparseNumber (const char*& current, COLvar& value, COLjsonParseState* pState);
static bool COLparseBoolean(const char*& current, COLvar& value, COLjsonParseState* pState);
static bool COLparseNull   (const char*& current, COLvar& value, COLjsonParseState* pState);
static bool COLparseValue  (const char*& current, COLvar& value, COLjsonParseState* pState);

static void COLskipWhitespace(const char*& current);
static bool COLpopulateError(COLvar* pInfo, const COLstring& Error, int Position);

bool COLparseJson(const char* pData, COLvar* pOutput);

bool COLparseJson(const COLstring& Data, COLvar* pOutput){
   COL_FUNCTION(COLparseJson-String);
   return COLparseJson(Data.c_str(), pOutput);
}

// Entry function to parse JSON
bool COLparseJson(const char* pData, COLvar* pOutput) {
   COL_FUNCTION(COLparseJson);
   COLjsonParseState State;
   const char* current = pData;
   COLskipWhitespace(current);
   if (*current == 0)                             { return COLpopulateError(pOutput, "Empty string", 0);                   }
   if (!COLparseValue(current, *pOutput, &State)) { return COLpopulateError(pOutput, State.ErrorMessage, current - pData); }
   COLskipWhitespace(current);  
   if (*current != '\0'){
      return COLpopulateError(pOutput, "Unexpected trailing content.", current-pData);
   } 
   return true;
}

static bool COLpopulateError(COLvar* pInfo, const COLstring& Error, int Position){
   COL_FUNCTION(COLpopulateError);
   pInfo->reset();
   (*pInfo) = "Error parsing JSON: " + Error + " Position: " + COLintToString(Position);
   //(*pInfo)["position"] = Position;
   return false;
}

// Determine the type of JSON data at the current position and parse it
static bool COLparseValue(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COLskipWhitespace(current);
   switch (*current) {
   case '{' :        return COLparseObject (current,output,pState);
   case '[' :        return COLparseArray  (current,output,pState);
   case '\'':
   case '"' :        return COLparseString (current,output,pState);
   case 't' :
   case 'f' :        return COLparseBoolean(current,output,pState);
   case 'n' :        return COLparseNull   (current,output,pState);
   case '-':
   case '0': case '1': case '2': case '3': case '4':
   case '5': case '6': case '7': case '8': case '9':
                     return COLparseNumber(current, output, pState);
   default: 
      COL_ERR("Unexpected character in JSON");
      pState->ErrorMessage = COLstring("Unexpected character '") + *current + "'"; 
      return false;
   }
}

// Utility function to skip whitespace in the JSON input
static void COLskipWhitespace(const char*& i) {
    while (*i == ' ' || *i == '\n' || *i == '\r' || *i == '\t') { ++i;}
}

// Placeholder implementations for specific parsing functions
static bool COLparseObject(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseObject);
   current++;  // Skip the '{'
   output.setMapType();  // Set the type to Map (or Object)
   while (*current) {
      COLskipWhitespace(current);
      if (*current == '}') {  // Check for end of object
         current++;  // Skip the '}'
         return true;
      }
      // Parse the key (must be a string)
      COLvar key;
      if (!COLparseString(current, key, pState)) {
         return false;  // Failed to parse the key
      }
      COLskipWhitespace(current);
      // Expecting colon after the key
      if (*current != ':') { 
         pState->ErrorMessage = "Missing colon after key";
         return false;  
      }
      current++;  // Skip the colon
      COLskipWhitespace(current);
      if (!COLparseValue(current, output[key.asString()], pState)) {
         return false;  // Failed to parse the value
      }
      COLskipWhitespace(current);
      if (*current == ',') {
         current++;  // Skip the comma and continue
      } else if (*current != '}') {
         return false;  // Missing comma or end of object brace
      }
   }
   return false;  // Didn't find the closing '}'
}

static bool COLparseArray(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseArray);
   if (*current != '[') { return false; }
   current++;  // Skip the '['
   output.setArrayType();  // Set the type to Array
   while (*current) {
      COLskipWhitespace(current);
      if (*current == ']') {  // Check for end of array
         current++;  // Skip the ']'
         return true;
      }
      COLvar& Item = output.push_back();
      if (!COLparseValue(current, Item, pState)) {
         return false;  // Failed to parse the item
      }
      COLskipWhitespace(current);
      if (*current == ',') {
         current++;  // Skip the comma
      } else if (*current != ']') {
         pState->ErrorMessage = "Missing comma or end of array bracket";
         return false;  
      }
   }
   pState->ErrorMessage = "Unable to find closing ]";
   return false; 
}

// Converts a single hexadecimal character to its decimal value
static bool COLhexCharToValue(char c, unsigned int& value) {
   COL_VAR(c);
   if (c >= '0' && c <= '9') { value = c - '0'; } else 
   if (c >= 'A' && c <= 'F') { value = 10 + (c - 'A'); } else 
   if (c >= 'a' && c <= 'f') { value = 10 + (c - 'a'); } else {
     return false; // Non-hexadecimal character
   }
   COL_VAR(value);
   return true;
}

// Converts a Unicode code point to a UTF-8 encoded string
static bool COLunicodeToUtf8(unsigned int codepoint, COLstring& output) {
   if (codepoint < 0x80) { 
      output += static_cast<char>(codepoint); } else 
   if (codepoint < 0x800) {
      output += static_cast<char>(192 + (codepoint / 64)); 
      output += static_cast<char>(128 + (codepoint % 64)); } else 
   if (codepoint < 0x10000) {
      output += static_cast<char>(224 + (codepoint / 4096));
      output += static_cast<char>(128 + ((codepoint / 64) % 64));
      output += static_cast<char>(128 + (codepoint % 64));} else 
   if (codepoint < 0x110000) {
      output += static_cast<char>(240 +  (codepoint / 262144));
      output += static_cast<char>(128 + ((codepoint / 4096) % 64));
      output += static_cast<char>(128 + ((codepoint / 64) % 64));
      output += static_cast<char>(128 + (codepoint % 64));
   }
   return true;
}

// Parses a Unicode escape sequence from a JSON string
static bool COLparseUnicode(const char*& current, COLstring& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseUnicode);
   unsigned int codepoint = 0;
   for (int i = 0; i < 4; i++) {
      unsigned int digitValue = 0;
      if (!COLhexCharToValue(*current++, digitValue)) {
         pState->ErrorMessage = "Invalid hexadecimal digit";
         return false;  
      }
      codepoint = (codepoint << 4) | digitValue;
   }
   COL_VAR(codepoint);
   return COLunicodeToUtf8(codepoint, output);
}

static bool COLparseString(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseString);
   char quoteType = *current;
   current++; // Skip the opening quote
   COLstring result;
   while (*current != quoteType && *current != '\0') {
      if (*current == '\\') { // Handle escape sequences
         current++; // Move past the backslash
         switch (*current) {
            case '"':  result += '"'; break;
            case '\\': result += '\\'; break;
            case '/':  result += '/'; break;
            case 'b':  result += '\b'; break;
            case 'f':  result += '\f'; break;
            case 'n':  result += '\n'; break;
            case 'r':  result += '\r'; break;
            case 't':  result += '\t'; break;
            case 'u': { // Unicode sequence (e.g., \u0123)
               current+=1;
               if (!COLparseUnicode(current, result, pState)){
                  return false;
               }
               current--;
               break;
            }
            default:
               COLstring BadSequence = current;
               BadSequence = BadSequence.substr(0, 1);
               COLstring Snippet = current;
               if (Snippet.size() > 30) { Snippet = Snippet.substr(0, 30); }
               pState->ErrorMessage = COL_T("Invalid escape sequence \\") + BadSequence + COL_T(" near \\") + Snippet;
               return false; // Invalid escape sequence
         }
      } else {
         result += *current;
      }
      current++;
   }
   if (*current != quoteType) {
      pState->ErrorMessage = "Missing closing quote";
      return false;
   }
   current++; // Skip the closing quote
   output = result; // Set the output COLvar to the parsed string
   return true;
}

static bool COLparseNumber(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseNumber);
   COLstring numberStr;
   bool hasFraction = false;
   bool hasExponent = false;
   if (*current == '-') { numberStr += *current++;}
   // Parse the integer part
   while (isdigit(*current)) { numberStr += *current++;}
   // Parse the fractional part
   if (*current == '.') {
      hasFraction = true;
      numberStr += *current++;
      while (isdigit(*current)) {numberStr += *current++;}
   }
   if (*current == 'e' || *current == 'E') {
      hasExponent = true;
      numberStr += *current++;
      if (*current == '+' || *current == '-') { numberStr += *current++; }
      while (isdigit(*current)) { numberStr += *current++;}
   }
   COL_TRC("Convert the COLstring to a numerical type using C standard library functions");
   char* endPtr;
   if (hasFraction || hasExponent) {
      double number = strtod(numberStr.c_str(), &endPtr);
      COL_VAR(number);
      if (*endPtr == '\0') { // Ensure entire string was valid
         output = number;
      } else {
         pState->ErrorMessage = "Unable to parse double";
         return false; // Parsing error occurred
      }
   } else {
      long long number = strtoll(numberStr.c_str(), &endPtr, 10);
      COL_VAR(number);
      if (*endPtr == '\0') { // Ensure entire string was valid
         output = number;
      } else {
         pState->ErrorMessage = "Unable to parse integer";
         return false; // Parsing error occurred
      }
   }
   return true;
}

static bool COLparseBoolean(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseBoolean);
   static const COLstring trueLiteral = "true";
   static const COLstring falseLiteral = "false";
   if (strncmp(current, trueLiteral.c_str(), trueLiteral.size()) == 0 && !isalnum(current[trueLiteral.size()])) {
      output = true;
      current += trueLiteral.size(); // Move past the 'true' literal
      return true;
   } else if (strncmp(current, falseLiteral.c_str(), falseLiteral.size()) == 0 && !isalnum(current[falseLiteral.size()])) {
      output = false;
      current += falseLiteral.size(); // Move past the 'false' literal
      return true;
   }
   pState->ErrorMessage = "Not a valid boolean";
   return false;
}

static bool COLparseNull(const char*& current, COLvar& output, COLjsonParseState* pState) {
   COL_FUNCTION(COLparseNull);
   static const COLstring nullLiteral = "null";
   if (strncmp(current, nullLiteral.c_str(), nullLiteral.size()) == 0 && !isalnum(current[nullLiteral.size()])) {
      output.reset(); // Set COLvar to null or clear it
      current += nullLiteral.size(); // Move past the 'null' literal
      return true;
   }
   pState->ErrorMessage = "Not a valid null";
   return false; // Not a valid null
}
