#ifndef __IWEB_CONFIG_H__
#define __IWEB_CONFIG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBconfig
//
// Description:
//
// IWEBconfig class
//
// Author: Shuo Hou
// Date:   Monday, March 15th, 2021 @ 03:03:15 PM
//---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <DIR/DIRpath.h>
#include <GUID/GUIDid.h>

class IWEBconfig{
public:
   IWEBconfig(): m_WebPort("7654"), m_WebSessionTimeout(60 * 60), m_Guid(GUIDnewId()), m_ServerDescription(m_WebPort+" Server"),
   m_UseHttps(false), m_HttpsCertificateFile(DIRfileConfigWebCertRelative()), m_HttpsPrivateKeyFile(DIRfileConfigWebKeyRelative()) {}
   const COLstring& webPort() const { return m_WebPort; }   // TODO - this feels like it should be a number.
   void setWebPort(const COLstring& Port) { m_WebPort = Port; }

   int webSessionTimeout() const { return m_WebSessionTimeout; }  // In seconds.

   bool useHttps() const { return m_UseHttps; }
   void setUseHttps(bool Https) { m_UseHttps = Https; }

   const COLstring& httpsPrivateKeyFile() const { return m_HttpsPrivateKeyFile; }
   void setHttpsPrivateKeyFile(const COLstring& FilePath);

   const COLstring& httpsCertificateKeyFile() const { return m_HttpsCertificateFile; }
   void setHttpsCertificateKeyFile(const COLstring& FilePath);

   const COLstring& serverDescription() const { return m_ServerDescription; }
   void setServerDescription(const COLstring& NewValue) { m_ServerDescription = NewValue; }

   const COLstring& serverColour() const { return m_ServerColour; }
   void setServerColour(const COLstring& NewValue) { m_ServerColour = NewValue; }

   void setUniqueId(const COLstring& UniqueId) { m_Guid = UniqueId; };
   const COLstring& uniqueId() const { return m_Guid; };

private:
   COLstring m_WebPort;
   int       m_WebSessionTimeout;  // In seconds.
   bool      m_UseHttps;
   COLstring m_HttpsCertificateFile;
   COLstring m_HttpsPrivateKeyFile;
   COLstring m_ServerDescription;
   COLstring m_ServerColour;
   COLstring m_Guid;
};

COLostream& operator<<(COLostream& Stream, const IWEBconfig& Config);

#endif // end of defensive include
