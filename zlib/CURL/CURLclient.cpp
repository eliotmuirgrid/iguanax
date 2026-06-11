//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLclient
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, November 10th, 2005 @ 03:37:47 PM
//
//---------------------------------------------------------------------------
#include "CURLclient.h"

#include <curl/curl.h>
#include <COL/COLerror.h>
#include <CURL/CURLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


#define CURL_NO_RESPONSE_CODE 0

// this is a class to pass into onRequestData() callback function
// we need to keep somewhere the reading position in the Data
class CURLrequestData
{
public:
   CURLrequestData(const COLstring& aData) : Data(aData), ReadIndex(0) {}
   COLstring Data;
   COLuint32 ReadIndex;
};

class CURLclientPrivate
{
public:
   CURLclientPrivate();
   ~CURLclientPrivate();

   // return HTTP response code
   COLuint32 requestGet();
   COLuint32 requestPut(const COLstring& PutData);
   COLuint32 requestPost(const COLstring& PostData);

   // throws on error!
   void checkError(CURLcode Code);

   // callback to write (load) a response from a server
   static size_t onResponseData(void *pBuffer, size_t SizeOfItem,
      size_t NumberOfItem, void *UserPointer);

   // callback to read (upload) data into a request
   static size_t onRequestData(void *pBuffer, size_t SizeOfItem,
      size_t NumberOfItem, void *UserPointer);

   void setSslOptions();

   char   ErrorBuffer[CURL_ERROR_SIZE];
   COLstring Url;
   COLstring Proxy;
   COLstring CertificateFile;
   COLstring AuthorityFile;
   COLstring ResponseHeader;
   COLstring ResponseData;
   COLuint64 Timeout;
   CURL*     Handle;
};

CURLclientPrivate::CURLclientPrivate()
: Handle(0), Timeout(0)
{
   try
   {
      Handle = CURLeasyInit();
      checkError(curl_easy_setopt(Handle, CURLOPT_NOSIGNAL, 1));
      checkError(curl_easy_setopt(Handle, CURLOPT_ERRORBUFFER, ErrorBuffer));
      checkError(curl_easy_setopt(Handle, CURLOPT_SSL_VERIFYHOST, 0));
      COL_VAR(CURLdefaultCaBundlePath());
      checkError(curl_easy_setopt(Handle, CURLOPT_CAINFO, CURLdefaultCaBundlePath().c_str()));
   }
   catch (const COLerror& Error)
   {
      curl_easy_cleanup(Handle);
      Handle = NULL;
      throw Error;
   }
}

CURLclientPrivate::~CURLclientPrivate()
{
   curl_easy_cleanup(Handle);
   Handle = NULL;
}

COLuint32 CURLclientPrivate::requestGet()
{
   ResponseData.clear();
   ResponseHeader.clear();
   // set method
   checkError(curl_easy_setopt(Handle, CURLOPT_HTTPGET, 1));
   // set some common options
   checkError(curl_easy_setopt(Handle, CURLOPT_URL, Url.c_str()));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEFUNCTION, &onResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEDATA, &ResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_HEADERFUNCTION, &onResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEHEADER, &ResponseHeader));
   // SSL staff
   setSslOptions();
   // perform request
   checkError(curl_easy_perform(Handle));
   long ResponseCode = CURL_NO_RESPONSE_CODE;
   checkError(curl_easy_getinfo(Handle, CURLINFO_RESPONSE_CODE, &ResponseCode));
   return ResponseCode;
}

COLuint32 CURLclientPrivate::requestPut(const COLstring& PutData)
{
   ResponseData.clear();
   ResponseHeader.clear();
   CURLrequestData Data(PutData);
   // set method
   checkError(curl_easy_setopt(Handle, CURLOPT_UPLOAD, 1));
   checkError(curl_easy_setopt(Handle, CURLOPT_INFILE,   &Data));
   checkError(curl_easy_setopt(Handle, CURLOPT_READFUNCTION, &onRequestData));
   checkError(curl_easy_setopt(Handle, CURLOPT_INFILESIZE, PutData.size()));
   // set some common options
   checkError(curl_easy_setopt(Handle, CURLOPT_URL, Url.c_str()));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEFUNCTION, &onResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEDATA, &ResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_HEADERFUNCTION, &onResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEHEADER, &ResponseHeader));
   // SSL staff
   setSslOptions();
   // perform request
   checkError(curl_easy_perform(Handle));
   long ResponseCode = CURL_NO_RESPONSE_CODE;
   checkError(curl_easy_getinfo(Handle, CURLINFO_RESPONSE_CODE, &ResponseCode));
   return ResponseCode;
}

COLuint32 CURLclientPrivate::requestPost(const COLstring& PostData)
{
   ResponseData.clear();
   ResponseHeader.clear();
   // set method
   checkError(curl_easy_setopt(Handle, CURLOPT_POST, 1));
   // set some common options
   checkError(curl_easy_setopt(Handle, CURLOPT_URL, Url.c_str()));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEFUNCTION, &onResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEDATA, &ResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_HEADERFUNCTION, &onResponseData));
   checkError(curl_easy_setopt(Handle, CURLOPT_WRITEHEADER, &ResponseHeader));
   // SSL staff
   setSslOptions();
   // perform request
   checkError(curl_easy_perform(Handle));
   long ResponseCode = CURL_NO_RESPONSE_CODE;
   checkError(curl_easy_getinfo(Handle, CURLINFO_RESPONSE_CODE, &ResponseCode));
   return ResponseCode;
}

