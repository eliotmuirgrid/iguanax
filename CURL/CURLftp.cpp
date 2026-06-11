//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLftp
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Sunday, November 8th, 2009
//---------------------------------------------------------------------------

#include "CURLftp.h"
#include "CURLftpContext.h"
#include "CURLutils.h"

#include <curl/curl.h>

#include <COL/COLutils.h>
#include <COL/COLauto.h>
#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <ctype.h>
#include <time.h>

extern "C"
{
#include <CURL/ftpparse.h>
}

//---------------------------------------------------------------------------
static COLstring CURLftpBuildUrl(const COLstring& Protocol, const CURLftpContext& Context, const COLstring& File)
{
   COLstring Result;
   COLostream Stream(Result);
   Stream << Protocol << "://" << Context.host() << ':' << Context.port()
      << '/' << File;
   // For FTP and FTPS (SSL), File is relative to the user's home folder.
   // To access files in the root folder, they need to have a URL like
   // ftp://host:21//file-in-root, so we can't prohibit double slashes.
   return Result;
}

//---------------------------------------------------------------------------
static COLstring CURLftpBuildAuthString(const CURLftpContext& Context, bool NoPassword)
{
   if (Context.username().is_null()) return COLstring();
   COLstring Result;
   COLostream Stream(Result);
   Stream << Context.username();
   if (!NoPassword) {
      Stream << ':' << Context.password();
   }
   return Result;
}

//---------------------------------------------------------------------------
static COLstring CURLftpShellEscapeFilename(const COLstring& Filename) {
   COL_FUNCTION(CURLftpShellEscapeFilename);
   // Escape the file name for commands like 'rm' to work properly
   // This prevents globbing with * and allows spaces in file names
   if (Filename[0] != '"' && Filename[Filename.size()-1] != '"') {
      COLstring EscapedFilename = Filename;
      EscapedFilename.replace("\"", "\\\"");
      EscapedFilename = '"' + EscapedFilename + '"';
      COL_VAR(EscapedFilename);
      return EscapedFilename;
   }
   // else assume the user specified the filename in quotes already
   COL_TRC("Looks good as is");
   COL_VAR(Filename);
   return Filename;
}

class CURLftpWriteHelper
{
public:
   CURLftpWriteHelper(COLsink& Target, CURLftp* pFtp)
   : m_pTarget(&Target), m_pFtp(pFtp), m_ExceptionThrown(false) {
      COL_METHOD(CURLftpWriteHelper::CURLftpWriteHelper);
   }
   ~CURLftpWriteHelper() {
      COL_METHOD(CURLftpWriteHelper::~CURLftpWriteHelper);
   }
   size_t doWrite(const void* pData, size_t SizeOfData) {
      COL_METHOD(CURLftpWriteHelper::doWrite);
      COL_VAR(SizeOfData);
      if (!m_ExceptionThrown) {
         try {
            return m_pTarget->write(pData,SizeOfData);
         }
         catch(const COLerror& Error) {
            m_ExceptionThrown = true;
            m_Exception = Error;
         }
      }
      return -1;
   }
   bool getException(COLerror& Exception) {
      COL_METHOD(CURLftpWriteHelper::getException);
      if (m_ExceptionThrown) {
         Exception = m_Exception;
         return true;
      }
      return false;
   }
private:
   CURLftp* m_pFtp;
   COLsink* m_pTarget;
   COLerror m_Exception;
   bool m_ExceptionThrown;
};

class CURLftpReadHelperBase
{
public:
   CURLftpReadHelperBase() {
      COL_METHOD(CURLftpReadHelperBase);
   }
   virtual ~CURLftpReadHelperBase(){
      COL_METHOD(~CURLftpReadHelperBase);
   }
   virtual bool getException(COLerror& Exception) = 0;
   virtual size_t doRead(void* pData, int SizeOfData) = 0;
   static size_t curlReadFileFunc(void* pDestination, size_t Size, size_t Num, void* pSource) {
      COL_FUNCTION(CURLftpReadHelperBase::curlReadFileFunc);
      COL_VAR2(Size, Num);
      CURLftpReadHelperBase* pHelper = (CURLftpReadHelperBase*)pSource;
      return pHelper->doRead(pDestination, Size * Num);
   }
private:
   CURLftpReadHelperBase(const CURLftpReadHelperBase&);
   CURLftpReadHelperBase& operator=(const CURLftpReadHelperBase&);
};

class CURLftpReadHelper : public CURLftpReadHelperBase
{
public:
   CURLftpReadHelper(FILfile& Source, CURLftp* pFtp)
   : m_pSource(&Source), m_pFtp(pFtp), m_ExceptionThrown(false) {
      COL_METHOD(CURLftpReadHelper::CURLftpReadHelper);
   }
   ~CURLftpReadHelper() {
      COL_METHOD(CURLftpReadHelper::~CURLftpReadHelper);
   }
private:
   size_t doRead(void* pData, int SizeOfData) {
      COL_METHOD(CURLftpReadHelper::doRead);
      if (!m_ExceptionThrown) {
         try {
            return m_pSource->read(pData, SizeOfData);
         }
         catch(const COLerror& Error) {
            m_ExceptionThrown = true;
            m_Exception = Error;
         }
      }
      return CURL_READFUNC_ABORT;
   }
   bool getException(COLerror& Exception) {
      COL_METHOD(CURLftpReadHelper::getException);
      if (m_ExceptionThrown) {
         Exception = m_Exception;
         return true;
      }
      return false;
   }
private:
   CURLftp* m_pFtp;
   FILfile* m_pSource;
   COLerror m_Exception;
   bool m_ExceptionThrown;
};

class CURLftpBufferReadHelper : public CURLftpReadHelperBase
{
public:
   CURLftpBufferReadHelper(const void* pData, COLint32 DataSize, CURLftp* pFtp)
   : m_pSource((const char*)pData)
   , m_DataSize(DataSize)
   , m_pFtp(pFtp)
   , m_ExceptionThrown(false)
   , m_CurrentRead(0) {
      COL_METHOD(CURLftpBufferReadHelper::CURLftpBufferReadHelper);
   }
   ~CURLftpBufferReadHelper() {
      COL_METHOD(CURLftpBufferReadHelper::~CURLftpBufferReadHelper);
   }
private:
    size_t doRead(void* pData, int SizeOfData) {
       COL_METHOD(CURLftpBufferReadHelper::doRead);
       if (!m_ExceptionThrown) {
          try {
             int AmountToRead = COL_MIN( m_DataSize - m_CurrentRead, SizeOfData );
             if (AmountToRead){
                const char* pStart = m_pSource + m_CurrentRead;
                ::memcpy(pData,pStart,AmountToRead);
                m_CurrentRead += AmountToRead;
             }
             COL_VAR(AmountToRead);
             return AmountToRead;
          }
          catch(const COLerror& Error) {
             m_ExceptionThrown = true;
             m_Exception = Error;
          }
       }
       return CURL_READFUNC_ABORT;
    }
    bool getException(COLerror& Exception) {
       COL_METHOD(CURLftpBufferReadHelper::getException);
       if (m_ExceptionThrown) {
          Exception = m_Exception;
          return true;
       }
       return false;
    }
private:
   COLint32 m_CurrentRead;
   const COLint32 m_DataSize;
   CURLftp* m_pFtp;
   const char* m_pSource;
   COLerror m_Exception;
   bool m_ExceptionThrown;
};

