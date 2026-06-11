//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOserialize.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-24, 2:31p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOserialize.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
COL_LOG_MODULE;

// SAML namespace URIs
static auto SAML_NS_METADATA = "urn:oasis:names:tc:SAML:2.0:metadata";
static auto SAML_NS_DS       = "http://www.w3.org/2000/09/xmldsig#";

static bool SSOisPreSamlFormat(const COLvar& Json) {
   COL_FUNCTION(SSOisPreSamlFormat);
   return !Json.exists("mode");  // mode tells whether we have saml or client flow
}

static COLstring SSOsamlGetAttributeValue(const xmlNodePtr Node, const char* AttrName) {
   if(!Node) return COLstring();
   xmlChar* Value = xmlGetProp(Node, (const xmlChar*)AttrName);
   if(!Value) return COLstring();
   COLstring Result((const char*)Value);
   xmlFree(Value);
   return Result;
}

static bool SSOsamlParseMetadata(const COLstring& MetadataXml, SSOsamlConfig& Config, COLstring& Error) {
   COL_FUNCTION(SSOsamlParseMetadata);
   try {
      const xmlDocPtr Doc = xmlParseMemory(MetadataXml.c_str(), MetadataXml.size());
      if(!Doc) {
         Error = "Failed to parse SAML metadata XML";
         return false;
      }

      const xmlXPathContextPtr XpathCtx = xmlXPathNewContext(Doc);
      if(!XpathCtx) {
         Error = "Failed to create XPath context";
         xmlFreeDoc(Doc);
         return false;
      }

      xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"md", (const xmlChar*)SAML_NS_METADATA);
      xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"ds", (const xmlChar*)SAML_NS_DS);

      xmlXPathObjectPtr EntityDescObj = xmlXPathEvalExpression((const xmlChar*)"//md:EntityDescriptor", XpathCtx);
      if(!EntityDescObj || !EntityDescObj->nodesetval || EntityDescObj->nodesetval->nodeNr == 0) {
         if(EntityDescObj) xmlXPathFreeObject(EntityDescObj);
         EntityDescObj = xmlXPathEvalExpression((const xmlChar*)"/EntityDescriptor", XpathCtx);
         if(!EntityDescObj || !EntityDescObj->nodesetval || EntityDescObj->nodesetval->nodeNr == 0) {
            Error = "Invalid metadata: EntityDescriptor not found";
            if(EntityDescObj) xmlXPathFreeObject(EntityDescObj);
            xmlXPathFreeContext(XpathCtx);
            xmlFreeDoc(Doc);
            return false;
         }
      }

      const xmlNodePtr EntityDescNode = EntityDescObj->nodesetval->nodeTab[0];
      Config.m_IdpEntityId            = SSOsamlGetAttributeValue(EntityDescNode, "entityID");
      xmlXPathFreeObject(EntityDescObj);

      if(Config.m_IdpEntityId.is_null()) {
         Error = "Invalid metadata: entityID attribute not found";
         xmlXPathFreeContext(XpathCtx);
         xmlFreeDoc(Doc);
         return false;
      }

      xmlXPathObjectPtr CertObj = xmlXPathEvalExpression(
          (const xmlChar*)"//md:IDPSSODescriptor/md:KeyDescriptor[@use='signing' or not(@use)]/ds:KeyInfo/ds:X509Data/ds:X509Certificate/text()",
          XpathCtx);

      if(!CertObj || !CertObj->nodesetval || CertObj->nodesetval->nodeNr == 0) {
         if(CertObj) xmlXPathFreeObject(CertObj);
         CertObj = xmlXPathEvalExpression(
             (const xmlChar*)"//IDPSSODescriptor/KeyDescriptor[@use='signing' or not(@use)]/KeyInfo/X509Data/X509Certificate/text()",
             XpathCtx);
      }

      if(CertObj && CertObj->nodesetval && CertObj->nodesetval->nodeNr > 0) {
         const xmlNodePtr CertNode = CertObj->nodesetval->nodeTab[0];
         if(CertNode->content) {
            Config.m_Certificate = (const char*)CertNode->content;
            Config.m_Certificate = COLremoveAllWhiteSpace(Config.m_Certificate);
         }
      }
      if(CertObj) xmlXPathFreeObject(CertObj);

      if(Config.m_Certificate.is_null()) { COL_TRC("Warning: No certificate found in metadata"); }

      // Extract SingleSignOnService URL (prefer HTTP-Redirect binding)
      xmlXPathObjectPtr SsoRedirectObj = xmlXPathEvalExpression(
          (const xmlChar*)"//md:IDPSSODescriptor/md:SingleSignOnService[@Binding='urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect']",
          XpathCtx);

      if(!SsoRedirectObj || !SsoRedirectObj->nodesetval || SsoRedirectObj->nodesetval->nodeNr == 0) {
         if(SsoRedirectObj) xmlXPathFreeObject(SsoRedirectObj);
         SsoRedirectObj = xmlXPathEvalExpression(
             (const xmlChar*)"//IDPSSODescriptor/SingleSignOnService[@Binding='urn:oasis:names:tc:SAML:2.0:bindings:HTTP-Redirect']",
             XpathCtx);
      }

      if(SsoRedirectObj && SsoRedirectObj->nodesetval && SsoRedirectObj->nodesetval->nodeNr > 0) {
         Config.m_SsoLoginUrl = SSOsamlGetAttributeValue(SsoRedirectObj->nodesetval->nodeTab[0], "Location");
      }
      if(SsoRedirectObj) xmlXPathFreeObject(SsoRedirectObj);

      // If HTTP-Redirect not found, try HTTP-POST
      if(Config.m_SsoLoginUrl.is_null()) {
         xmlXPathObjectPtr SsoPostObj = xmlXPathEvalExpression(
             (const xmlChar*)"//md:IDPSSODescriptor/md:SingleSignOnService[@Binding='urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST']",
             XpathCtx);

         if(!SsoPostObj || !SsoPostObj->nodesetval || SsoPostObj->nodesetval->nodeNr == 0) {
            if(SsoPostObj) xmlXPathFreeObject(SsoPostObj);
            SsoPostObj = xmlXPathEvalExpression(
                (const xmlChar*)"//IDPSSODescriptor/SingleSignOnService[@Binding='urn:oasis:names:tc:SAML:2.0:bindings:HTTP-POST']",
                XpathCtx);
         }

         if(SsoPostObj && SsoPostObj->nodesetval && SsoPostObj->nodesetval->nodeNr > 0) {
            Config.m_SsoLoginUrl = SSOsamlGetAttributeValue(SsoPostObj->nodesetval->nodeTab[0], "Location");
         }
         if(SsoPostObj) xmlXPathFreeObject(SsoPostObj);
      }

      // Extract SingleLogoutService (optional)
      xmlXPathObjectPtr SloObj =
          xmlXPathEvalExpression((const xmlChar*)"//md:IDPSSODescriptor/md:SingleLogoutService", XpathCtx);

      // Try without namespace prefix if not found
      if(!SloObj || !SloObj->nodesetval || SloObj->nodesetval->nodeNr == 0) {
         if(SloObj) xmlXPathFreeObject(SloObj);
         SloObj = xmlXPathEvalExpression((const xmlChar*)"//IDPSSODescriptor/SingleLogoutService", XpathCtx);
      }

      if(SloObj && SloObj->nodesetval && SloObj->nodesetval->nodeNr > 0) {
         Config.m_SloLogoutUrl = SSOsamlGetAttributeValue(SloObj->nodesetval->nodeTab[0], "Location");
      }
      if(SloObj) xmlXPathFreeObject(SloObj);

      xmlXPathFreeContext(XpathCtx);
      xmlFreeDoc(Doc);

      if(Config.m_SsoLoginUrl.is_null()) {
         Error = "Invalid metadata: SingleSignOnService URL not found";
         return false;
      }

      COL_TRC("SAML metadata parsed successfully");
      COL_VAR2(Config.m_IdpEntityId, Config.m_SsoLoginUrl);
      COL_TRC("SLO Logout URL: " << (Config.m_SloLogoutUrl.is_null() ? "(not configured)" : Config.m_SloLogoutUrl));
      COL_TRC(
          "Certificate: " << (Config.m_Certificate.is_null() ? "(none)" : Config.m_Certificate.substr(0, 50) + "..."));
      return true;
   } catch(const COLerror& e) {
      Error = "Failed to parse SAML metadata: " + e.description();
      return false;
   }
}

