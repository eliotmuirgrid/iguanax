//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SECpathParser.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/12/24 10:34 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLarray.h>
#include <FIL/FILutils.h>
#include <SEC/SECpathParser.h>

#include <cctype>
COL_LOG_MODULE;

// Helper to check if a character is a valid path character
static bool SECisValidPathChar(const char c) {
   return std::isalnum(c) || c == '_' || c == '-' || c == '.' || c == '/' || c == '\\';
}

// Helper to extract path between quotes
static COLstring SECextractQuotedPath(const COLstring& Input, size_t& Pos) {
   COL_FUNCTION(SECextractQuotedPath);
   char QuoteChar = Input[Pos];
   Pos++;  // Skip opening quote

   COLstring Path;
   while(Pos < Input.size() && Input[Pos] != QuoteChar) {
      Path += Input[Pos];
      Pos++;
   }
   if(Pos < Input.size()) Pos++;  // Skip closing quote
   return Path;
}

COLarray<COLstring> SECpathParse(const COLstring& Command) {
   COL_FUNCTION(SECparsePaths);
   COLarray<COLstring> Paths;
   size_t               Pos = 0;
   while(Pos < Command.size()) {
      // Skip whitespace
      while(Pos < Command.size() && std::isspace(Command[Pos])) { Pos++; }

      if(Pos >= Command.size()) break;

      // Handle quoted paths
      if(Command[Pos] == '"' || Command[Pos] == '\'') {
         COLstring Path = SECextractQuotedPath(Command, Pos);
         if(!Path.is_null() && (Path.find('/') != npos || Path.find('\\') != npos)) { Paths.push_back(Path); }
         continue;
      }

      // Handle unquoted paths
      size_t Start            = Pos;
      bool   HasPathSeparator = false;

      // Check for Windows drive letter pattern
      bool HasDriveLetter = false;
      if(Pos + 1 < Command.size() && std::isalpha(Command[Pos]) && Command[Pos + 1] == ':') {
         HasDriveLetter = true;
         Pos += 2;
      }

      // Collect path characters
      while(Pos < Command.size() && SECisValidPathChar(Command[Pos])) {
         if(Command[Pos] == '/' || Command[Pos] == '\\') { HasPathSeparator = true; }
         Pos++;
      }

      // Only add if it's a valid path (has separator or drive letter)
      if(Pos > Start && (HasPathSeparator || HasDriveLetter)) { Paths.push_back(Command.substr(Start, Pos - Start)); }
      else if(Pos == Start) { Pos++; }
   }
   COL_VAR(Paths.size());
   return Paths;
}