//---------------------------------------------------------------------------
// Used when we receive data from the CURL requests

static size_t CURLftpWriteIgnoreFunc(void* pBuffer, size_t Size, size_t Number, void* pIgnore)
{
   COL_FUNCTION(CURLftpWriteIgnoreFunc);
   //COL_VAR(pIgnore); // not used
   COL_DUMP("unused/ignore", pBuffer, Size*Number);
   return Size * Number; // successful write
}

static size_t CURLftpWriteFuncHelper(void* pBuffer, size_t Size, size_t Number, void* pExtra)
{
   COL_FUNCTION(CURLftpWriteFuncHelper);
   COL_VAR(pExtra);
   CURLftpWriteHelper* pHelper = (CURLftpWriteHelper*)pExtra;
   COL_DUMP("pBuffer", pBuffer, Size*Number);
   return pHelper->doWrite(pBuffer, Size * Number);
}

static size_t CURLftpWriteStringVectorFunc(void* pBuffer, size_t Size, size_t Number, void* pExtra)
{
   COL_FUNCTION(CURLftpWriteStringVectorFunc);
   COL_VAR(pExtra);
   COL_DUMP("pBuffer", pBuffer, Size*Number);
   COLarray<COLstring>& Vector = *(COLarray<COLstring>*)pExtra;
   COLstring& NewStr = Vector.push_back();
   NewStr.write(pBuffer, Size * Number);
   NewStr.strip(COLstring::Trailing, '\n');
   return Size * Number;
}

struct CURLcontextReasonString
{
   CURLcode   ReasonCode;
   char  ReasonString[128];
};

static const CURLcontextReasonString CURLcontextErrStrings[] =
{
   { CURLE_COULDNT_RESOLVE_HOST, "The host could not be resolved." },
   { CURLE_COULDNT_CONNECT, "Could not connect to host." },
   { CURLE_FTP_WEIRD_SERVER_REPLY, "Received an unexpected reply from the FTP server."},
   { CURLE_SSL_CONNECT_ERROR, "An error occurred while trying to establish an SSL connection."},
   { CURLE_PEER_FAILED_VERIFICATION, "Certificate validation failed because the certificate could not be verified."},
   { CURLE_LOGIN_DENIED, "Invalid username and/or password."},
   { (CURLcode)0 }
};

static const char* pSshPeerVerificationFailedError = "Host fingerprint validation failed.";

static void CURLftpGetReasonString(CURLcode ErrCode, const char* pErrorBuffer, const COLstring& Protocol, COLostream& Stream)
{
   const CURLcontextReasonString* pReasonList = CURLcontextErrStrings;
   //find in our list first
   while(pReasonList && pReasonList->ReasonString[0]) {
      if (pReasonList->ReasonCode == ErrCode) {
         if (pReasonList->ReasonCode == CURLE_PEER_FAILED_VERIFICATION && Protocol == "sftp"){
            Stream << pSshPeerVerificationFailedError;
         } else{
            Stream << pReasonList->ReasonString;
         }
         if (pErrorBuffer[0] != '\0') {
            Stream << newline << "Debug Info: " << pErrorBuffer;
         }
         break;
      }
      pReasonList++;
   }
   if (!pReasonList || !pReasonList->ReasonString[0]) {
      //not found
      if (pErrorBuffer[0] != '\0') {
         Stream << pErrorBuffer;
      }
      else {
         Stream << curl_easy_strerror(ErrCode);
      }
   }
}

typedef COLarray< COLauto<COLstring> > TOptStringList;

COLstring CURLftpFindFtpError(CURL* pHandle, const COLarray<COLstring>* pHeaderInfo)
{
   COL_FUNCTION(CURLftpFindFtpError);
   COLstring FtpErrorStr;
   //see if we can get an ftp error out of the ftp response,
   long FtpResponse = 0;
   curl_easy_getinfo(pHandle, CURLINFO_RESPONSE_CODE, &FtpResponse);

   if (FtpResponse/100 == 5 || FtpResponse/100 == 4) {
      //error codes, or transient errors, try to find them in header info if available
      //find in reverse order
      if (pHeaderInfo && pHeaderInfo->size()) {
         COLstring CodeStr = COLint32ToString(FtpResponse);
         const COLstring* pHeaderStr = pHeaderInfo->end();
         pHeaderStr--;
         while(FtpResponse) {
            if (pHeaderStr->find(CodeStr) != npos) {
               FtpErrorStr = *pHeaderStr;
               FtpResponse = 0; //found
            }
            if (pHeaderStr == pHeaderInfo->begin()) {
               break;
            }
            else {
               pHeaderStr--;
            }
         }
      }
   }

   if (FtpResponse && pHeaderInfo) {
      COLostream FtpErrorStream(FtpErrorStr);
      //unknown error code, just provide all info from the header info
      for (const COLstring* pStr = pHeaderInfo->begin(); pStr != pHeaderInfo->end(); pStr++) {
         FtpErrorStream << *pStr << '\n';
      }
   }
   COL_VAR(FtpErrorStr);
   return FtpErrorStr;
}