static bool SSOloadClientConfig(const COLvar& Json, SSOconfig& Config) {
   COL_FUNCTION(SSOloadClientConfig);
   Config.m_ClientConfig.m_ClientId       = Json["client_id"];
   Config.m_ClientConfig.m_OrganizationId = Json["organization_id"];
   return true;
}

static bool SSOloadSamlConfig(const COLvar& Json, SSOconfig& Config, COLstring& Error) {
   COL_FUNCTION(SSOloadSamlConfig);
   Config.m_SamlConfig.m_EntityId     = Json["entity_id"];
   Config.m_SamlConfig.m_AcsUrl       = Json["acs_url"];
   Config.m_SamlConfig.m_MetadataPath = Json["metadata_path"];
   if(!FILfileExists(Config.m_SamlConfig.m_MetadataPath)) {
      Error = "SAML metadata file not found: " + Config.m_SamlConfig.m_MetadataPath;
      COL_VAR(Error);
      return false;
   }
   COL_TRC("Reading SAML metadata from: " << Config.m_SamlConfig.m_MetadataPath);
   COLstring MetadataXml;
   FILreadFile(Config.m_SamlConfig.m_MetadataPath, &MetadataXml);
   return SSOsamlParseMetadata(MetadataXml, Config.m_SamlConfig, Error);
}

