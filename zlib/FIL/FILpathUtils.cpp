//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILpathUtils
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Monday, April 6th, 2009 @ 02:53:09 PM
//
//---------------------------------------------------------------------------
#include "FILprecomp.h"
#pragma hdrstop

#include "FILpathUtils.h"
#include "FILutils.h"

#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <COL/COLarray.h>

COL_LOG_MODULE;

const char* FIL_PATH_SEP_POSIX = "/";
const char* FIL_PATH_SEP_WINDOWS = "\\";

#define MAX_NESTED_DEPTH 256
#define NESTED_DEPTH_ERROR "Error: Invalid path (too many nested directories or excessive parent directory references)."

COLstring FILpathVolume(const COLstring& Path);

static bool isThisAlpha(const char C) {
   return ( (0x41 <= C && C <= 0x5a) || (0x61 <= C && C <= 0x7A) );
}

static bool checkDirDepthLimit(int DirectoryDepth){
   // COL_FUNCTION(checkDirDepthLimit);
   if (DirectoryDepth >= MAX_NESTED_DEPTH){ 
      COL_ERR("Directory depth exceeds 256");
      return false;
   } else if (DirectoryDepth < 0){
      COL_ERR("Directory depth below 0");
      return false;
   }
   return true;
}

bool FILpathSimplify(const COLstring& Path, COLstring* pOut, COLstring* pErr){
   COL_FUNCTION(FILpathSimplify);
   try {
      *pOut = FILpathSimplifyUnsafe(Path);
   } catch (const COLerror& Err){
      *pErr = Err.description();
      return false;
   }
   return true;
}