//---------------------------------------------------------------------------
// This class provides a nice wrapper around a CURL* object. Use the
// CURL_SETOPT and CURL_SETSTROPT to ensure errors are handled cleanly and
// strings are cached locally to avoid bad references within the CURL* obj.
class CURLcontext
{
   public:
      CURLcontext(COLuint32 TimeoutInSeconds, bool DebugToStdErr)
      : m_pQuoteList(0), m_pPostQuoteList(0) {
         COL_METHOD(CURLcontext::CURLcontext);
         COL_VAR2(TimeoutInSeconds, DebugToStdErr);
         m_ErrorBuffer[0] = '\0';

         m_pHandle = CURLeasyInit();
         curl_easy_setopt(m_pHandle, CURLOPT_ERRORBUFFER, m_ErrorBuffer);

         if (DebugToStdErr){
            curl_easy_setopt(m_pHandle, CURLOPT_VERBOSE, 1);
         }
         curl_easy_setopt(m_pHandle, CURLOPT_NOSIGNAL, 1);
         curl_easy_setopt(m_pHandle, CURLOPT_TIMEOUT, TimeoutInSeconds);
      }
      ~CURLcontext() {
         COL_METHOD(CURLcontext::~CURLcontext);
         //important to clear all this so nothing weird gets done during shutdown
         clearOptions();
         clearQuoteList();
         clearPostQuoteList();
         curl_easy_cleanup(m_pHandle);
         m_pHandle = NULL;
      }
      void checkError(CURLcode Code, COLuint32 LineNumber, const COLstring& OperationStr = "", const COLarray<COLstring>* pHeaderInfo = 0) {
         //COL_METHOD(CURLcontext::checkError);
         if (Code != CURLE_OK) {
            COLstring FtpErrorStr = CURLftpFindFtpError(m_pHandle, pHeaderInfo);
            bool UseCurlErrorStr = true;

            switch(Code) {
            // these errors should have enough info from either the errorstring
            // and/or the header output
            case CURLE_QUOTE_ERROR:
            case CURLE_PARTIAL_FILE:
            case CURLE_REMOTE_ACCESS_DENIED:
               UseCurlErrorStr = FtpErrorStr.is_null();
               break;
            default:
               break;
            }

            COLstring PreCurlErrorStr;
            COLstring ErrDesc;
            COLostream ErrStream(ErrDesc);
            if (OperationStr.size()) {
               ErrStream << OperationStr;
               if (UseCurlErrorStr) {
                  COLostream PreCurlErrorStream(PreCurlErrorStr);
                  PreCurlErrorStream << newline << "Additional Info: ";
               }
            }
            if (UseCurlErrorStr) {
               ErrStream << PreCurlErrorStr;
               CURLftpGetReasonString(Code, m_ErrorBuffer,m_Protocol, ErrStream);
            }
            if (FtpErrorStr.size()) {
               ErrStream << newline << "FTP server response:" << newline << FtpErrorStr;
            }
            COL_VAR(ErrDesc);
            throw COLerror(ErrDesc, LineNumber, __FILE__, Code);
         }
      }
      CURL* handle() {
         return m_pHandle;
      }
      void addQuote(const char* pStr) {
         COL_METHOD(CURLcontext::addQuote);
         COL_VAR(pStr);
         m_pQuoteList = curl_slist_append(m_pQuoteList, pStr);
      }
      curl_slist* quoteList() {
         return m_pQuoteList;
      }
      void clearQuoteList() {
         COL_METHOD(CURLcontext::clearQuoteList);
         curl_slist_free_all(m_pQuoteList);
         m_pQuoteList = 0;
      }
      void addPostQuote(const char* pStr) {
         COL_METHOD(CURLcontext::addPostQuote);
         COL_VAR(pStr);
         m_pPostQuoteList = curl_slist_append(m_pPostQuoteList, pStr);
      }
      curl_slist* postQuoteList() {
         return m_pPostQuoteList;
      }
      void clearPostQuoteList() {
         COL_METHOD(CURLcontext::clearPostQuoteList);
         curl_slist_free_all(m_pPostQuoteList);
         m_pPostQuoteList = 0;
      }
      void clearOptions();
      TOptStringList& contextStrings() {
         return m_ContextStrings;
      }
      COLstring m_Protocol;
private:
      CURLcontext(const CURLcontext&);            // disallow
      CURLcontext& operator=(const CURLcontext&); // disallow

      struct curl_slist* m_pQuoteList;
      struct curl_slist* m_pPostQuoteList;
      CURL* m_pHandle;
      char  m_ErrorBuffer[CURL_ERROR_SIZE];
      TOptStringList m_ContextStrings;
};

#define CURL_SETOPT(_context, _option, _value) (_context).checkError(curl_easy_setopt((_context).handle(), _option, _value), __LINE__ );

#define CURL_SETSTROPT(_context, _strlist, _option, _value) \
{ \
   COLstring* _colstr = new COLstring((_value));\
   (_strlist).push_back(_colstr);\
   (_context).checkError(curl_easy_setopt((_context).handle(), (_option), _colstr->c_str()), __LINE__); \
}

#define CURL_PERFORM(_context) _context.checkError(curl_easy_perform((_context).handle()), __LINE__);

void CURLcontext::clearOptions()
{
   COL_METHOD(CURLcontext::clearOptions);
   long Zero = 0;
   CURL_SETOPT(*this, CURLOPT_WRITEHEADER, Zero);
   CURL_SETOPT(*this, CURLOPT_HEADERFUNCTION, 0);
   CURL_SETOPT(*this, CURLOPT_HEADERDATA, 0);
   CURL_SETOPT(*this, CURLOPT_HEADER, 0);
   CURL_SETOPT(*this, CURLOPT_CUSTOMREQUEST, 0);
   CURL_SETOPT(*this, CURLOPT_QUOTE, 0);
   CURL_SETOPT(*this, CURLOPT_POSTQUOTE, 0);
   CURL_SETOPT(*this, CURLOPT_WRITEDATA, 0);
   CURL_SETOPT(*this, CURLOPT_WRITEFUNCTION, 0);
   CURL_SETOPT(*this, CURLOPT_READFUNCTION, 0);
   CURL_SETOPT(*this, CURLOPT_READDATA, 0);
   CURL_SETOPT(*this, CURLOPT_UPLOAD, Zero);
   CURL_SETOPT(*this, CURLOPT_NOBODY, Zero);
   CURL_SETOPT(*this, CURLOPT_FILETIME, Zero);
   CURL_SETOPT(*this, CURLOPT_DIRLISTONLY, Zero);
}

//--------------------------------------------------------------------------

static void CURLftpPrepareAuth(CURLcontext& Context, const CURLftpContext FtpContext, bool NoPassword)
{
   COL_FUNCTION(CURLftpPrepareAuth);
   COLstring AuthString = CURLftpBuildAuthString(FtpContext,NoPassword);
   COL_VAR(AuthString);
   if (!AuthString.is_null()) {
      CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_USERPWD, AuthString);
   }
}

