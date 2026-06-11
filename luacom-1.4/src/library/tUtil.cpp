// tUtil.cpp: implementation of the tUtil class.
//
//////////////////////////////////////////////////////////////////////

// RCS Info
static char *rcsid = "$Id: tUtil.cpp,v 1.4 2012-09-10 19:19:27 nasron Exp $";
static char *rcsname = "$Name: HEAD $";


#include <assert.h>
#if !defined(__WINE__) || defined(__MSVCRT__)
#include <process.h>
#endif

#include "tUtil.h"
#include "luacom_dll.h"
#include "tLuaCOMException.h"

#define MAX_VALID_STRING_SIZE 1000

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool tUtil::IsValidString(LPCTSTR string)
{
  bool return_value = string != NULL &&
    !IsBadStringPtr(string, MAX_VALID_STRING_SIZE);

  assert(return_value);

  return return_value;
}

std::string tUtil::GetErrorMessage(DWORD errorcode)
{
  LPTSTR lpMsgBuf;
  DWORD result = 0;

  result = FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    errorcode,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL);

  if(result == 0)
    return NULL;

  // Strip the newline at the end of the message

  while((result > 1) && ((lpMsgBuf[result-1] == '\n') || (lpMsgBuf[result-1] == '\r')))
    result--;
  lpMsgBuf[result] = '\0';

  std::string out(lpMsgBuf);

  // ifware, if error code is CO_E_NOTINITIALIZED, make a helpful error message
  if (errorcode == CO_E_NOTINITIALIZED){
     char* pErr = "\nIf luacom is being used from within an Iguana Translator,\n"
        "this error occurs because Iguana was not configured to initialize COM for translator scripts.\n"
        "The following steps should be taken:\n"
        " 1) Set the environment variable CHM_LUACOM_ENABLE to 'true' (without the quotes).\n"
        "    This can be done through Settings->Environment Variables, or in the operating system\n"
        "    before Iguana starts.\n"
        " 2) Restart Iguana.\n\n";
     out += pErr;
  }

  // Free the buffer.
  LocalFree( lpMsgBuf );

  return out;
}

std::string tUtil::bstr2string(BSTR bstr)
{
  char* str = NULL;
  long size = 0;
  int result = 0;

  try
  {
    if(bstr != NULL)
    {
      // gets the size of the buffer
      size = WideCharToMultiByte(
        CP_UTF8,            // code page
        0,            // performance and mapping flags
        bstr,    // wide-character string
        -1,          // number of chars in string
        str,     // buffer for new string
        0,          // size of buffer
        NULL,     // default for unmappable chars
        NULL  // set when default char used
      );

      if(!size)
        LUACOM_ERROR(tUtil::GetErrorMessage(GetLastError()).c_str());

      str = new char[size];

      result = WideCharToMultiByte(
        CP_UTF8,            // code page
        0,            // performance and mapping flags
        bstr,    // wide-character string
        -1,          // number of chars in string
        str,     // buffer for new string
        size,          // size of buffer
        NULL,     // default for unmappable chars
        NULL  // set when default char used
      );

      if(!result)
        LUACOM_ERROR(tUtil::GetErrorMessage(GetLastError()).c_str());

    }
    else
    {
      str = new char[1];
      str[0] = '\0';
    }
  }
  catch(class tLuaCOMException& e)
  {
    UNUSED(e);

    if(str)
      delete[] str;

    str = new char[1];
    str[0] = '\0';
  }

  std::string out(str);

  delete[] str;

  return out;
}

std::string tUtil::bstr2string(BSTR bstr, size_t& computedSize)
{
  char* str = NULL;
  long size = 0;
  int result = 0;

  computedSize = 0;

  try
  {
    if(bstr != NULL)
    {
	  computedSize = SysStringLen(bstr);
	  if(!computedSize) {
		  return "";
	  }

      // gets the size of the buffer
      size = WideCharToMultiByte(
        CP_UTF8,            // code page
        0,            // performance and mapping flags
        bstr,    // wide-character string
		  (int)computedSize,	// number of chars in string
        str,     // buffer for new string
        0,          // size of buffer
        NULL,     // default for unmappable chars
        NULL  // set when default char used
      );

      if(!size)
        LUACOM_ERROR(tUtil::GetErrorMessage(GetLastError()).c_str());

      str = new char[size];

      result = WideCharToMultiByte(
        CP_UTF8,            // code page
        0,            // performance and mapping flags
        bstr,    // wide-character string
        (int)computedSize,	// number of chars in string
        str,     // buffer for new string
        size,          // size of buffer
        NULL,     // default for unmappable chars
        NULL  // set when default char used
      );

      if(!result)
         LUACOM_ERROR(tUtil::GetErrorMessage(GetLastError()).c_str());

    }
    else
    {
      str = new char[1];
      str[0] = '\0';
    }
  }
  catch(class tLuaCOMException& e)
  {
    UNUSED(e);

    if(str)
      delete[] str;

    str = new char[1];
    str[0] = '\0';
  }

  std::string out(str,computedSize);

  delete[] str;

  return out;
}