static bool SSOparseConfig(const COLvar& Json, SSOconfig& Config, COLstring& Err) {
   COL_FUNCTION(SSOparseConfig);
   Config.m_Mode = SSOmodeToEnum(Json["mode"].asString());
   if(Config.m_Mode == DISABLED) { return true; }
   Config.m_Provider = SSOproviderToEnum(Json["provider"].asString());
   COL_VAR2(SSOmodeToString(Config.m_Mode), SSOproviderToString(Config.m_Provider));
   return Config.m_Mode == SAML ? SSOloadSamlConfig(Json, Config, Err) : SSOloadClientConfig(Json, Config);
}

static bool SSOparsePreSamlConfig(const COLvar& Json, SSOconfig& Config) {
   COL_FUNCTION(SSOparsePreSamlConfig);
   Config.m_Mode                          = CLIENT;
   Config.m_Provider                      = SSOproviderToEnum(Json["provider"].asString());
   Config.m_ClientConfig.m_ClientId       = Json["client_id"];
   Config.m_ClientConfig.m_OrganizationId = Json["organization_id"];
   return true;
}

bool SSOload(SSOconfig& Config, COLstring& Err) {
   COL_FUNCTION(SSOload);
   const COLstring SsoFile = DIRfileConfigSso();
   if(!FILfileExists(SsoFile)) {
      COL_TRC(SsoFile << " does not exist.");
      return true;
   }
   try {
      COLvar    Data;
      COLstring Raw;
      FILreadFile(SsoFile, &Raw);
      COL_VAR(Raw);
      if(!Data.parse(Raw) || !Data.isMap()) {
         Err = "Invalid SSO config format.";
         return false;
      }
      SSOclearConfig();
      return SSOisPreSamlFormat(Data) ? SSOparsePreSamlConfig(Data, Config) : SSOparseConfig(Data, Config, Err);
   } catch(const COLerror& e) {
      COL_TRC("Error reading SSO config: " << e.description());
      Err = e.description();
   }
   return false;
}

void SSOconfigToVar(const SSOconfig& Config, COLvar& ConfigVar) {
   COL_FUNCTION(SSOconfigToVar);
   ConfigVar.reset();
   ConfigVar.setMapType();
   ConfigVar["mode"] = SSOmodeToString(Config.m_Mode);
   if(Config.m_Mode == CLIENT) {
      ConfigVar["provider"]        = SSOproviderToString(Config.m_Provider);
      ConfigVar["client_id"]       = Config.m_ClientConfig.m_ClientId;
      ConfigVar["organization_id"] = Config.m_ClientConfig.m_OrganizationId;
   } else if(Config.m_Mode == SAML) {
      ConfigVar["provider"]      = SSOproviderToString(Config.m_Provider);
      ConfigVar["entity_id"]     = Config.m_SamlConfig.m_EntityId;
      ConfigVar["acs_url"]       = Config.m_SamlConfig.m_AcsUrl;
      ConfigVar["metadata_path"] = Config.m_SamlConfig.m_MetadataPath;
   }
   COL_VAR(ConfigVar.json(1));
}

void SSOconfigFromVar(const COLvar& Var, SSOconfig& Config) {
   COL_FUNCTION(SSOconfigFromVar);
   Config.m_Mode = SSOmodeToEnum(Var["mode"].asString());
   if(Config.m_Mode == DISABLED) { return SSOclearConfig(); }
   if(Config.m_Mode == CLIENT) {
      Config.m_ClientConfig.m_ClientId       = Var["client_id"];
      Config.m_ClientConfig.m_OrganizationId = Var["organization_id"];
   } else {
      Config.m_SamlConfig.m_EntityId     = Var["entity_id"];
      COLstring AcsUrl                   = Var["acs_url"].asString();

      const COLstring Suffix = "/saml/verify";
      const int SuffixLen = Suffix.size();

      // Remove trailing slashes
      while(AcsUrl.size() > 0 && AcsUrl[AcsUrl.size() - 1] == '/') { AcsUrl = AcsUrl.substr(0, AcsUrl.size() - 1); }
      // Remove existing /saml/verify if present
      if(AcsUrl.size() >= SuffixLen) {
         const COLstring End = AcsUrl.substr(AcsUrl.size() - SuffixLen);
         if(End == Suffix) { AcsUrl = AcsUrl.substr(0, AcsUrl.size() - SuffixLen); }
      }
      Config.m_SamlConfig.m_AcsUrl       = AcsUrl + Suffix;
      Config.m_SamlConfig.m_MetadataPath = Var["metadata_path"];
   }
   Config.m_Provider = SSOproviderToEnum(Var["provider"].asString());
}