static int parseMonth(const char* p)
{
   static const char *const names[] = {"Jan", "Feb", "Mar", "Apr",
      "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};
   for(int i=0; names[i]; i++)
      if(!strcmp(p, names[i])) return i + 1;
   return -1;
}

static int parseSpecial(struct ftpparse* fp, const char* pBuf, int Len)
{
   long Size;
   int Month, Day, Year, Hour, Minute, Start;
   char MonthName[4];
   if(sscanf(pBuf, "%ld %3s %d, %d %d:%d %n", &Size, MonthName,
      &Day, &Year, &Hour, &Minute, &Start) >= 6 && Start < Len
      && Size >= 0 && Day > 0 && Day <= 31 && Year >= 1970
      && Hour >= 0 && Hour < 24 && Minute >= 0 && Minute < 60
      && (Month = parseMonth(MonthName)) > 0)
   {
      struct tm Time = {0};
      bool IsDir = false;
      while(Start < Len && isspace((unsigned char)pBuf[Start])) Start++;
      while(Start < Len && isspace((unsigned char)pBuf[Len-1])) Len--;
      if(Start < Len && pBuf[Len-1] == '/') { IsDir = true; Len--; }
      if(Start >= Len) return 0;  // No filename.
      Time.tm_min  = Minute;
      Time.tm_hour = Hour;
      Time.tm_mday = Day;
      Time.tm_mon  = Month - 1;
      Time.tm_year = Year - 1900;
      fp->name        = pBuf + Start;
      fp->namelen     = Len - Start;
      fp->flagtrycwd  =  IsDir;
      fp->flagtryretr = !IsDir;
      fp->sizetype    = FTPPARSE_SIZE_UNKNOWN;
      fp->size        = Size;
      fp->mtimetype   = FTPPARSE_MTIME_LOCAL;  // Assumed.
      fp->mtime       = mktime(&Time);
      fp->idtype      = FTPPARSE_ID_UNKNOWN;
      fp->id          = NULL;
      fp->idlen       = 0;
      return 1;
   }
   return 0;
}

//---------------------------------------------------------------------------
void CURLftpExtractList(const COLstring& RawListing, COLarray<CURLftpListDirEntry>& Result)
{
   COL_FUNCTION(CURLftpExtractList);

   const char* pStart = RawListing.c_str();
   const char* pCurrent = pStart;

   do {
      switch (*pCurrent) {
         case '\0':
         case '\n':
         case '\r':
            if (pCurrent - pStart > 1)
            {
               const char* pLineStr = pStart;
               const COLuint32 LineLen = pCurrent - pStart;
               struct ftpparse ParseResult = {0};
               if (ftpparse(&ParseResult, pLineStr, LineLen) ||
                   parseSpecial(&ParseResult, pLineStr, LineLen))
               {
                  // TODO - ftpparse() may return FTPPARSE_MTIME_REMOTEDAY
                  // if it cannot determine the time-of-day for mtime.  We
                  // could try sending MDTM to get the the full timestamp.

                  CURLftpListDirEntry NewEntry;
                  if (ParseResult.mtimetype == FTPPARSE_MTIME_LOCAL) {
                     NewEntry.ModificationTime = ParseResult.mtime;
                  } else if (ParseResult.mtimetype != FTPPARSE_MTIME_UNKNOWN) {
                     // ftpparser() drops the time in as if UTC,
                     // but it's more likely to be localtime.
                     struct tm Time = {0}, *pTime = NULL;
#ifdef _WIN32
                     pTime = gmtime(&ParseResult.mtime);
#else
                     pTime = gmtime_r(&ParseResult.mtime, &Time);
#endif
                     if (pTime != NULL) {
                        pTime->tm_isdst = -1;  // Auto-detect.
                        // Some SFTP servers report 0 for ParseResult.mtime, which could cause
                        // mktime to return -1 if the system time zone >= UTC+1. See IX-3371
                        // Use the raw ParseResult.mtime in this case
                        time_t ModTime = mktime(pTime);
                        if (ModTime < 0) { ModTime = ParseResult.mtime; }
                        NewEntry.ModificationTime = ModTime;
                     }
                  }
                  NewEntry.FileName.write(ParseResult.name, ParseResult.namelen);
                  NewEntry.IsRetrievable = ParseResult.flagtryretr != 0;
                  Result.push_back(NewEntry);
               }
               else {
                  COL_ERR("Cannot parse file name from ftp list entry '" << COLstring(pLineStr, LineLen) << '\'');
                  // put the entire line in FileName and let the user parse it.
                  CURLftpListDirEntry NewEntry;
                  NewEntry.FileName.write(pLineStr, LineLen);
                  Result.push_back(NewEntry);
               }
            }
            pStart = pCurrent + 1;
            break;
         default:;
      }
   } while (*pCurrent++);
}

//---------------------------------------------------------------------------

CURLftp::CURLftp(COLuint32 iTimeoutInSeconds, bool DebugToStdErr) : m_pContext(0), m_TimeoutInSeconds(iTimeoutInSeconds), m_DebugToStdErr(DebugToStdErr)
{
   COL_METHOD(CURLftp::CURLftp);
   reset();
}

CURLftp::~CURLftp()
{
   COL_METHOD(CURLftp::~CURLftp);
   delete m_pContext;
   m_pContext = NULL;
}

void CURLftp::reset()
{
   COL_METHOD(CURLftp::reset);
   delete m_pContext;
   m_pContext = new CURLcontext(m_TimeoutInSeconds,m_DebugToStdErr);
   m_FtpContext = CURLftpContext();
}

void* CURLftp::initBasic(const COLstring& Protocol, const COLstring& Host, COLuint16 Port)
{
   COL_METHOD(CURLftp::initBasic);
   COL_VAR3(Protocol, Host, Port);
   reset();
   m_pContext->m_Protocol = Protocol;
   m_FtpContext.setHost(Host);
   m_FtpContext.setPort(Port);
   return m_pContext->handle();
}

void* CURLftp::curlHandle() {
   return m_pContext->handle();
}

void CURLftp::setUsername(const COLstring& User)
{
   COL_METHOD(CURLftp::setUsername);
   COL_VAR(User);
   CURL_SETOPT(*m_pContext, CURLOPT_USERNAME, User.c_str());
   m_FtpContext.setUsername(User);
}

void CURLftp::setPassword(const COLstring& Pass)
{
   COL_METHOD(CURLftp::setPassword);
   COL_VAR(Pass);
   CURL_SETOPT(*m_pContext, CURLOPT_PASSWORD, Pass.c_str());
   m_FtpContext.setPassword(Pass);
}

void CURLftp::initFtp(const CURLftpContext& FtpContext)
{
   COL_METHOD(CURLftp::initFtp);
   reset();
   m_FtpContext = FtpContext;
   m_pContext->m_Protocol = "ftp";
   CURLftpPrepareAuth(*m_pContext, FtpContext, false);
}

void CURLftp::initFtpSsl(const CURLftpContext& FtpContext, const COLstring& SslPrivateKeyFileName, const COLstring& SslCertificateFileName, bool VerifyPeer, const COLstring& SslCertificateAuthorityFileName)
{
   COL_METHOD(CURLftp::initFtpSsl);
   COL_VAR4(SslPrivateKeyFileName, SslCertificateFileName, VerifyPeer, SslCertificateAuthorityFileName);
   reset();
   m_FtpContext = FtpContext;
   m_pContext->m_Protocol = "ftps";

   CURLcontext& Context = *m_pContext;

   //always add PROT P since the server doesn't have to use
   //SSL for the data connection implicitly. This forces the
   //server to provide ssl encryption on the data channel
   Context.addQuote("PROT P");
   CURL_SETOPT(Context, CURLOPT_QUOTE, Context.quoteList());
   CURLftpPrepareAuth(Context, FtpContext, false);

   //TODO - at some point we may have to verify the hostname in the cert
   CURL_SETOPT(Context, CURLOPT_SSL_VERIFYHOST, 0);

   CURL_SETOPT(Context, CURLOPT_FTP_SSL, CURLFTPSSL_ALL);
   CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_SSLKEY, SslPrivateKeyFileName.c_str());
   CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_SSLCERT, SslCertificateFileName.c_str());
   CURL_SETOPT(Context, CURLOPT_SSL_VERIFYPEER, (VerifyPeer ? 1 : 0) );
   if (VerifyPeer)
   {
      CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_CAINFO, SslCertificateAuthorityFileName.c_str());
   }
}

