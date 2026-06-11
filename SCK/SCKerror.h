#ifndef __SCK_ERROR_H__
#define __SCK_ERROR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKerror
//
// Author: Kevin Cai
//
// Description: Maps SCK error codes to their corresponding negative linux equivalents.
//
// Implementation: On linux, the corresponding SCK error code is the negated of error code.
//     On windows, some system error codes are matched to corresponding SCK error codes.
//---------------------------------------------------------------------------
#include <COL/COLstring.h>

#ifdef _WIN32
#include <errno.h>
#endif

// WARNING! Don't use this call directly.  For Windows especially
// things like ::closesocket will reset the error code.  So it's important
// to get the error code immediately after a socket call fails - before you
// even do tracing since tracing under windows will invoke functions which
// result in a successful call to the OS and thereby clear the error flag.
int SCKlastSocketError();

// Returns the corresponding SCKerror if it exists, otherwise leaves it as is.
int SCKmapErrorCode(int ErrorCode);

// If error code < 0 (it is a SCK error), returns a personalized string.
// Otherwsie leaves it to the OS using ::strerror().
COLstring SCKsocketError(int ErrorCode);

// Other possible mappings:
// ERROR_IO_PENDING
// WSA_IO_PENDING
// WAIT_TIMEOUT
// EINTR

// TODO - We have adopted a similar solution to libuv (aka node.js - one disadvantage with this is that
// we don't currently have a nice readable list of the defined codes in a header file - we can leave
// it as is or later pick a different technique like mapping the codes under the hood or using extern variables etc. etc. Eliot
// No urgency - let's see how it goes.

#ifdef EWOULDBLOCK // Only for OS before 2002 that have these two macros seperate.
   #undef EWOULDBLOCK
   #define EWOULDBLOCK EAGAIN
#endif