COLstring FILpathSimplifyUnsafe(const COLstring& Path) {
   COL_FUNCTION(FILpathSimplifyUnsafe);
   COL_VAR(Path);
   // This function will attempt to 'simplify' a path via the following methods:
   //   * Stripping out any './' which exist
   //   * Removing any '../' which are resolveable
   //   * Removing any extraneous slashes (i.e. 'foo//bar')
   //
   // The code below does this in a single path over the string. It keeps a 
   // list of places where directories have started, which is used when jumping
   // up on '..'. This currently has a hard-coded limit of 256 directories, but 
   // this (hopefully) shouldn't be a problem. After IX-4181, this function will
   // throw an error in such cases to prevent SIGABRT crashes.
   //
   // The algorithm itself is pretty much what would happen if you did it by
   // hand. It works its way through the string, and instead of using substring
   // to cut out extraneous directories, it just moves the write pointer back 
   // and overwrites them. 

   const char* pInput = Path.c_str();

   // This holds our output. It must be big enough for us to have the entire 
   // Path in it, and pre-allocating means we can pretty much treat it as
   // a big block of data.
   COLstring Buffer;
   Buffer.setCapacity(Path.size() + 1);

   char* pOutputStart = Buffer.get_buffer();

   // This is our actual write pointer, which we move around as we pass over
   // the input.
   char* pOutput = pOutputStart;

   // Our stack of directories, which are just pointers within the output
   // Buffer.
   char* pDirectoryOffsets[MAX_NESTED_DEPTH];

   // How many directories are in the directory stack.
   int DirectoryDepth = 0;

   // The separator we wish to use. This code will use the 'proper' separators
   // when it identifies a Windows, *NIX or Samba absolute path, otherwise it 
   // uses the system default.
   char Separator = FIL_DIR_SEPARATOR[0];

   enum {
      ST_START,  // Beginning of a directory name, we're at the first char after the slash.
      ST_DATA,   // Any old directory name content which isn't something we specifically care about.
      ST_DOT,    // We've seen a single . at the start of a dir name so far
      ST_DOTDOT  // Two dots!
   } State = ST_START;

   // Figure out what kind of path we're dealing with here
   if (Path.size() >= 1 && 
       *pInput == '/')
   {
      COL_TRC("POSIX style absolute path.");
      // POSIX-style absolute path
      Separator = '/';
      *pOutput++ = *pInput++;
   }
   else if (Path.size() >= 2 &&
            ::memcmp(pInput, "\\\\", 2) == 0)
   {
      COL_TRC("Samba style absolute path.");
      // Samba-style absolute path
      Separator = '\\';
      ::memcpy(pOutput, pInput, 2);
      pOutput += 2;
      pInput += 2;
   }
   else if (Path.size() >= 3 && 
            isThisAlpha(pInput[0]) &&
            pInput[1]==':' &&
            (pInput[2]=='/' || pInput[2]=='\\')
         )
   {
      COL_TRC("Windows style absolute path.");
      // Windows-style absolute path
      Separator = '\\';
      pOutput[0] = pInput[0];
      pOutput[1] = pInput[1];
      pOutput[2] = Separator;
      //::memcpy(pOutput, pInput, 3);
      pOutput += 3;
      pInput += 3;
   }
   else
   {
      COL_TRC("Relative path.");
      // Relative path of some sort, we use the current platform's default
      // separator.
   }

   // Set up where our 'root' directory is.
   pDirectoryOffsets[DirectoryDepth++] = pOutput;

   while (*pInput)
   {
      // LastChar is the value we've seen. We move the pointers ahead before
      // we look at its value for non-separator cases, so it's more sane to 
      // have it in a variable.
      char LastChar = *pInput;
      switch (LastChar)
      {
         case '\\':
         case '/':
            LastChar = Separator;
            break;
         default:
            break;
      }

      pInput++;
      *pOutput++ = LastChar;

      switch (State)
      {
         case ST_START:
            switch (LastChar)
            {
               case '.':
                  State = ST_DOT;
                  break;
               case '\\':
               case '/':
                  // Since we're at the start of a directory name, this means 
                  // that we have multiple slashes, which are redundant and can
                  // be stripped out.
                  pOutput--;
                  State = ST_START;
                  break;
               default:
                  State = ST_DATA;
                  break;
            }
            break;
         case ST_DATA:
            switch (LastChar)
            {
               case '\\':
               case '/':
                  // We've got a non-special directory name, so we just keep
                  // going after adding our write position to the stack.
                  State = ST_START;
                  if (!checkDirDepthLimit(DirectoryDepth)) { throw COLerror(NESTED_DEPTH_ERROR); }  // IX-4181 
                  pDirectoryOffsets[DirectoryDepth++] = pOutput;
                  break;
               default:
                  break;
            }
            break;
         case ST_DOT:
            switch (LastChar)
            {
               case '\\':
               case '/':
                  // We've seen a reference to the current directory, so we 
                  // rewind to the character after the previous slash.
                  if (!checkDirDepthLimit(DirectoryDepth)) { throw COLerror(NESTED_DEPTH_ERROR); } // IX-4181
                  pOutput = pDirectoryOffsets[DirectoryDepth - 1];
                  State = ST_START;
                  break;
               case '.':
                  State = ST_DOTDOT;
                  break;
               default:
                  State = ST_DATA;
                  break;
            }
            break;
         case ST_DOTDOT:
            switch (LastChar)
            {
               case '\\':
               case '/':
                  // We've seen a reference to the parent directory. Make sure
                  // that we have enough directories to be able to resolve the
                  // parent and back ourselves up.
                  if (DirectoryDepth >= 2)
                  {
                     pOutput = pDirectoryOffsets[(--DirectoryDepth) - 1];
                  }
                  State = ST_START;
                  break;
               default:
                  State = ST_DATA;
                  break;
            }
            break;
      }
   }
   
   // Make sure we handle a .. or . at the end of a string without a trailing
   // slash correctly.
   switch (State)
   {
      case ST_DOT:
         if (!checkDirDepthLimit(DirectoryDepth)) { throw COLerror(NESTED_DEPTH_ERROR); } // IX-4181
         pOutput = pDirectoryOffsets[--DirectoryDepth];
         break;
      case ST_DOTDOT:
         if (DirectoryDepth >= 2)
         {
            pOutput = pDirectoryOffsets[(--DirectoryDepth) - 1];
         }
         break;
      default:;
   }

   // Not NULL-terminated, use the special constructor.
   COLstring SimplePath(pOutputStart, pOutput - pOutputStart);
   COL_VAR(SimplePath);
   return SimplePath;
   //return COLstring(pOutputStart, pOutput - pOutputStart);
}