static COLstring fixupMd5String(const COLstring& Input)
{
   //ensure that strings are exactly 32 length,
   //else curl will pretend the fingerprint doesn't matter....kinda silly
   COLstring Output(Input);
   Output.toLowerCase();
   while(Output.size() < 32){
      Output += ' ';
   }
   if (Output.size() > 32){
      Output = Output.substr(0,32);
   }
   return Output;
}

void CURLftp::initSecureShellFtp(const CURLftpContext& FtpContext, const COLstring& SslPrivateKeyFileName, const COLstring& SslPublicKeyFileName, bool VerifyHost, const COLstring& HostFingerprint)
{
   COL_METHOD(CURLftp::initSecureShellFtp);
   COL_VAR4(SslPrivateKeyFileName, SslPublicKeyFileName, VerifyHost, HostFingerprint);
   reset();
   m_FtpContext = FtpContext;
   m_pContext->m_Protocol = "sftp";

   CURLcontext& Context = *m_pContext;
   CURLftpPrepareAuth(Context, FtpContext, true);

   CURL_SETOPT(Context, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
   CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_SSH_PRIVATE_KEYFILE, SslPrivateKeyFileName.c_str());
   CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_SSH_PUBLIC_KEYFILE, SslPublicKeyFileName.c_str());
   if (VerifyHost){
      CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, fixupMd5String(HostFingerprint) );
   }
}

void CURLftp::initSecureShellFtpUserPassword(const CURLftpContext& FtpContext, bool VerifyHost, const COLstring& HostFingerprint)
{
   COL_METHOD(CURLftp::initSecureShellFtpUserPassword);
   COL_VAR2(VerifyHost, HostFingerprint);
   reset();
   m_FtpContext = FtpContext;
   m_pContext->m_Protocol = "sftp";

   CURLcontext& Context = *m_pContext;
   CURLftpPrepareAuth(Context, FtpContext, false);
   if (VerifyHost){
      CURL_SETSTROPT(Context, Context.contextStrings(), CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, fixupMd5String(HostFingerprint));
   }
}

bool CURLftpTranslateCode(CURL* pHandle, CURLcode InCode, CURLftpWriteHelper* pWriteHelper, CURLftpReadHelperBase* pReadHelper, CURLftp::eResult& OutCode)
{
   COL_FUNCTION(CURLftpTranslateCode);
   COL_VAR(InCode);

   switch(InCode) {
   case CURLE_OK:
      {
         long Response = 0;
         curl_easy_getinfo(pHandle, CURLINFO_RESPONSE_CODE, &Response);
         if (Response/100 == 4) {
             /* From the curl utility:
              * This is typically when the FTP server only allows a certain
              * amount of users and we are not one of them.  All 4xx codes
              * are transient.
              */
             OutCode = CURLftp::CURL_FTP_BUSY;
         }
         else {
            OutCode = CURLftp::CURL_FTP_OK;
         }
      }
      break;
   case CURLE_COULDNT_RESOLVE_HOST:
   case CURLE_COULDNT_CONNECT:
      OutCode = CURLftp::CURL_FTP_CANNOT_CONNECT;
      break;
   case CURLE_OPERATION_TIMEDOUT:
      OutCode = CURLftp::CURL_FTP_TIMEOUT;
      break;
   case  CURLE_ABORTED_BY_CALLBACK:
      if (pReadHelper) {
         COLerror Exception("",0);
         if (pReadHelper->getException(Exception)) {
            throw Exception;
         }
         OutCode = CURLftp::CURL_FTP_INTERRUPTED;
         break;
      }
      COL_VAR(OutCode);
      return false;
      break;
   case CURLE_WRITE_ERROR:
      if (pWriteHelper) {
         COLerror Exception("",0);
         if (pWriteHelper->getException(Exception)) {
            throw Exception;
         }
         OutCode = CURLftp::CURL_FTP_INTERRUPTED;
         break;
      }
      /*no break*/
   default:
      COL_VAR(OutCode);
      return false;
   }
   COL_VAR(OutCode);
   return true;
}

static size_t CURLftpWriteFailFunc(void* pBuffer, size_t Size, size_t Number, void* pExtra)
{
   COL_FUNCTION(CURLftpWriteFailFunc);
   COL_DUMP("ignore this data", pBuffer, Size*Number);
   return -1;
}

CURLftp::eResult CURLftp::checkForFileExistence(const COLarray<COLstring>& HeaderOutput)
{
   COL_METHOD(CURLftp::checkForFileExistence);
   const long One = 1;
   const long Zero = 0;
   CURLcontext& Context = *m_pContext;
   const COLstring ErrorString = "Could not check if file already exists on the server.";
   //assume url is already set up
   CURLftp::eResult FtpResult = CURL_FTP_OK;
   //check existence
   COL_VAR(m_pContext->m_Protocol);
   if (m_pContext->m_Protocol == "sftp") {
      //TODO - As of writing, curllib doesn't support the CURLOPT_NOBODY
      //method of checking file existence for sftp, so we actually have
      //to fetch the file, and just quit out of our write function if the file does exist
      CURL_SETOPT(Context, CURLOPT_WRITEFUNCTION, &CURLftpWriteFailFunc);
      CURLcode Result = curl_easy_perform(Context.handle());
      if (!CURLftpTranslateCode(Context.handle(), Result, 0, 0, FtpResult) || Result == CURLE_OK) {
         switch(Result) {
         case CURLE_OK:
         case CURLE_WRITE_ERROR:
            //file exists, we got here from our dummy function.
            return CURL_FTP_REMOTE_FILE_EXISTS;
            break;
         case CURLE_FTP_COULDNT_RETR_FILE:
         case CURLE_REMOTE_FILE_NOT_FOUND:
            //file doesn't exist
            return CURL_FTP_OK;
            break;
         default:
            //unknown error
            Context.checkError(Result,__LINE__,ErrorString, &HeaderOutput);
         }
      }
      //else some transient error
   }
   else {
      // ftp/ftps
      // Note that CURLOPT_NOBODY doesn't seem to work without FILETIME.
      // Need this to get the file info if it exists.
      // The filetime is not needed - just the callback has to be fired.
      // (Andrew: I didn't believe these next 3 lines were needed by curl,
      // but I was proven wrong. FTP put will not work without it. Odd.)
      void* UnusedInvalidPointer = (void*)0x1234567; // must be non-zero!!
      CURL_SETOPT(Context, CURLOPT_WRITEFUNCTION, &CURLftpWriteIgnoreFunc);
      CURL_SETOPT(Context, CURLOPT_WRITEDATA, UnusedInvalidPointer);

      CURL_SETOPT(Context, CURLOPT_NOBODY, One);
      CURL_SETOPT(Context, CURLOPT_FILETIME, One);
      CURLcode Result = curl_easy_perform(Context.handle());
      if (!CURLftpTranslateCode(Context.handle(), Result, 0, 0, FtpResult)) {
         switch(Result) {
         case CURLE_FTP_COULDNT_RETR_FILE:
         case CURLE_REMOTE_FILE_NOT_FOUND:
            //file doesn't exist
            return CURL_FTP_OK;
            break;
         default:
            //unknown error
            Context.checkError(Result,__LINE__,ErrorString, &HeaderOutput);
         }
      }
      else if (FtpResult == CURL_FTP_OK) {
         //file exists
         return CURL_FTP_REMOTE_FILE_EXISTS;
      }
   }
   return FtpResult;
}

