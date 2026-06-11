//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKerror
//
// Author: Kevin Cai
//
// Description: Maps SCK error codes to their corresponding negative linux equivalents.
//---------------------------------------------------------------------------

#ifndef _WIN32
#include <errno.h>
#endif

#include "SCKsystem.h"
#include "SCKerror.h"

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// WARNING - you have to be SUPER careful about putting any tracing in here before calling WSAGetLastError
// otherwise you'll break the windows implementation of this sockets library.  The problem is tracing can
// reset the last error status to be successful so you lose the error codes.
int SCKlastSocketError(){
   int LastSocketError = COLlastError();
   COL_VAR(LastSocketError);
   int MappedErrorCode = SCKmapErrorCode(LastSocketError);
   COL_VAR(MappedErrorCode);
   return MappedErrorCode;
}

int SCKmapErrorCode(int ErrorCode){
   COL_FUNCTION(SCKmapErrorCode);
#ifndef _WIN32
   ErrorCode *= -1; // Attempting to match errno to our code.
   switch (ErrorCode){
      case SCK_EACCES :
      case SCK_EADDRINUSE :
      case SCK_EADDRNOTAVAIL :
      case SCK_EAFNOSUPPORT :
      case SCK_EAGAIN :
      case SCK_EALREADY :
      case SCK_EBADF :
      case SCK_EBUSY :
      case SCK_ECONNABORTED :
      case SCK_ECONNREFUSED :
      case SCK_ECONNRESET :
      case SCK_EEXIST :
      case SCK_EFAULT :
      case SCK_EINVAL :
      case SCK_EIO :
      case SCK_EISCONN :
      case SCK_EISDIR :
      case SCK_ELOOP :
      case SCK_EMFILE :
      case SCK_EMSGSIZE :
      case SCK_ENAMETOOLONG :
      case SCK_ENETUNREACH :
      case SCK_ENOBUFS :
      case SCK_ENOENT :
      case SCK_ENOMEM :
      case SCK_ENOSPC :
      case SCK_ENOTCONN :
      case SCK_ENOTEMPTY :
      case SCK_ENOTSOCK :
      case SCK_ENOTSUP :
      case SCK_EOF :
      case SCK_EPERM :
      case SCK_EPIPE :
      case SCK_EPROTONOSUPPORT :
      case SCK_EROFS :
      case SCK_ETIMEDOUT :
      case SCK_EXDEV :
      case SCK_E2BIG :
      case SCK_EINPROGRESS :
         return (ErrorCode);
   }
   return -ErrorCode; // Can't find match. Returning system error code as is.
#else
   switch(ErrorCode){
      case ERROR_NOACCESS:                    return SCK_EACCES;
      case WSAEACCES:                         return SCK_EACCES;
      case ERROR_ADDRESS_ALREADY_ASSOCIATED:  return SCK_EADDRINUSE;
      case WSAEADDRINUSE:                     return SCK_EADDRINUSE;
      case WSAEADDRNOTAVAIL:                  return SCK_EADDRNOTAVAIL;
      case WSAEAFNOSUPPORT:                   return SCK_EAFNOSUPPORT;
      case WSAEWOULDBLOCK:                    return SCK_EAGAIN;
      case WSAEALREADY:                       return SCK_EALREADY;
      case ERROR_INVALID_FLAGS:               return SCK_EBADF;
      case ERROR_INVALID_HANDLE:              return SCK_EBADF;
      case ERROR_LOCK_VIOLATION:              return SCK_EBUSY;
      case ERROR_PIPE_BUSY:                   return SCK_EBUSY;
      case ERROR_SHARING_VIOLATION:           return SCK_EBUSY;
      case ERROR_CONNECTION_ABORTED:          return SCK_ECONNABORTED;
      case WSAECONNABORTED:                   return SCK_ECONNABORTED;
      case ERROR_CONNECTION_REFUSED:          return SCK_ECONNREFUSED;
      case WSAECONNREFUSED:                   return SCK_ECONNREFUSED;
      case ERROR_NETNAME_DELETED:             return SCK_ECONNRESET;
      case WSAECONNRESET:                     return SCK_ECONNRESET;
      case ERROR_ALREADY_EXISTS:              return SCK_EEXIST;
      case ERROR_FILE_EXISTS:                 return SCK_EEXIST;
      case ERROR_BUFFER_OVERFLOW:             return SCK_EFAULT;
      case WSAEFAULT:                         return SCK_EFAULT;
      case ERROR_HOST_UNREACHABLE:            return SCK_EHOSTUNREACH;
      case WSAEHOSTUNREACH:                   return SCK_EHOSTUNREACH;
      case ERROR_INSUFFICIENT_BUFFER:         return SCK_EINVAL;
      case ERROR_INVALID_DATA:                return SCK_EINVAL;
      case ERROR_INVALID_PARAMETER:           return SCK_EINVAL;
      case WSAEINVAL:                         return SCK_EINVAL;
      case WSAEPFNOSUPPORT:                   return SCK_EINVAL;
      case WSAESOCKTNOSUPPORT:                return SCK_EINVAL;
      case ERROR_BEGINNING_OF_MEDIA:          return SCK_EIO;
      case ERROR_BUS_RESET:                   return SCK_EIO;
      case ERROR_CRC:                         return SCK_EIO;
      case ERROR_DEVICE_DOOR_OPEN:            return SCK_EIO;
      case ERROR_DEVICE_REQUIRES_CLEANING:    return SCK_EIO;
      case ERROR_DISK_CORRUPT:                return SCK_EIO;
      case ERROR_EOM_OVERFLOW:                return SCK_EIO;
      case ERROR_FILEMARK_DETECTED:           return SCK_EIO;
      case ERROR_GEN_FAILURE:                 return SCK_EIO;
      case ERROR_INVALID_BLOCK_LENGTH:        return SCK_EIO;
      case ERROR_IO_DEVICE:                   return SCK_EIO;
      case ERROR_NO_DATA_DETECTED:            return SCK_EIO;
      case ERROR_NO_SIGNAL_SENT:              return SCK_EIO;
      case ERROR_OPEN_FAILED:                 return SCK_EIO;
      case ERROR_SETMARK_DETECTED:            return SCK_EIO;
      case ERROR_SIGNAL_REFUSED:              return SCK_EIO;
      case WSAEINPROGRESS:                    return SCK_EINPROGRESS;
      case WSAEISCONN:                        return SCK_EISCONN;
      case ERROR_CANT_RESOLVE_FILENAME:       return SCK_ELOOP;
      case ERROR_TOO_MANY_OPEN_FILES:         return SCK_EMFILE;
      case WSAEMFILE:                         return SCK_EMFILE;
      case WSAEMSGSIZE:                       return SCK_EMSGSIZE;
      case ERROR_FILENAME_EXCED_RANGE:        return SCK_ENAMETOOLONG;
      case ERROR_NETWORK_UNREACHABLE:         return SCK_ENETUNREACH;
      case WSAENETUNREACH:                    return SCK_ENETUNREACH;
      case WSAENOBUFS:                        return SCK_ENOBUFS;
      case ERROR_DIRECTORY:                   return SCK_ENOENT;
      case ERROR_FILE_NOT_FOUND:              return SCK_ENOENT;
      case ERROR_INVALID_NAME:                return SCK_ENOENT;
      case ERROR_INVALID_DRIVE:               return SCK_ENOENT;
      case ERROR_INVALID_REPARSE_DATA:        return SCK_ENOENT;
      case ERROR_MOD_NOT_FOUND:               return SCK_ENOENT;
      case ERROR_PATH_NOT_FOUND:              return SCK_ENOENT;
      case WSAHOST_NOT_FOUND:                 return SCK_ENOENT;
      case WSANO_DATA:                        return SCK_ENOENT;
      case ERROR_NOT_ENOUGH_MEMORY:           return SCK_ENOMEM;
      case ERROR_OUTOFMEMORY:                 return SCK_ENOMEM;
      case ERROR_CANNOT_MAKE:                 return SCK_ENOSPC;
      case ERROR_DISK_FULL:                   return SCK_ENOSPC;
      case ERROR_EA_TABLE_FULL:               return SCK_ENOSPC;
      case ERROR_END_OF_MEDIA:                return SCK_ENOSPC;
      case ERROR_HANDLE_DISK_FULL:            return SCK_ENOSPC;
      case ERROR_NOT_CONNECTED:               return SCK_ENOTCONN;
      case WSAENOTCONN:                       return SCK_ENOTCONN;
      case ERROR_DIR_NOT_EMPTY:               return SCK_ENOTEMPTY;
      case WSAENOTSOCK:                       return SCK_ENOTSOCK;
      case ERROR_NOT_SUPPORTED:               return SCK_ENOTSUP;
      case ERROR_BROKEN_PIPE:                 return SCK_EOF;
      case ERROR_ACCESS_DENIED:               return SCK_EPERM;
      case ERROR_PRIVILEGE_NOT_HELD:          return SCK_EPERM;
      case ERROR_BAD_PIPE:                    return SCK_EPIPE;
      case ERROR_NO_DATA:                     return SCK_EPIPE;
      case ERROR_PIPE_NOT_CONNECTED:          return SCK_EPIPE;
      case WSAESHUTDOWN:                      return SCK_EPIPE;
      case WSAEPROTONOSUPPORT:                return SCK_EPROTONOSUPPORT;
      case ERROR_WRITE_PROTECT:               return SCK_EROFS;
      case ERROR_SEM_TIMEOUT:                 return SCK_ETIMEDOUT;
      case WSAETIMEDOUT:                      return SCK_ETIMEDOUT;
      case ERROR_NOT_SAME_DEVICE:             return SCK_EXDEV;
      case ERROR_INVALID_FUNCTION:            return SCK_EISDIR;
      case ERROR_META_EXPANSION_TOO_LONG:     return SCK_E2BIG;
   }
   return ErrorCode;
#endif
}

