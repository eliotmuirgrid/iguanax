//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: IWEBserver.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLmath.h>
#include <COL/COLutils.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBiguanaUrl.h>
#include <IWEB/IWEBsave.h>
#include <IWEBA/IWEBserver.h>
#include <LINK/LINKconstants.h>
#include <POP/POPcolor.h>
#include <PRO/PROweb.h>
#include <SDB/SDBcapi.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

void IWEBlog(const COLstring& Log) {
   COL_FUNCTION(IWEBlog);
   COL_VAR(Log);
   COLcout << Log << newline;
   SDBIlogService(Log, "#startup");
}

void IWEBlogError(const COLstring& Log) {
   COL_FUNCTION(IWEBlogError);
   COL_VAR(Log);
   COLcout << Log << newline;
   SDBIlogService(Log, "#error #startup");
}

void IWEBopenSetWebPortHelp() {
   COL_FUNCTION(IWEBopenSetWebPortHelp);
   const COLstring Link = LINKiwebPortChange;
   if(!PROweb(Link)) { COLcout << "See: " << POPpop(Link) << newline; }
}

// TODO - switch to using pointers
bool IWEBserverInit(WEBserver& WEBserver, IWEBconfig& WebConfig, bool Quiet, COLstring* pError) {
   COL_FUNCTION(IWEBserverInit);
   COLstring HttpsCertificateFile   = FILpathResolveEnvironment(WebConfig.httpsCertificateKeyFile());
   COLstring HttpsPrivateKeyFile    = FILpathResolveEnvironment(WebConfig.httpsPrivateKeyFile());
   COLstring ConfigurationDirectory = FILexecutableDir();

   if(WebConfig.useHttps()) {
      if(!Quiet) { IWEBlog("HTTPS is enabled - trying to set up SSL context."); }
      if(WEBserver.m_SslContext.setCertificate(HttpsCertificateFile) != 0) {
         IWEBlogError("Not able to use HTTPS for web interface. Failed to load certificate file:\n" +
                      HttpsCertificateFile);
         WebConfig.setUseHttps(false);
         IWEBsave(WebConfig);
      }
      if(WEBserver.m_SslContext.setPrivateKey(HttpsPrivateKeyFile) != 0) {
         IWEBlogError("Not able to use HTTPS for web interface. Failed to load private key file:\n" +
                      HttpsPrivateKeyFile);
         WebConfig.setUseHttps(false);
         IWEBsave(WebConfig);
      }
   }
   // load ok, we can start now
   const COLstring& PortStr   = COLexpandEnvironmentVariables(WebConfig.webPort());
   const COLuint16  PortShort = COLmath::atous(PortStr.c_str());
   if(PortShort <= 1) {
      *pError = "Need to define valid port for web server interface.";
      return false;
   }

   COL_VAR2(PortStr, PortShort);
   // WEBserver starts here
   if(!WEBserver.start(PortShort, "", WebConfig.useHttps())) {
      COLstring ErrorMessage = "Failed to listen on port " + COLintToString(PortShort) +
                               ".\nCheck to see if another instance of IguanaX is running.";
      SDBIlogService(ErrorMessage, "#error #startup");
      COLostream ErrorStream(ErrorMessage);
      IWEBportChangeInfo(ErrorStream);
      *pError = ErrorMessage;
      return false;
   }
   IWEBlog(COLstring("Listening on ") + IWEBiguanaUrl(WebConfig));
   return true;
}