//either file or data is used
CURLftp::eResult CURLftp::putFileImpl(const COLstring& LocalFileName, const void* pData,
   COLint32 DataSize, const COLstring& FtpPath, bool Overwrite, const COLstring& TempExtension)
{
   COL_METHOD(CURLftp::putFileImpl);
   COL_VAR3(LocalFileName, DataSize, FtpPath);
   COL_VAR2(Overwrite, TempExtension);
   TOptStringList TempOptStrings;
   const long One = 1;
   const long Zero = 0;

   if (LocalFileName.size()) {
      if (!FILfileExists(LocalFileName )){
         throw COLerror(COL_T("Put operation failed, file: ") + LocalFileName + COL_T(" does not exist."));
      }
   }
   else COLPRECONDITION(pData);

   CURLcontext& Context = *m_pContext;
   Context.clearOptions();

   //set up url
   COLstring CorrectedPath = FtpPath;
   CorrectedPath.replace("\\", "/");
   CorrectedPath.replace(" ", "%20");
   COLstring UrlString = CURLftpBuildUrl(m_pContext->m_Protocol, m_FtpContext, CorrectedPath);
   COL_VAR(UrlString);
   CURL_SETSTROPT(Context, TempOptStrings, CURLOPT_URL, UrlString);

   COLarray<COLstring> HeaderOutput;
   CURL_SETOPT(Context, CURLOPT_WRITEHEADER, One);
   CURL_SETOPT(Context, CURLOPT_HEADERFUNCTION, CURLftpWriteStringVectorFunc);
   CURL_SETOPT(Context, CURLOPT_HEADERDATA, &HeaderOutput);

   eResult ExistenceResult = CURL_FTP_OK;
   try{
      ExistenceResult = checkForFileExistence(HeaderOutput);
      switch( ExistenceResult ) {
      case CURL_FTP_OK:
         //file doesn't exist, continue
         break;
      case CURL_FTP_REMOTE_FILE_EXISTS:
         if (Overwrite) {
            break;
         }
      default:
         if (Overwrite) {
            //#21259 - since we're overwriting anyway, we can just try to do the upload
            //and ignore the error from trying to determine if the file
            //exists, and we assume the file doesn't exist
            ExistenceResult = CURL_FTP_OK;
            break;
         }
         //return error or if file exists or (not overwriting and any other error)
         return ExistenceResult;
      }
   }
   catch(const COLerror& Err){
      if (Overwrite){
         //See #21259
         ExistenceResult = CURL_FTP_OK;
      }
      else{
         throw;
      }
   }

   //file doesn't exist, or we're overwriting.
   //set up for the temp file rename scheme
   Context.clearPostQuoteList();
   if (TempExtension.size())
   {
      //change URL
      COLstring TempUploadPath = CorrectedPath;
      COLstring TempExtensionUrl = TempExtension;
      TempExtensionUrl.replace(" ", "%20");
      TempUploadPath += TempExtensionUrl;
      CURL_SETSTROPT(Context, TempOptStrings, CURLOPT_URL, CURLftpBuildUrl(m_pContext->m_Protocol, m_FtpContext, TempUploadPath));

      if ( ExistenceResult == CURL_FTP_REMOTE_FILE_EXISTS ) {
         COL_TRC("Deleting existing file before renaming");
         if (m_pContext->m_Protocol == "sftp") {
            Context.addPostQuote((COLstring("rm ") + CURLftpShellEscapeFilename(CorrectedPath)).c_str());
         } else {
            // DELE requires no %20 so go back to input path and grab file name (we did a CWD when we setup the context)
            COLstring Fix, Rest;
            Fix = FtpPath;
            Fix.replace("\\", "/");
            Fix.rsplit(Rest, Fix, "/");
            Context.addPostQuote((COLstring("DELE ") + Fix).c_str());
         }
      }
      if ( m_pContext->m_Protocol == "sftp" ) {
         COLstring RawPathWithTemp = CorrectedPath + TempExtension;
         RawPathWithTemp.replace("%20", " ");
         COLstring RawPath = CorrectedPath;
         RawPath.replace("%20", " ");
         Context.addPostQuote((COLstring("rename ") + CURLftpShellEscapeFilename(RawPathWithTemp) +
                              " " + CURLftpShellEscapeFilename(RawPath)).c_str());
      }
      else {
         COLstring TempUploadNoPath,Rest;
         TempUploadPath.rsplit(Rest,TempUploadNoPath,"/");
         TempUploadNoPath.replace("%20", " ");

         COLstring CorrectedUploadNoPath;
         CorrectedPath.rsplit(Rest,CorrectedUploadNoPath,"/");
         CorrectedUploadNoPath.replace("%20", " ");

         //at this point, we are in the directory we uploaded to, we assume we CWD'd here
         //so we don't include the full path info, because if its not absolute (and even if it is sometimes)
         //it won't work
         Context.addPostQuote((COLstring("RNFR ") + TempUploadNoPath).c_str());
         Context.addPostQuote((COLstring("RNTO ") + CorrectedUploadNoPath).c_str());
      }

      CURL_SETOPT(Context, CURLOPT_POSTQUOTE, Context.postQuoteList());
   }

   CURL_SETOPT(Context, CURLOPT_WRITEFUNCTION, 0);
   CURL_SETOPT(Context, CURLOPT_WRITEDATA, 0);
   CURL_SETOPT(Context, CURLOPT_NOBODY, Zero);
   CURL_SETOPT(Context, CURLOPT_FILETIME, Zero);

   COLauto<FILfile> pFile;
   COLauto<CURLftpReadHelperBase> pReadHelper;

   if (LocalFileName.size()){
      pFile = new FILfile(LocalFileName , FILfile::Read);
      pReadHelper = new CURLftpReadHelper(*pFile,this);
   }
   else{
      pReadHelper = new CURLftpBufferReadHelper(pData,DataSize,this);
   }
   CURL_SETOPT(Context, CURLOPT_READFUNCTION, &CURLftpReadHelperBase::curlReadFileFunc);
   CURL_SETOPT(Context, CURLOPT_READDATA, pReadHelper.get());

   CURL_SETOPT(Context, CURLOPT_UPLOAD, One);

   CURLcode Result = curl_easy_perform(Context.handle());
   eResult FtpResult = CURL_FTP_OK;
   if (!CURLftpTranslateCode(Context.handle(), Result, 0, pReadHelper, FtpResult)) {
      COLstring ErrorString;
      COLostream ErrStream(ErrorString);
      if (LocalFileName.size()){
         ErrStream << COL_T("Could not upload local file '") << LocalFileName << COL_T("' to remote path '")  << CorrectedPath<<  COL_T("'.");
      }
      else{
         ErrStream << COL_T("Could not upload data to remote path '")  << CorrectedPath<<  COL_T("'.");
      }
      Context.checkError(Result,__LINE__, ErrorString, &HeaderOutput);
   }
   if (FtpResult == CURL_FTP_OK && ExistenceResult == CURL_FTP_REMOTE_FILE_EXISTS) {
      FtpResult = CURL_FTP_OK_OVERWROTE_FILE;
   }
   return FtpResult;
}

