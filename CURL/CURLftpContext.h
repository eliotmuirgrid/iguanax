#ifndef __CURL_FTP_CONTEXT_H__
#define __CURL_FTP_CONTEXT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CURLftpContext
//
// Description:
//
// CURLftpContext class.
//
// Author: Matt Lawrence
// Date:   Sunday, November 8th, 2009 @ 01:57:29 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

class COL_DLL CURLftpContext
{
public:
   CURLftpContext() : m_Port(21) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~CURLftpContext() {}

   const COLstring& host() const       { return m_Host; }
   void setHost(const COLstring& Host) { m_Host = Host; }

   COLuint16 port() const       { return m_Port; }
   void setPort(COLuint16 Port) { m_Port = Port; }

   const COLstring& username() const           { return m_Username; }
   void setUsername(const COLstring& Username) { m_Username = Username; }

   const COLstring& password() const           { return m_Password; }
   void setPassword(const COLstring& Password) { m_Password = Password; }

private:
   COLstring m_Host;
   COLuint16 m_Port;

   COLstring m_Username;
   COLstring m_Password;
};

#endif // end of defensive include
