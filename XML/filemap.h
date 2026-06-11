/*
Copyright (c) 1998, 1999 Thai Open Source Software Center Ltd
See the file COPYING for copying permission.
*/
// This file is used for the implementation of XMLfileParser
// together with win32filemap.cpp
//
// it's a somewhat incomplete hack by Eliot


#include <stddef.h>

#ifdef XML_UNICODE
int filemap(const wchar_t *name,
       void (*processor)(const void *, size_t, const wchar_t *, void *arg),
       void *arg);
#else
#ifdef __cplusplus
   extern "C"
#endif // _cplusplus
int filemap(const char *name,
       void (*processor)(const void *, size_t, const char *, void *arg),
       void *arg);
#endif