COLstring FILpathUnCruft(const COLstring& Path) {
   COL_FUNCTION(FILpathUnCruft);
   COL_VAR(Path);

   const char* pInput = Path.c_str();
   COLstring Buffer;
   Buffer.setCapacity(Path.size() + 1);
   char* pOutputStart = Buffer.get_buffer();
   char* pOutput = pOutputStart;
   char Separator = FIL_DIR_SEPARATOR[0];

   enum {
      ST_DIRTY,  // We've just seen a crufty character. We start in this state, to remove cruft from the beginning.
      ST_DATA
   } State = ST_DIRTY;

   while (*pInput) {

      // LastChar is the value we've seen. We move the pointers ahead before
      // we look at its value for non-separator cases, so it's more sane to 
      // have it in a variable.
      char LastChar = *pInput;

      pInput++;
      *pOutput++ = LastChar;

      switch (State)
      {
      case ST_DIRTY:
         switch (LastChar)
         {
         case '\\':
         case '/':
         case '.':
         case '<': 
         case '>':
         case ':':
         case '|':
         case '?':
         case '"':
         case '*':
            pOutput--;
            State = ST_DIRTY;
            break;
         default:
            State = ST_DATA;
            break;
         }
         break;
      case ST_DATA:
         switch (LastChar)
         {
         case '\\':
         case '/':
         case '.':
            State = ST_DIRTY;
            break;
         case '<': 
         case '>':
         case ':':
         case '|':
         case '?':
         case '"':
         case '*':
            pOutput--;
            State = ST_DIRTY;
         default:
            break;
         }
         break;
      }
   }
   // Not NULL-terminated, use the special constructor.
   COLstring SimplePath(pOutputStart, pOutput - pOutputStart);
   COL_VAR(SimplePath);
   return SimplePath;
}

COLstring FILpathResolve(const COLstring& Path, COLstring WorkingDirectory) {
   COL_FUNCTION(FILpathResolve);
   COL_VAR2(Path, WorkingDirectory);
   if (FILpathIsAbsolute(Path)) {
#ifdef _WIN32
      COL_TRC("In Windows.");
      COLstring PathVolume = FILpathVolume(Path);
      if(PathVolume.is_null()){
         COL_TRC("Windows: PathVolume is null.");
         // path is absolute, yet has no volume (presumably a slash-rooted path)
         // on Windows, resolving is normally expected to produce the volume
         // try to attach one from WorkingDirectory
         COLstring WorkingVolume = FILpathVolume(WorkingDirectory);
         if(!WorkingVolume.is_null()){
            COL_TRC("Windows: Working volume is null.");
            return FILpathAppend(WorkingVolume, Path);
         }
      }
      COL_TRC("End Windows specific path resolution");
#endif
      COLstring Out, OutErr;
      FILpathSimplify(Path, &Out, &OutErr);
      return Out;
   } if (WorkingDirectory.is_null()) {
      WorkingDirectory = FILworkingDir();
   }
   COL_VAR2(Path, WorkingDirectory);
   return FILpathAppend(WorkingDirectory, Path);
}

COLstring FILpathAppend(const COLstring& PathA, const COLstring& PathB) {
   COL_FUNCTION(FILpathAppend);
   COL_VAR2(PathA, PathB);
   if (PathA.is_null()) {
      COLstring Out, Err; 
      if (!FILpathSimplify(PathB, &Out, &Err)) { 
         COL_ERR(Err);
         return PathB;
      }
      return Out;
   }
   COLstring Result(PathA);
   FILaddPathSeparator(Result);
   Result.append(PathB);
   COLstring Out, Err;
   if (!FILpathSimplify(Result, &Out, &Err)) {
      COL_ERR(Err);
      return Result;
   }
   return Out;
}

bool FILpathIsAbsolute(const COLstring& Path) {
   COL_FUNCTION(FILpathIsAbsolute);
   const char* pPath = Path.c_str();

   if(Path.size()==2 && isThisAlpha(pPath[0]) && pPath[1]==':') {
      return true;
   }

   if (Path.size() >= 3 && 
       isThisAlpha(pPath[0]) &&
       ':' == pPath[1] && 
       ('\\' == pPath[2] || '/' == pPath[2])
       )
   {
      // e.g. "C:\"
      return true;
   }
   
   if (Path.size() >= 3 && 
            pPath[0] == '\\' &&
            pPath[1] == '\\')
   {
      // UNC - \\something
      return true;
   }

   if (Path.size() >= 1 && 
            (pPath[0] == '/' || pPath[0]==*FIL_PATH_SEP_NATIVE))
   {
      // begins with slash
      return true;
   }

   return false;
}