CURLftp::eResult CURLftp::putFile(const COLstring& LocalFileName, const COLstring& FtpPath, bool Overwrite, const COLstring& TempExtension)
{
   COL_METHOD(CURLftp::putFile);
   COL_VAR4(LocalFileName, FtpPath, Overwrite, TempExtension);
   COLPRECONDITION(LocalFileName.size());
   return putFileImpl(LocalFileName, 0, 0, FtpPath,Overwrite,TempExtension);
}

CURLftp::eResult CURLftp::putData( const void* pData, COLint32 DataSize, const COLstring& FtpPath, bool Overwrite, const COLstring& TempExtension)
{
   COL_METHOD(CURLftp::putData);
   return putFileImpl("",pData,DataSize, FtpPath,Overwrite,TempExtension);
}

CURLftp::eResult CURLftp::listDirectory(const COLstring& FtpPath, COLarray<CURLftpListDirEntry>& Result, COLstring* pRawResult)
{
   COL_METHOD(CURLftp::listDirectory);
   TOptStringList TempOptStrings;
   CURLcontext& Context = *m_pContext;
   Context.clearOptions();

   const long One = 1;
   const long Zero = 0;
   COLstring CorrectedPath = FtpPath;
   CorrectedPath.replace("\\", "/");
   CorrectedPath.replace(" ", "%20");
   //always ensure that it ends with a slash, so we're doing a dir list
   //else sftp will try to fetch the file
   CorrectedPath.stripAll();
   if (CorrectedPath.size() && CorrectedPath[CorrectedPath.size()-1] != '/') {
      CorrectedPath += '/';
   }
   COLstring UrlString = CURLftpBuildUrl(m_pContext->m_Protocol, m_FtpContext, CorrectedPath);
   COL_VAR(UrlString);

   CURL_SETSTROPT(Context, TempOptStrings, CURLOPT_URL, UrlString);

   COLstring RawListing;
   COLsinkString RawListingSink(RawListing);
   CURLftpWriteHelper WriteHelper(RawListingSink,this);

   CURL_SETOPT(Context, CURLOPT_WRITEFUNCTION, CURLftpWriteFuncHelper);
   CURL_SETOPT(Context, CURLOPT_WRITEDATA, &WriteHelper);

   COLarray<COLstring> HeaderOutput;
   CURL_SETOPT(Context, CURLOPT_WRITEHEADER, One);
   CURL_SETOPT(Context, CURLOPT_HEADERFUNCTION, CURLftpWriteStringVectorFunc);
   CURL_SETOPT(Context, CURLOPT_HEADERDATA, &HeaderOutput);

   CURLcode ReturnCode = curl_easy_perform(Context.handle());

   COLstring ErrorString;
   COLostream ErrStream(ErrorString);
   ErrStream << COL_T("Could not get directory listing from remote path '") << CorrectedPath<<  '\'';

   eResult FtpResult = CURL_FTP_OK;
   if (!CURLftpTranslateCode(Context.handle(), ReturnCode, &WriteHelper, 0, FtpResult)) {
      Context.checkError(ReturnCode,__LINE__, ErrorString, &HeaderOutput);
   }

   if (m_DebugToStdErr){
      COLcerr << "CURLftp::listDirectory() RawListing" << newline << RawListing << newline;
   }

   //check for acceptable codes, to see if we should throw or return
   switch(FtpResult) {
   case CURL_FTP_OK:
      if (pRawResult) {
         RawListing.swap(pRawResult);
      } else {
         Result.clear();
         CURLftpExtractList(RawListing, Result);
      }
      /*no break*/
   case CURL_FTP_CANNOT_CONNECT:
   case CURL_FTP_BUSY:
   case CURL_FTP_TIMEOUT:
   case CURL_FTP_INTERRUPTED:
      break;
   default:
      //everything else is a problem.
      Context.checkError(ReturnCode,__LINE__, ErrorString, &HeaderOutput);
   }

   return FtpResult;
}

