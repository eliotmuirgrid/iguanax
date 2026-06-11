#ifndef __COL_DLL_H__
#define __COL_DLL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLdll
//
// Description:
//
// Class which wraps basic shared library and dll functionality in an
// operating system independent manner.  
//
// Why is it useful?  There are generally three ways an application can
// link to library code:
//
// 1) Via static linking.
// 2) Via dynamic linking at application load time - this is when we use
//    an import library to a DLL (dynamic linked library) or shared object (the
//    unix equivalent)
// 3) Dynamically loading at runtime - this is what COLdll is for implementing.
//
// Loading dynamically at runtime is the hardest way to link a library
// to an application - but it is the most flexible.  It has the advantages
// that:
//
//   i)  An application can explicitly check for the existence of a DLL and for
//       the existence of functions within that DLL.  This means we can write
//       code which can gracefully handle older operating systems that do not
//       support more advanced functionality we might wish to support.
//
//   ii) It is possible to extend existing applications - i.e. add support for a new
//       kind of graphics format to a graphics application etc.
//
// Hint - dumpbin is a very handy tool when trying to figure out the names of
// symbols in libraries.  See the intranet for details.  nm is the equivalent
// tool in Linux and many other POSIX compliant operating systems.
//
// Author: Eliot Muir, Rishi Marwah with portions ported from the original PLG
// library that Henry Tran did and ported by Guillaume.
//
// Date:   Tue 09/01/1998 
//---------------------------------------------------------------------------

#include "COLstring.h"

typedef void* COLdllFcnPtr;

class COLdllPrivate;

enum COLdllLoadResult
{
   COL_DLL_LOAD_OK = 1,
   COL_DLL_LOAD_INVALID_LIBRARY = -1, // The file was not a valid DLL or shared object - i.e. a text file
   COL_DLL_LOAD_NOT_EXIST = -2, // The file did not exist
   COL_DLL_LOAD_NO_PERMISSIONS = -3, //  We did not have permissions to load it
   COL_DLL_LOAD_UNKNOWN_ERROR = -4
};

const COLuint32 COL_PROC_DOES_NOT_EXIST=127;

class COLdll{
public:
   COLdll();
   COLdll(const COLstring& DllFileName, bool IsOwner=true);
   virtual ~COLdll();   

   // Either loadLibrary or tryLoadLibrary can be used to load a DLL or shared object (*.so) 
   // loadLibrary will throw an exception if it fails.
   // IsOwner determines whether or not COLdll will call freeLibrary automatically.
   void loadLibrary(const COLstring& DllFileName, bool IsOwner=true);
   // loadLibrary but returns bool instead of throwing error
   bool loadLibraryWithStatus(const COLstring& DllFileName, bool IsOwner=true);
   COLdllLoadResult tryLoadLibrary(const COLstring& DllFileName, bool IsOwner=true, COLstring* pError=nullptr);

   // This is the last error message if tryLoadLibrary fails
   const COLstring& lastErrorMessage() const;

   void freeLibrary();

   // This is the symbol name of the function in the DLL
   // hint - checkout out dumpbin as a tool to work out what
   // the symbolic name of a function is - it will be affected
   // by the calling convention and whether is a unicode/ansi
   // version etc.  Under windows this function will throw if the procedure
   // is not found with the error code COL_PROC_DOES_NOT_EXIST
   // - if another more exotic exception is produced the error code
   // from the operating system will be returned.
   //
   // Under POSIX there is no definition of what the error code will be.  The
   // POSIX standard does not define things so exactly.
   void* getProcAddress(const char* ProcName);

   // canLoadProcAddress could return false in some cases where the proc exists but the DLL is unloadable due to other
   // issues such as permissions and dependencies.
   bool canLoadProcAddress(const char* ProcName);

   // The full DLL path - under windows anyway - the POSIX implementation will just
   // give the name of shared object that was passed to loadLibrary or tryLoadLibrary.
   const COLstring& libraryName() const;

   // Returns true if the library was loaded.
   bool loaded() const;
private:
   void* DllHandle;
   bool IsOwner;
   COLstring LibraryName;
   COLstring ErrorString;

   COLdll& operator=(const COLdll& Orig); // not allowed 
   COLdll(const COLdll& Orig); // not allowed
};

#endif // end of defensive include