void CURLclientPrivate::setSslOptions()
{
   if (CertificateFile.size())
   {
      checkError(curl_easy_setopt(Handle, CURLOPT_SSLCERT, CertificateFile.c_str()));
   }
   else
   {
      checkError(curl_easy_setopt(Handle, CURLOPT_SSLCERT, NULL));
   }

   if (AuthorityFile.size())
   {
      checkError(curl_easy_setopt(Handle, CURLOPT_CAINFO, AuthorityFile.c_str()));
   }

   checkError(curl_easy_setopt(Handle, CURLOPT_SSL_VERIFYPEER, 1));
}

void CURLclientPrivate::checkError(CURLcode Code)
{
   if (Code != (CURLcode)CURLclient::Ok)
   {
      COLstring Description("CURL library error: ");
      Description += ErrorBuffer;
//      Description += curl_easy_strerror(Code);
      COL_ERROR_STREAM(Description, Code);
   }
}

size_t CURLclientPrivate::onResponseData(void *pBuffer, size_t SizeOfItem,
                                         size_t NumberOfItem, void *UserPointer)
{
   COLuint32 Size = SizeOfItem * NumberOfItem;
   reinterpret_cast<COLstring*>(UserPointer)->write(pBuffer, Size);
   return Size;
}

size_t CURLclientPrivate::onRequestData(void *pBuffer, size_t SizeOfItem,
                                        size_t NumberOfItem, void *UserPointer)
{
   COLuint32 Size = SizeOfItem * NumberOfItem;
   CURLrequestData* pData = reinterpret_cast<CURLrequestData*>(UserPointer);
   if(Size + pData->ReadIndex > pData->Data.size())
   {
      Size = pData->Data.size() - pData->ReadIndex;
   }
   if (Size > 0)
   {
      memcpy(pBuffer, pData->Data.c_str() + pData->ReadIndex, Size);
      pData->ReadIndex += Size;
   }
   return Size;
}

CURLclient::CURLclient()
{
   pMember = new CURLclientPrivate;
}

CURLclient::~CURLclient()
{
   delete pMember;
}

// old implementation
COLuint32 CURLclient::sendRequest(const COLstring& Url, COLstring& ResponseData)
{
   pMember->checkError(curl_easy_setopt(pMember->Handle, CURLOPT_WRITEFUNCTION, &CURLclientPrivate::onResponseData));
   pMember->checkError(curl_easy_setopt(pMember->Handle, CURLOPT_WRITEDATA, &ResponseData));
   pMember->checkError(curl_easy_setopt(pMember->Handle, CURLOPT_URL, Url.c_str()));
   pMember->checkError(curl_easy_perform(pMember->Handle));

   // ResponseCode must be initialized before passing pointer into curl_easy_getinfo
   long ResponseCode = CURL_NO_RESPONSE_CODE;
   pMember->checkError(curl_easy_getinfo(pMember->Handle, CURLINFO_RESPONSE_CODE, &ResponseCode));
   return ResponseCode;
}

COLuint32 CURLclient::requestGet()
{
   COLPRECONDITION(pMember->Url != "");
   return pMember->requestGet();
}

COLuint32 CURLclient::requestPut(const COLstring& UploadData)
{
   COLPRECONDITION(pMember->Url != "");
   return pMember->requestPut(UploadData);
}

COLuint32 CURLclient::requestPost(const COLstring& PostData)
{
   COLPRECONDITION(pMember->Url != "");
   return pMember->requestPost(PostData);
}

void CURLclient::setUrl(const COLstring& Url)
{
   pMember->Url = Url;
}

const COLstring& CURLclient::url() const
{
   return pMember->Url;
}

void CURLclient::setSslOptions(const COLstring& CertificateFile, const COLstring& AuthorityFile)
{
   pMember->CertificateFile = CertificateFile;
   pMember->AuthorityFile   = AuthorityFile;
}

const COLstring& CURLclient::responseHeader() const
{
   return pMember->ResponseHeader;
}

const COLstring& CURLclient::responseContent() const
{
   return pMember->ResponseData;
}

void CURLclient::setTimeout(COLuint64 Seconds)
{
   pMember->Timeout = Seconds;
   pMember->checkError(curl_easy_setopt(pMember->Handle, CURLOPT_TIMEOUT, Seconds));
}

COLuint64 CURLclient::timeout()
{
   return pMember->Timeout;
}

void CURLclient::setProxy(COLstring Proxy)
{
   pMember->Proxy = Proxy;
   pMember->checkError(curl_easy_setopt(pMember->Handle, CURLOPT_PROXY, Proxy.c_str()));
}

COLstring CURLclient::proxy()
{
   return pMember->Proxy;
}
