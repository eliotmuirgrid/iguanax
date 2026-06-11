// tLuaCOMException.cpp: implementation of the tLuaCOMException class.
//
//////////////////////////////////////////////////////////////////////

//#include <iostream.h>
#include <string.h>
#include <stdio.h>

#include "tLuaCOMException.h"
#include "tUtil.h"

char *tLuaCOMException::messages[] =  
     {
       "Internal Error",
       "Parameter(s) out of range",
       "Type conversion error",
       "COM error",
       "COM exception",
       "Unsupported feature required",
       "Windows error",
       "LuaCOM error",
       "Memory allocation error"
     };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

tLuaCOMException::tLuaCOMException(Errors p_code, char *p_file, int p_line,
                                   const char *p_usermessage)
{
  code = p_code;

  file = p_file;
  line = p_line;
  usermessage = p_usermessage ? p_usermessage : "";

  tUtil::log("luacom", getMessage().c_str());
}

tLuaCOMException::~tLuaCOMException()
{
}

std::string tLuaCOMException::getMessage()
{
   std::string errorbuffer;
   errorbuffer = usermessage;

   errorbuffer += "Additional Debug Info: ";
   errorbuffer += messages[code];
   if(file.size())
   {
      char error_position[1000];
      sprintf(error_position, ", (%s,%d)", file.c_str(), line);
      errorbuffer += error_position;
   }

   return errorbuffer;
}

std::string tLuaCOMException::GetErrorMessage(DWORD errorcode)
{
  return tUtil::GetErrorMessage(errorcode);
}