COLstring FILpathExtLast(const COLstring& FileName, bool IncludeDot) {
   COL_FUNCTION(FILpathExtLast);
   COLstring FileNameOnly = FILpathName(FileName);
   COLindex DotIndex = FileNameOnly.rfind(".");
   if(DotIndex == npos) {
      return ""; // no extension = empty string
   }
   if(IncludeDot) {
      // .extension 
      return FileNameOnly.substr(DotIndex);
   }
   // extension without the dot
   return FileNameOnly.substr(DotIndex+1);
}

COLstring FILpathNameWithoutLastExt(const COLstring& Path){
   COL_FUNCTION(FILpathNameWithoutLastExt);
   COLstring FileNameOnly = FILpathName(Path);
   COLindex DotIndex = FileNameOnly.rfind(".");
   if(DotIndex == npos)
   {
      return FileNameOnly; // no dot - return entire filename
   }
   // got dot - return everything up to, but not including, the last dot
   return FileNameOnly.substr(0, DotIndex);
};

COLstring FILpathName(const COLstring& Path) {
   COL_FUNCTION(FILpathName);
   const char* pBegin = Path.c_str();
   const char* pEnd = pBegin + Path.size();
   const char* pRead = pEnd;

   while (--pRead >= pBegin)
   {
      switch (*pRead)
      {
         case '\\':
         case '/':
            pRead++;
            return COLstring(pRead, pEnd - pRead);
      }
   }
   return Path;
}

COLstring FILpathDir(const COLstring& Path, bool Simplify) {
   COL_FUNCTION(FILpathDir);
   COL_VAR2(Path, Simplify);
   const char* pBegin = Path.c_str();
   const char* pEnd = pBegin + Path.size();
   const char* pRead = pEnd;

   while (--pRead >= pBegin)
   {
      switch (*pRead)
      {
         case '\\':
         case '/':
            pRead++;
            if(!Simplify)
            {
               return COLstring(pBegin, pRead - pBegin);
            }
            COLstring Temp(pBegin, pRead - pBegin);
            COLstring Out, OutErr;
            FILpathSimplify(Temp, &Out, &OutErr);
            return Out;
      }
   }
   return COLstring();
}

COLstring FILpathDirResolved(const COLstring& Path, COLstring WorkingDirectory) {
   COL_FUNCTION(FILpathDirResolved);
   return FILpathDir(FILpathResolve(Path, WorkingDirectory));
}

// extract volume from a path, as is
// there may well not be one, for a relative or slash-rooted one - in which case, empty string is returned
COLstring FILpathVolume(const COLstring& Path) {
   COL_FUNCTION(FILpathVolume);
   const char* pBegin = Path.c_str();
   const char* pEnd = pBegin + Path.size();
   const char* pRead = pBegin - 1;

   if (Path.size() < 2) {
      return COLstring();
   }

   if (Path.size()==2 && isThisAlpha(pBegin[0]) && pBegin[1]==':') {
      return Path + FIL_PATH_SEP_WINDOWS;
   }
   if (pBegin[1] == ':' &&
            (pBegin[2] == '\\' || pBegin[2]=='/') &&
            isThisAlpha(pBegin[0])) {
      return COLstring(pBegin, 3);
   } 
   if (pBegin[0] != '\\' && 
            pBegin[1] != '\\') {
      return COLstring();
   }

   pRead += 2;

   bool SeenFirstSlash = false;
   while (++pRead < pEnd)
   {
      switch (*pRead)
      {
         case '\\':
            if (SeenFirstSlash)
            {
               return COLstring(pBegin, pRead - pBegin + 1);
            }
            SeenFirstSlash = true;
            break;
      }
   }
   return COLstring();
}

// obtain volume for a path, resolving if necessary
COLstring FILpathVolumeResolved(const COLstring& Path, COLstring WorkingDirectory) {
   COL_FUNCTION(FILpathVolumeResolved);
   return FILpathVolume(FILpathResolve(Path, WorkingDirectory));
}

