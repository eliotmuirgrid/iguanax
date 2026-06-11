#if _MSC_VER >= 1400  /* MSVC 2005 or later */
         
#include <stdlib.h>
#include <windows.h>

static void
invalidParameterHandler(const wchar_t *Expression,
                        const wchar_t *Function,
                        const wchar_t *File,
                        unsigned int   Line,
                        uintptr_t      pReserved)
{
   /* No nothing.  Behave like a Unix runtime and return error codes. */
}

BOOL WINAPI DllMain(HINSTANCE Dll, DWORD Reason, LPVOID Reserved)
{
   switch(Reason) {
   case DLL_PROCESS_ATTACH:
      _set_invalid_parameter_handler(invalidParameterHandler);
   }
   return TRUE;
}

#endif  /* MSVC 2005 or later */