enum{
#ifdef EADDRINUSE
   SCK_EADDRINUSE = -EADDRINUSE,
#else
   SCK_EADDRINUSE = -1001,
#endif

#ifdef EADDRNOTAVAIL
   SCK_EADDRNOTAVAIL = -EADDRNOTAVAIL,
#else
   SCK_EADDRNOTAVAIL = -1002,
#endif

#ifdef EAFNOSUPPORT
   SCK_EAFNOSUPPORT = -EAFNOSUPPORT,
#else
   SCK_EAFNOSUPPORT = -1003,
#endif

#ifdef EAGAIN
   SCK_EAGAIN = -EAGAIN,
#else
   SCK_EAGAIN = -1004,
#endif

#ifdef EALREADY
   SCK_EALREADY = -EALREADY,
#else
   SCK_EALREADY = -1005,
#endif

#ifdef EBADF
   SCK_EBADF = -EBADF,
#else
   SCK_EBADF = -1006,
#endif

#ifdef EBUSY
   SCK_EBUSY = -EBUSY,
#else
   SCK_EBUSY = -1007,
#endif

#ifdef ECONNABORTED
   SCK_ECONNABORTED = -ECONNABORTED,
#else
   SCK_ECONNABORTED = -1008,
#endif

#ifdef ECONNRESET
   SCK_ECONNRESET = -ECONNRESET,
#else
   SCK_ECONNRESET = -1009,
#endif

#ifdef EEXIST
   SCK_EEXIST = -EEXIST,
#else
   SCK_EEXIST = -1010,
#endif

#ifdef EHOSTUNREACH
   SCK_EHOSTUNREACH = -EHOSTUNREACH,
#else
   SCK_EHOSTUNREACH = -1011,
#endif

#ifdef EINVAL
   SCK_EINVAL = -EINVAL,
#else
   SCK_EINVAL = -1012,
#endif

#ifdef EIO
   SCK_EIO = -EIO,
#else
   SCK_EIO = -1013,
#endif

#ifdef EISCONN
   SCK_EISCONN = -EISCONN,
#else
   SCK_EISCONN = -1014,
#endif

#ifdef ELOOP
   SCK_ELOOP = -ELOOP,
#else
   SCK_ELOOP = -1015,
#endif

#ifdef EMFILE
   SCK_EMFILE = -EMFILE,
#else
   SCK_EMFILE = -1016,
#endif

#ifdef EMSGSIZE
   SCK_EMSGSIZE = -EMSGSIZE,
#else
   SCK_EMSGSIZE = -1017,
#endif

#ifdef ENAMETOOLONG
   SCK_ENAMETOOLONG = -ENAMETOOLONG,
#else
   SCK_ENAMETOOLONG = -1018,
#endif

#ifdef ENETUNREACH
   SCK_ENETUNREACH = -ENETUNREACH,
#else
   SCK_ENETUNREACH = -1019,
#endif

#ifdef ENOBUFS
   SCK_ENOBUFS = -ENOBUFS,
#else
   SCK_ENOBUFS = -1020,
#endif

#ifdef ENOENT
   SCK_ENOENT = -ENOENT,
#else
   SCK_ENOENT = -1021,
#endif

#ifdef ENOMEM
   SCK_ENOMEM = -ENOMEM,
#else
   SCK_ENOMEM = -1022,
#endif

#ifdef ENOSPC
   SCK_ENOSPC = -ENOSPC,
#else
   SCK_ENOSPC = -1023,
#endif

#ifdef ENOTCONN
   SCK_ENOTCONN = -ENOTCONN,
#else
   SCK_ENOTCONN = -1024,
#endif

#ifdef ENOTEMPTY
   SCK_ENOTEMPTY = -ENOTEMPTY,
#else
   SCK_ENOTEMPTY = -1025,
#endif

#ifdef ENOTSOCK
   SCK_ENOTSOCK = -ENOTSOCK,
#else
   SCK_ENOTSOCK = -1026,
#endif

#ifdef ENOTSUP
   SCK_ENOTSUP = -ENOTSUP,
#else
   SCK_ENOTSUP = -1027,
#endif

#ifdef EOF
   SCK_EOF = -EOF,
#else
   SCK_EOF = -1028,
#endif

#ifdef EPERM
   SCK_EPERM = -EPERM,
#else
   SCK_EPERM = -1029,
#endif

#ifdef EPIPE
   SCK_EPIPE = -EPIPE,
#else
   SCK_EPIPE = -1030,
#endif

#ifdef EPROTONOSUPPORT
   SCK_EPROTONOSUPPORT = -EPROTONOSUPPORT,
#else
   SCK_EPROTONOSUPPORT = -1031,
#endif

#ifdef EROFS
   SCK_EROFS = -EROFS,
#else
   SCK_EROFS = -1032,
#endif

#ifdef ETIMEDOUT
   SCK_ETIMEDOUT = -ETIMEDOUT,
#else
   SCK_ETIMEDOUT = -1033,
#endif

#ifdef EXDEV
   SCK_EXDEV = -EXDEV,
#else
   SCK_EXDEV = -1034,
#endif

#ifdef EISDIR
   SCK_EISDIR = -EISDIR,
#else
   SCK_EISDIR = -1035,
#endif

#ifdef E2BIG
   SCK_E2BIG = -E2BIG,
#else
   SCK_E2BIG = -1036,
#endif

#ifdef EACCES
   SCK_EACCES = -EACCES,
#else
   SCK_EACCES = -1037,
#endif

#ifdef ECONNREFUSED
   SCK_ECONNREFUSED = -ECONNREFUSED,
#else
   SCK_ECONNREFUSED = -1038,
#endif

#ifdef EFAULT
   SCK_EFAULT = -EFAULT,
#else
   SCK_EFAULT = -1039,
#endif

#ifdef EINPROGRESS
   SCK_EINPROGRESS = -EINPROGRESS,
#else
   SCK_EINPROGRESS = -1040,
#endif

SCK_DNSFAILURE = -1041,
SCK_ESSLVERIFY = -1042,
SCK_SSLBADHOST = -1043,
};


#endif // End of defensive guard