COLstring FILpathResolveEnvironment(const COLstring& Original) {
   COL_FUNCTION(FILpathResolveEnvironment);
   //Don't expand an empty string (some error messages will complain about passing a directory, 
   //when the error should be that the value is empty).

   if(!Original.is_null()){

      COLstring Resolved;
      COLstring Last = Original;

      //Expand leading ~ and/or a single pass of environment variables if present.
      Resolved = FILexpand(Original);
   
      //In case there are multiple passes of environment variables.
      while(Resolved != Last){
         Last = Resolved;
         Resolved = COLexpandEnvironmentVariables(Resolved);
      }
      
      //Expand relative path if present.
      Resolved = FILpathResolve(Resolved);
      
      return Resolved;
   }

   return Original;

}

void FILpathParentDirList(COLarray<COLstring>& DirList, const COLstring& FilePath) {
   COL_FUNCTION(FILpathParentDirList);
   COLstring Dir = FILpathSepRemove(FILpathDir(FilePath,false));
   COLstring Volume;
   if(FILpathIsAbsolute(FilePath)){
      Volume = FILpathSepRemove(FILpathVolumeResolved(FilePath));
   }
   if(!Dir.is_null() && Dir != Volume)
   {
      FILpathParentDirList(DirList, Dir);
      DirList.push_back(Dir);
   }
}

COLstring FILpathSepRemove(const COLstring& Path, bool RemoveDotsToo) {
   COL_FUNCTION(FILpathSepRemove);
   COLstring Tmp(Path);
   if (RemoveDotsToo) {
      while(Tmp.size() && (Tmp[Tmp.size()-1]=='/' || Tmp[Tmp.size()-1]=='\\' || Tmp[Tmp.size()-1]=='.') ) {
         Tmp = Tmp.substr(0, Tmp.size()-1);
      }
   } else {
      while(Tmp.size() && (Tmp[Tmp.size()-1]=='/' || Tmp[Tmp.size()-1]=='\\') ) {
         Tmp = Tmp.substr(0, Tmp.size()-1);
      }
   }
   return Tmp;
}

COLstring FILpathSepAdd(const COLstring& Path, const char* Sep) {
   COL_FUNCTION(FILpathSepAdd);
   COLMSGPRECONDITION(Sep, "Separator can't be NULL");
   return FILpathSepRemove(Path) + Sep;
}

COLstring FILpathSepToPosix(const COLstring& Path) {
   COL_FUNCTION(FILpathSepToPosix);
   COLstring Result = Path;
   Result.replace(FIL_PATH_SEP_WINDOWS, FIL_PATH_SEP_POSIX);
   return Result;
}

COLstring FILpathSepToNative(const COLstring& Path) {
   COL_FUNCTION(FILpathSepToNative);
   COLstring Result = Path;
#ifdef _WIN32
   Result.replace(FIL_PATH_SEP_POSIX, FIL_PATH_SEP_WINDOWS);
#else
   Result.replace(FIL_PATH_SEP_WINDOWS, FIL_PATH_SEP_POSIX);
#endif
   return Result;
}

COLstring FILexpand(const COLstring& OriginalPath){
   COL_FUNCTION(FILexpand);
   COLstring Path = COLexpandEnvironmentVariables(OriginalPath);
   if (Path == "~" || (Path.size() >= 2 && Path[0] == '~' && (Path[1] == '/' || Path[1] == '\\'))){
#ifdef _WIN32
      const char* Home = ::getenv("UserProfile");
#else
      const char* Home = ::getenv("HOME");
#endif
      if (Home && Home[0]){
         return Home + Path.substr(1);
      }
   }
   return Path;
}

COLstring FILparentDir(COLstring Path){
   COL_FUNCTION(FILparentDir);
   COL_VAR(Path);
#ifdef _WIN32
   Path = Path.strip(COLstring::StripType::Trailing, '\\');
#else
   Path = Path.strip(COLstring::StripType::Trailing, '/');
#endif
   COLstring Dir = FILpathSepRemove(FILpathDir(Path,false));
   COL_VAR(Dir);
   return Dir;
}