BSTR tUtil::string2bstr(const char * string, size_t len)
{
  if(!string)
    return NULL;

  BSTR bstr;
  if(len == 0)
  {
    bstr = SysAllocStringLen(NULL, 0);
  }
  else
  {
	long wclength = 
      MultiByteToWideChar(CP_UTF8, 0, string, (int)len, NULL, 0);
	try
    {
      if(wclength == 0)
        LUACOM_ERROR(tUtil::GetErrorMessage(GetLastError()).c_str());
    }
    catch(class tLuaCOMException& e)
    {
      UNUSED(e);
      return NULL;
    }
    bstr = SysAllocStringLen(NULL, wclength);
    MultiByteToWideChar(CP_UTF8, 0, string, (int)len, bstr, wclength);
  }

  return bstr;
}

bool tUtil::OpenLogFile(const char *name)
{
  tUtil::CloseLogFile();
  tLuaSetLogFile( fopen(name, "w") );

  if(!tLuaGetLogFile())
    return false;
  else
    return true;
}

void tUtil::CloseLogFile()
{
  FILE* pLogFile = tLuaGetLogFile();
  if(pLogFile)
  {
    fclose(pLogFile);
    tLuaSetLogFile(0);
  }
}

void tUtil::log(const char *who, const char *what, ...)
{
  FILE* pLogFile = tLuaGetLogFile();
  if(pLogFile && who && what)
  {
    int size = 0;

    fprintf(pLogFile, "%s:", who);

    va_list marker;
    va_start(marker, what);

    size = vfprintf(pLogFile, what, marker);

    va_end(marker);

    if(what[strlen(what) - 1] != '\n')
      fprintf(pLogFile, "\n");

    fflush(pLogFile);

#ifdef VERBOSE

    char *buffer = new char[size+1];

    sprintf(buffer, "%s:", who);

    va_start(marker, what);

    size = vsprintf(buffer, what, marker);

    va_end(marker);

    MessageBox(NULL, buffer, "LuaCOM Log", MB_OK | MB_ICONEXCLAMATION);

    delete[] buffer;
    buffer = NULL;

#endif // VERBOSE
  }
}

void tUtil::log_verbose(const char *who, const char *what, ...)
{
#ifdef VERBOSE
  FILE* pLogFile = tLuaGetLogFile();
  if(pLogFile && who && what)
  {
    fprintf(pLogFile, "%s:", who);

    va_list marker;
    va_start(marker, what);

    vfprintf(pLogFile, what, marker);

    va_end(marker);

    if(what[strlen(what) - 1] != '\n')
      fprintf(pLogFile, "\n");

    fflush(pLogFile);
  }
#endif
}


char * tUtil::strdup(const char *string)
{
  if(!string)
    return NULL;

  char *new_string = (char *) malloc(strlen(string)+1);

  strcpy(new_string, string);

  return new_string;
}

void tUtil::ShowHelp(const char *filename, unsigned long context)
{
  // filename must have at least the extension
  if(!filename || strlen(filename) < 5)
    return;

  const char* extension = &filename[strlen(filename) - 4];

  if(_stricmp(extension, ".chm") == 0)
  {
    char context_param[50];
  
    if(context != 0)
      sprintf(context_param, "-mapid %d", context);
    else
      context_param[0] = '\0';
#if !defined(__WINE__) || defined(__MSVCRT__)
    _spawnlp(_P_NOWAIT, "hh.exe", "hh.exe", context_param, filename, NULL);
#else
    MessageBox(NULL, "FIX - not implemented - _spawnlp", "LuaCOM", MB_ICONEXCLAMATION);
    #warning FIX - not implemented - _spawnlp
#endif
  }
  else if(_stricmp(extension, ".hlp") == 0)
  {
    if(context != 0)
      WinHelp(NULL, filename, HELP_CONTEXT, context);
    else
      WinHelp(NULL, filename, HELP_FINDER, 0);
  }
}