CURLftp::eResult CURLftp::getFile(const COLstring& FtpPath, COLsink& Result)
{
   COL_METHOD(CURLftp::getFile);
   COL_VAR(FtpPath);

   TOptStringList TempOptStrings;
   CURLcontext& Context = *m_pContext;
   Context.clearOptions();

   const long One = 1;
   const long Zero = 0;
   COLstring CorrectedPath = FtpPath;
   CorrectedPath.replace("\\", "/");
   CorrectedPath.replace(" ", "%20");
   COLstring UrlString = CURLftpBuildUrl(m_pContext->m_Protocol, m_FtpContext, CorrectedPath);
   COL_VAR(UrlString);

   CURLftpWriteHelper WriteHelper(Result,this);

   CURL_SETSTROPT(Context, TempOptStrings, CURLOPT_URL, UrlString);
   CURL_SETOPT(Context, CURLOPT_WRITEFUNCTION, &CURLftpWriteFuncHelper);
   CURL_SETOPT(Context, CURLOPT_WRITEDATA, &WriteHelper);

   COLarray<COLstring> HeaderOutput;
   CURL_SETOPT(Context, CURLOPT_WRITEHEADER, One);
   CURL_SETOPT(Context, CURLOPT_HEADERFUNCTION, CURLftpWriteStringVectorFunc);
   CURL_SETOPT(Context, CURLOPT_HEADERDATA, &HeaderOutput);

   CURLcode ReturnCode = curl_easy_perform(Context.handle());

   COLstring ErrorString;
   COLostream ErrStream(ErrorString);
   ErrStream << COL_T("Could not get file '") << CorrectedPath<<  '\'';

   eResult FtpResult = CURL_FTP_OK;
   if (!CURLftpTranslateCode(Context.handle(), ReturnCode, &WriteHelper, 0, FtpResult)) {
      Context.checkError(ReturnCode,__LINE__, ErrorString, &HeaderOutput);
   }
   //check for acceptable codes, to see if we should throw or return
   switch(FtpResult) {
   case CURL_FTP_OK:
   case CURL_FTP_CANNOT_CONNECT:
   case CURL_FTP_BUSY:
   case CURL_FTP_TIMEOUT:
   case CURL_FTP_INTERRUPTED:
      break;
   default:
      //everything else is a problem.
      Context.checkError(ReturnCode,__LINE__, ErrorString, &HeaderOutput);
   }
   return FtpResult;
}

CURLftp::eResult CURLftp::deleteFile(const COLstring& FilePath)
{
   COL_METHOD(CURLftp::deleteFile);
   COL_VAR(FilePath);

   TOptStringList TempOptStrings;
   CURLcontext& Context = *m_pContext;
   Context.clearOptions();

   COLstring UrlString = CURLftpBuildUrl(m_pContext->m_Protocol, m_FtpContext, "");
   COL_VAR(UrlString);
   CURL_SETSTROPT(Context, TempOptStrings, CURLOPT_URL, UrlString);
   COLstring CorrectedPath = FilePath;
   CorrectedPath.replace("\\", "/"); // no need to url encode spaces, we never use this to build a url. rm/DELE expect regular paths without encoding.
   //set up for delete
   long One = 1;
   CURL_SETOPT(Context, CURLOPT_NOBODY, One);
   Context.clearPostQuoteList();
   COL_VAR(m_pContext->m_Protocol);
   if (m_pContext->m_Protocol == "sftp") {
      Context.addPostQuote((COLstring("rm ") + CURLftpShellEscapeFilename(CorrectedPath)).c_str());
   } else {
      Context.addPostQuote((COLstring("DELE ") + CorrectedPath).c_str());
   }
   CURL_SETOPT(Context, CURLOPT_POSTQUOTE, Context.postQuoteList());

   COLarray<COLstring> HeaderOutput;
   CURL_SETOPT(Context, CURLOPT_WRITEHEADER, One);
   CURL_SETOPT(Context, CURLOPT_HEADERFUNCTION, CURLftpWriteStringVectorFunc);
   CURL_SETOPT(Context, CURLOPT_HEADERDATA, &HeaderOutput);

   CURLcode Result = curl_easy_perform(Context.handle());

   COLstring ErrorString;
   COLostream ErrStream(ErrorString);
   ErrStream << COL_T("Could not delete '") << CorrectedPath<<  '\'';

   eResult FtpResult = CURL_FTP_OK;
   if (!CURLftpTranslateCode(Context.handle(), Result, 0, 0, FtpResult)) {
      Context.checkError(Result,__LINE__, ErrorString, &HeaderOutput);
   }

   //check for acceptable codes, to see if we should throw or return
   switch(FtpResult)
   {
   case CURL_FTP_OK:
   case CURL_FTP_CANNOT_CONNECT:
   case CURL_FTP_BUSY:
   case CURL_FTP_TIMEOUT:
      break;
   default:
      //everything else is a problem.
      Context.checkError(Result,__LINE__, ErrorString, &HeaderOutput);
   }
   return FtpResult;
}

CURLftp::eResult CURLftp::renameFile(const COLstring& OrigPath, const COLstring& NewPath)
{
   COL_METHOD(CURLftp::renameFile);
   COL_VAR2(OrigPath, NewPath);

   TOptStringList TempOptStrings;
   CURLcontext& Context = *m_pContext;
   Context.clearOptions();

   COLstring UrlString = CURLftpBuildUrl(m_pContext->m_Protocol, m_FtpContext, "");
   COL_VAR(UrlString);
   CURL_SETSTROPT(Context, TempOptStrings, CURLOPT_URL, UrlString);

   COLstring CorrectedOrigPath = OrigPath;
   COLstring CorrectedNewPath = NewPath;
   CorrectedOrigPath.replace("\\", "/");
   CorrectedNewPath.replace("\\", "/");

   //set up for rename
   long One = 1;
   CURL_SETOPT(Context, CURLOPT_NOBODY, One);

   COLarray<COLstring> HeaderOutput;
   CURL_SETOPT(Context, CURLOPT_WRITEHEADER, One);
   CURL_SETOPT(Context, CURLOPT_HEADERFUNCTION, CURLftpWriteStringVectorFunc);
   CURL_SETOPT(Context, CURLOPT_HEADERDATA, &HeaderOutput);

   Context.clearPostQuoteList();
   COL_VAR(m_pContext->m_Protocol);
   if (m_pContext->m_Protocol == "sftp") {
      Context.addPostQuote((COLstring("rename ") + CURLftpShellEscapeFilename(CorrectedOrigPath) +
                            ' ' + CURLftpShellEscapeFilename(CorrectedNewPath)).c_str());
   } else {
      Context.addPostQuote((COLstring("RNFR ") + CorrectedOrigPath).c_str());
      Context.addPostQuote((COLstring("RNTO ") + CorrectedNewPath).c_str());
   }
   CURL_SETOPT(Context, CURLOPT_POSTQUOTE, Context.postQuoteList());

   CURLcode Result = curl_easy_perform(Context.handle());
   eResult FtpResult = CURL_FTP_OK;

   COLstring ErrorString;
   COLostream ErrStream(ErrorString);
   ErrStream << COL_T("Could not rename '") << CorrectedOrigPath << COL_T("' to '") << CorrectedNewPath << '\'';

   if (!CURLftpTranslateCode(Context.handle(), Result, 0, 0, FtpResult)) {
      Context.checkError(Result,__LINE__, ErrorString, &HeaderOutput);
   }

   //check for acceptable codes, to see if we should throw or return
   switch(FtpResult) {
   case CURL_FTP_OK:
   case CURL_FTP_CANNOT_CONNECT:
   case CURL_FTP_BUSY:
   case CURL_FTP_TIMEOUT:
      break;
   default:
      //everything else is a problem.
      Context.checkError(Result,__LINE__, ErrorString, &HeaderOutput);
   }
   return FtpResult;
}