COLstring SCKsocketError(int ErrorCode){
   if (ErrorCode >= 0){
      return COLerrorString(ErrorCode);
   }
   switch(ErrorCode){
      case SCK_EADDRINUSE       :  return "Address in use";
      case SCK_EADDRNOTAVAIL    :  return "Address not available";
      case SCK_EAFNOSUPPORT     :  return "Address family not supported";
      case SCK_EAGAIN           :  return "Resource temporarily not available, try again later";
      case SCK_EALREADY         :  return "Connection already in progress";
      case SCK_EBADF            :  return "Bad file descriptor";
      case SCK_EBUSY            :  return "Resource is busy or locked";
      case SCK_ECONNABORTED     :  return "Connection aborted";
      case SCK_ECONNRESET       :  return "Connection reset by peer";
      case SCK_EEXIST           :  return "File already exists";
      case SCK_EINVAL           :  return "Invalid argument";
      case SCK_EIO              :  return "I/O error";
      case SCK_EISCONN          :  return "Socket is already connected";
      case SCK_ELOOP            :  return "Too many symbolic links";
      case SCK_EMFILE           :  return "Too many open files";
      case SCK_EMSGSIZE         :  return "Message size too long";
      case SCK_ENAMETOOLONG     :  return "Name too long";
      case SCK_ENETUNREACH      :  return "Unreachable network";
      case SCK_ENOBUFS          :  return "No buffer space available";
      case SCK_ENOENT           :  return "File or directory not found";
      case SCK_ENOMEM           :  return "Not enough memory available";
      case SCK_ENOSPC           :  return "No space left on device";
      case SCK_ENOTCONN         :  return "Socket is not connected";
      case SCK_ENOTEMPTY        :  return "Directory is not empty";
      case SCK_ENOTSOCK         :  return "Socket operation on non-socket";
      case SCK_ENOTSUP          :  return "Operation not supported on socket";
      case SCK_EOF              :  return "End of file";
      case SCK_EPERM            :  return "Operation not permitted";
      case SCK_EPIPE            :  return "Broken pipe";
      case SCK_EPROTONOSUPPORT  :  return "Protocol not supported";
      case SCK_EROFS            :  return "Read-only file system";
      case SCK_ETIMEDOUT        :  return "Connection timed out";
      case SCK_EXDEV            :  return "Cross-device link not permitted";
      case SCK_EISDIR           :  return "Illegal operation on a directory";
      case SCK_E2BIG            :  return "Argument list is too big";
      case SCK_EACCES           :  return "Permission denied";
      case SCK_ECONNREFUSED     :  return "Connection refused";
      case SCK_EFAULT           :  return "Bad memory address";
      case SCK_EINPROGRESS      :  return "Operation in progress";
      case SCK_DNSFAILURE       :  return "Could not resolve hostname";
      case SCK_ESSLVERIFY       :  return "Certificate verification failed";
      case SCK_SSLBADHOST       :  return "Hostname mismatch";
   }
   return "Unknown error";
}
