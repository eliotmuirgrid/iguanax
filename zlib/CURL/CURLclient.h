#ifndef __CURL_CLIENT_H__
#define __CURL_CLIENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLclient
//
// Description:
//
// This is a URL client class, used to make request to a server.  So far
// it has only been used with (but is not limited to) http.
//
// Example:
//
// CURLclient Client;
// COLstring Response;
// COLuint64 HTTPResponseCode = Client.sendRequest("http://www.google.ca", Response);
// if (HTTPResponseCode == CURLclient::HttpStatusOk)
// {
//    COLcout << Response;
// }
//
// Author: Kevin Senn
// Date:   Thursday, November 10th, 2005 @ 03:37:47 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>
#include <curl/curl.h>

class CURLclientPrivate;

class COL_DLL CURLclient
{
public:
   // Error codes which can be thrown in the CURLclient constructor,
   // or the sendRequest method.  If an error is thrown, it will
   // also be thrown with a textual description of the error, obtained
   // from CURLerrorCodeToString (CURLutils.h).  For more information
   // on what each of these error codes mean, see curllib\libcurl-errors.html
   enum CURLerrorCode
   {
      Ok                       = CURLE_OK,
      UnsupportedProtocol      = CURLE_UNSUPPORTED_PROTOCOL,
      FailedInit               = CURLE_FAILED_INIT,
      UrlMalformat             = CURLE_URL_MALFORMAT,
      UrlMalformatUser         = CURLE_URL_MALFORMAT_USER,
      CouldntResolveProxy      = CURLE_COULDNT_RESOLVE_PROXY,
      CouldntResolveHost       = CURLE_COULDNT_RESOLVE_HOST,
      CouldntConnect           = CURLE_COULDNT_CONNECT,
      FtpWeirdServerReply      = CURLE_FTP_WEIRD_SERVER_REPLY,
      FtpAccessDenied          = CURLE_FTP_ACCESS_DENIED,
      FtpUserPasswordIncorrect = CURLE_FTP_USER_PASSWORD_INCORRECT,
      FtpWeirdPassReply        = CURLE_FTP_WEIRD_PASS_REPLY,
      FtpWeirdUserReply        = CURLE_FTP_WEIRD_USER_REPLY,
      FtpWeirdPasvReply        = CURLE_FTP_WEIRD_PASV_REPLY,
      FtpWeird227Format        = CURLE_FTP_WEIRD_227_FORMAT,
      FtpCantGetHost           = CURLE_FTP_CANT_GET_HOST,
      FtpCantReconnect         = CURLE_FTP_CANT_RECONNECT,
      FtpCouldntSetBinary      = CURLE_FTP_COULDNT_SET_BINARY,
      PartialFile              = CURLE_PARTIAL_FILE,
      FtpCouldntRetrFile       = CURLE_FTP_COULDNT_RETR_FILE,
      FtpWriteError            = CURLE_FTP_WRITE_ERROR,
      FtpQuoteError            = CURLE_FTP_QUOTE_ERROR,
      HttpReturnedError        = CURLE_HTTP_RETURNED_ERROR,
      WriteError               = CURLE_WRITE_ERROR,
      MalformatUser            = CURLE_MALFORMAT_USER,
      FtpCouldntStorFile       = CURLE_FTP_COULDNT_STOR_FILE,
      ReadError                = CURLE_READ_ERROR,
      OutOfMemory              = CURLE_OUT_OF_MEMORY,
      OperationTimeouted       = CURLE_OPERATION_TIMEOUTED,
      FtpCouldntSetAscii       = CURLE_FTP_COULDNT_SET_ASCII,
      FtpPortFailed            = CURLE_FTP_PORT_FAILED,
      FtpCouldntUseRest        = CURLE_FTP_COULDNT_USE_REST,
      FtpCouldntGetSize        = CURLE_FTP_COULDNT_GET_SIZE,
      HttpRangeError           = CURLE_HTTP_RANGE_ERROR,
      HttpPostError            = CURLE_HTTP_POST_ERROR,
      SslConnectError          = CURLE_SSL_CONNECT_ERROR,
      FtpBadDownloadResume     = CURLE_FTP_BAD_DOWNLOAD_RESUME,
      FileCouldntReadFile      = CURLE_FILE_COULDNT_READ_FILE,
      LdapCannotBind           = CURLE_LDAP_CANNOT_BIND,
      LdapSearchFailed         = CURLE_LDAP_SEARCH_FAILED,
      LibraryNotFound          = CURLE_LIBRARY_NOT_FOUND,
      FunctionNotFound         = CURLE_FUNCTION_NOT_FOUND,
      AbortedByCallback        = CURLE_ABORTED_BY_CALLBACK,
      BadFunctionArgument      = CURLE_BAD_FUNCTION_ARGUMENT,
      BadCallingOrder          = CURLE_BAD_CALLING_ORDER,
      HttpPortFailed           = CURLE_HTTP_PORT_FAILED,
      BadPasswordEntered       = CURLE_BAD_PASSWORD_ENTERED,
      TooManyRedirects         = CURLE_TOO_MANY_REDIRECTS,
      UnknownTelnetOption      = CURLE_UNKNOWN_TELNET_OPTION,
      TelnetOptionSyntax       = CURLE_TELNET_OPTION_SYNTAX,
      Obsolete                 = CURLE_OBSOLETE,
      SslPeerCertificate       = CURLE_SSL_PEER_CERTIFICATE,
      GotNothing               = CURLE_GOT_NOTHING,
      SslEngineNotFound        = CURLE_SSL_ENGINE_NOTFOUND,
      SslEngineSetFailed       = CURLE_SSL_ENGINE_SETFAILED,
      SendError                = CURLE_SEND_ERROR,
      RecvError                = CURLE_RECV_ERROR,
      ShareInUse               = CURLE_SHARE_IN_USE,
      SslCertProblem           = CURLE_SSL_CERTPROBLEM,
      SslCipher                = CURLE_SSL_CIPHER,
      SslCacert                = CURLE_SSL_CACERT,
      BadContentEncoding       = CURLE_BAD_CONTENT_ENCODING,
      LdapInvalidUrl           = CURLE_LDAP_INVALID_URL,
      FileSizeExceeded         = CURLE_FILESIZE_EXCEEDED,
      FtpSslFailed             = CURLE_FTP_SSL_FAILED,
      SendFailRewind           = CURLE_SEND_FAIL_REWIND,
      SslEngingeInitFailed     = CURLE_SSL_ENGINE_INITFAILED,
      LoginDenied              = CURLE_LOGIN_DENIED
   };

