#ifndef __CURL_FTP_H__
#define __CURL_FTP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLftp
//
// Description:
//
// CURLftp class.
//
// Author: Matt Lawrence
// Date:   Sunday, November 8th, 2009 @ 01:16:09 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <COL/COLarray.h>
#include <COL/COLdateTime.h>
#include <SIG/SIGsignaller2.h>

#include "CURLftpContext.h"

class CURLcontext;

// POD - allow default copy constructor and assignment operator.
class CURLftpListDirEntry
{
public:
   CURLftpListDirEntry() : IsRetrievable(false)
   {
   }
   COLstring FileName;
   COLdateTime ModificationTime;
   bool IsRetrievable;
};

#define CURL_FTP_DEFAULT_TIMEOUT 60

class CURLftp
{
public:
   CURLftp( COLuint32 TimeoutInSeconds = CURL_FTP_DEFAULT_TIMEOUT, bool DebugToStdErr = false);
   ~CURLftp();

   enum eResult
   {
      CURL_FTP_OK,
      CURL_FTP_OK_OVERWROTE_FILE,
      CURL_FTP_CANNOT_CONNECT,
      CURL_FTP_TIMEOUT,
      CURL_FTP_REMOTE_FILE_EXISTS,
      CURL_FTP_INTERRUPTED,
      CURL_FTP_BUSY
   };

   // This is a more basic API, where no CURLftpContext object is used, and
   // nothing much is done.  Returns a CURL*, and you can set stuff yourself.
   //
   void* initBasic(const COLstring& Protocol,  // ftp, sftp, or ftps.
                   const COLstring& Host,
                   COLuint16 Port);

   // Another way to get the CURL* handle.
   void* curlHandle();

   // When using initBasic(), set the username/password via these, so the
   // internal CURLftpContext object can be updated.
   //
   void setUsername(const COLstring&);
   void setPassword(const COLstring&);

   //each one of these puts things into a different mode
   void initFtp(const CURLftpContext& Context);

   //Note, key shouldnt be password protected!
   void initFtpSsl(const CURLftpContext& Context,
                   const COLstring& SslPrivateKeyFileName,
                   const COLstring& SslCertificateFileName,
                   bool VerifyPeer,
                   const COLstring& SslCertificateAuthorityFileName);

   void initSecureShellFtp(const CURLftpContext& FtpContext,
                           const COLstring& SslPrivateKeyFileName,
                           const COLstring& SslPublicKeyFileName,
                           bool VerifyHost,
                           const COLstring& HostFingerprint);
   void initSecureShellFtpUserPassword(
      const CURLftpContext& FtpContext,
      bool VerifyHost,
      const COLstring& HostFingerprint);

   //note that FtpPath is the full path to the target file name
   //if temp extension is null, then upload then rename scheme is not used
   //putData uploads plain data as the remote file
   eResult putFile( const COLstring& LocalFileName, const COLstring& FtpPath, bool Overwrite, const COLstring& TempExtension);
   eResult putData( const void* pData, COLint32 DataSize, const COLstring& FtpPath, bool Overwrite, const COLstring& TempExtension);

   //returns ok, cannot connect, timeout, interrupted, or busy
   //if 'pRawResult' is set, it's populated with the raw list output. Otherwise, the output is parsed and stored in 'Results'.
   //throws on everything else
   eResult listDirectory(const COLstring& FtpPath, COLarray<CURLftpListDirEntry>& Results, COLstring* pRawResult = NULL);

   //returns ok, cannot connect, timeout, interrupted, or busy
   //throws on everything else
   eResult getFile(const COLstring& FtpPath, // path to the desired file relative to the root
                   COLsink& Sink);

   //returns ok, cannot connect, timeout, interrupted, or busy
   //throws on everything else
   eResult deleteFile(const COLstring& FilePath);

   //returns ok, cannot connect, timeout, interrupted, or busy
   //throws on everything else
   eResult renameFile(const COLstring& OrigPath, const COLstring& NewPath);

   //signal is called during transfer, return true in the parameter
   //to continue, false will cancel the transfer, resulting in CURL_FTP_INTERRUPTED
   //being returned
   //
   // #15988 #16066 - We cannot provide this callback as libcurl doesn't
   // always clean up everything when we interrupt it, leading to leaks.
   //
   //SIGsignaller2<CURLftp*, bool& /*Return*/> OnContinueOperation;

private:
   eResult putFileImpl( const COLstring& LocalFileName,
                        const void* pData,
                        COLint32 DataSize,
                        const COLstring& FtpPath,
                        bool Overwrite,
                        const COLstring& TempExtension);
   void reset();
   eResult checkForFileExistence(const COLarray<COLstring>& HeaderOutput);
   CURLcontext* m_pContext;
   CURLftpContext m_FtpContext;
   COLuint32 m_TimeoutInSeconds;
   bool m_DebugToStdErr;
   CURLftp& operator=(const CURLftp&);
   CURLftp(const CURLftp&);
};

// for unit testing
void CURLftpExtractList(const COLstring& RawListing, COLarray<CURLftpListDirEntry>& Result);

#endif // end of defensive include