   // HTTP response codes.  If performing an HTTP request, the code returned
   // through the ResponseCode argument of sendRequest should be one of the
   // following values.  These are HTTP 1.0 standard status codes.
   enum CURLhttpResponseCode
   {
      HttpStatusOk                 = 200,
      HttpStatusCreated            = 201,
      HttpStatusAccepted           = 202,
      HttpStatusNoContent          = 204,
      HttpStatusMovedPerm          = 301,
      HttpStatusFound              = 302,
      HttpStatusNotModified        = 304,
      HttpStatusBadRequest         = 400,
      HttpStatusUnauthorized       = 401,
      HttpStatusForbidden          = 403,
      HttpStatusNotFound           = 404,
      HttpStatusInternalError      = 500,
      HttpStatusNotImplemented     = 501,
      HttpStatusBadGateway         = 502,
      HttpStatusServiceUnavailable = 503
   };

   CURLclient();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~CURLclient();

   // Send this request and returns the response by writing to the Sink.  The response
   // code will be returned.  Throws on errors.
   // NOTE: If there is already something in the Sink (for example, if it is a
   // COLstring with a previous value "hello world", the response will simply be
   // appended to any pre-existing values - "hello world<html><head>..."
   COLuint32 sendRequest(const COLstring& Url, COLstring& Sink);

   // The set of the "GET", "PUT" and "POST" methods will use URL and SSL options
   // to make a request to URL over HTTP or HTTPS
   // These methods throws COLerror in case of failure

   // some data (key=value pairs) can be attached to the URL
   COLuint32 requestGet();

   // UploadData - data to upload to a server
   COLuint32 requestPut(const COLstring& UploadData);

   // PostData - the pairs "key-value" to post to a server
   COLuint32 requestPost(const COLstring& PostData);

   // set / get functions for URL
   void setUrl(const COLstring& Url);
   const COLstring& url() const;

   // set SSL Certificate and Authority files for using HTTPS
   // if was not set - use usual HTTP
   void setSslOptions(const COLstring& CertificateFile, const COLstring& AuthorityFile);

   // after invoking of the HTTP methods returns header from response
   const COLstring& responseHeader() const;

   // after invoking of the HTTP methods returns content (body) from response
   const COLstring& responseContent() const;

   // Set the time-out value (in seconds) for this client.
   // A timeout of 0 indicates no timeout.
   // By default there will be no time-out.
   void setTimeout(COLuint64 Seconds);

   // Get the time-out value (in seconds) for this client.
   // A timeout of 0 indicates no timeout.
   COLuint64 timeout();

   // Set the proxy (in the format "hostname", "XXX.XXX.XXX.XXX",
   // "hostname:YY" or "XXX.XXX.XXX.XXX:YY",
   // where YY is the port number).
   void setProxy(COLstring Proxy);

   // Get the proxy for the client
   COLstring proxy();


private:
   CURLclientPrivate* pMember;
   CURLclient(const CURLclient& Orig); // not allowed
   CURLclient& operator=(const CURLclient& Orig); // not allowed
};

#endif // end of defensive include
