//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlVerify.cpp
//
//  Description: SAML signature verification and validation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SSO/SSOsamlVerify.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <xmlsec/openssl/app.h>
#include <xmlsec/openssl/crypto.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/xmlsec.h>
#include <xmlsec/xmltree.h>

#include <ctime>
COL_LOG_MODULE;

COLstring SSOsamlFormatCertificateToPem(const COLstring& RawCert) {
   COL_FUNCTION(SSOsamlFormatCertificateToPem);
   if(RawCert.find("-----BEGIN CERTIFICATE-----") != npos) {
      COL_TRC("Certificate already in PEM format");
      return RawCert;
   }
   COL_TRC("Converting raw base64 certificate to PEM format");
   COLstring CleanCert = RawCert;
   CleanCert.replace("\n", "");
   CleanCert.replace("\r", "");
   CleanCert.replace(" ", "");
   CleanCert.replace("\t", "");
   COLstring PemCert = "-----BEGIN CERTIFICATE-----\n";
   for(size_t i = 0; i < CleanCert.size(); i += 64) {
      const size_t Len = (i + 64 < CleanCert.size()) ? 64 : CleanCert.size() - i;
      PemCert += CleanCert.substr(i, Len) + "\n";
   }
   PemCert += "-----END CERTIFICATE-----\n";
   COL_TRC("Formatted certificate, total length: " << PemCert.size() << " bytes");
   return PemCert;
}

void SSOsamlRegisterXmlIds(xmlNodePtr Node, xmlDocPtr Doc) {
   if(!Node) return;
   if(Node->type == XML_ELEMENT_NODE) {
      const xmlAttrPtr IdAttr = xmlHasProp(Node, (const xmlChar*)"ID");
      if(IdAttr && IdAttr->children && IdAttr->children->content) {
         xmlAddID(NULL, Doc, IdAttr->children->content, IdAttr);
      }
   }
   for(xmlNodePtr Child = Node->children; Child != NULL; Child = Child->next) { SSOsamlRegisterXmlIds(Child, Doc); }
}

static xmlSecKeyPtr SSOsamlLoadPublicKeyFromCert(const COLstring& PemCert, COLstring& Error) {
   COL_FUNCTION(SSOsamlLoadPublicKeyFromCert);
   BIO* CertBio = BIO_new_mem_buf(PemCert.c_str(), PemCert.size());
   if(!CertBio) {
      Error = "Failed to create BIO";
      return NULL;
   }

   X509* Cert = PEM_read_bio_X509(CertBio, NULL, NULL, NULL);
   BIO_free(CertBio);
   if(!Cert) {
      Error = "Failed to parse certificate";
      return NULL;
   }

   EVP_PKEY* Pkey = X509_get_pubkey(Cert);
   X509_free(Cert);
   if(!Pkey) {
      Error = "Failed to extract public key";
      return NULL;
   }

   const int             KeyType   = EVP_PKEY_base_id(Pkey);
   const xmlSecKeyDataId KeyDataId = KeyType == EVP_PKEY_RSA   ? xmlSecOpenSSLKeyDataRsaId
                                     : KeyType == EVP_PKEY_DSA ? xmlSecOpenSSLKeyDataDsaId
                                                               : NULL;
   if(!KeyDataId) {
      Error = "Unsupported key type";
      EVP_PKEY_free(Pkey);
      return NULL;
   }
   const xmlSecKeyDataPtr KeyData = xmlSecKeyDataCreate(KeyDataId);
   if(!KeyData) {
      Error = "Failed to create key data";
      EVP_PKEY_free(Pkey);
      return NULL;
   }
   const int AdoptResult = KeyType == EVP_PKEY_RSA ? xmlSecOpenSSLKeyDataRsaAdoptEvp(KeyData, Pkey)
                                                   : xmlSecOpenSSLKeyDataDsaAdoptEvp(KeyData, Pkey);
   if(AdoptResult < 0) {
      Error = "Failed to adopt key";
      xmlSecKeyDataDestroy(KeyData);
      EVP_PKEY_free(Pkey);
      return NULL;
   }
   const xmlSecKeyPtr Key = xmlSecKeyCreate();
   if(!Key || xmlSecKeySetValue(Key, KeyData) < 0) {
      Error = "Failed to create key";
      if(!Key)
         xmlSecKeyDataDestroy(KeyData);
      else
         xmlSecKeyDestroy(Key);
      return NULL;
   }
   COL_TRC("Successfully loaded public key from certificate");
   return Key;
}

static void SSOsamlCleanupXmlSec() {
   xmlSecOpenSSLShutdown();
   xmlSecOpenSSLAppShutdown();
   xmlSecShutdown();
}

bool SSOsamlVerifySignature(xmlDocPtr Doc, const COLstring& Certificate, COLstring& Error) {
   COL_FUNCTION(SSOsamlVerifySignature);
   if(!Doc) {
      Error = "Invalid XML document";
      return false;
   }
   if(Certificate.is_null()) {
      Error = "No certificate available for signature verification";
      return false;
   }

   COL_TRC("Converting certificate to PEM format");
   const COLstring PemCert = SSOsamlFormatCertificateToPem(Certificate);

   COL_TRC("Initializing xmlsec library");
   if(xmlSecInit() < 0) {
      Error = "Failed to initialize xmlsec";
      return false;
   }
   if(xmlSecOpenSSLAppInit(NULL) < 0) {
      Error = "Failed to initialize OpenSSL";
      xmlSecShutdown();
      return false;
   }
   if(xmlSecOpenSSLInit() < 0) {
      Error = "Failed to initialize xmlsec OpenSSL backend";
      xmlSecOpenSSLAppShutdown();
      xmlSecShutdown();
      return false;
   }

   COL_TRC("Registering ID attributes");
   SSOsamlRegisterXmlIds(xmlDocGetRootElement(Doc), Doc);

   COL_TRC("Finding Signature node");
   const xmlNodePtr SigNode = xmlSecFindNode(xmlDocGetRootElement(Doc), xmlSecNodeSignature, xmlSecDSigNs);
   if(!SigNode) {
      Error = "Signature node not found";
      SSOsamlCleanupXmlSec();
      return false;
   }

   const xmlSecDSigCtxPtr DsigCtx = xmlSecDSigCtxCreate(NULL);
   if(!DsigCtx) {
      Error = "Failed to create signature context";
      SSOsamlCleanupXmlSec();
      return false;
   }

   const xmlSecKeyPtr Key = SSOsamlLoadPublicKeyFromCert(PemCert, Error);
   if(!Key) {
      xmlSecDSigCtxDestroy(DsigCtx);
      SSOsamlCleanupXmlSec();
      return false;
   }

   DsigCtx->signKey = Key;

   if(xmlSecDSigCtxVerify(DsigCtx, SigNode) < 0 || DsigCtx->status != xmlSecDSigStatusSucceeded) {
      Error = "Signature verification failed";
      xmlSecDSigCtxDestroy(DsigCtx);
      SSOsamlCleanupXmlSec();
      return false;
   }

   xmlSecDSigCtxDestroy(DsigCtx);
   SSOsamlCleanupXmlSec();

   COL_TRC("Signature verified successfully");
   return true;
}

time_t SSOsamlParseISO8601(const COLstring& Timestamp) {
   tm  TimeInfo = {};
   int Year, Month, Day, Hour, Min, Sec;
   if(sscanf(Timestamp.c_str(), "%d-%d-%dT%d:%d:%dZ", &Year, &Month, &Day, &Hour, &Min, &Sec) == 6) {
      TimeInfo.tm_year  = Year - 1900;
      TimeInfo.tm_mon   = Month - 1;
      TimeInfo.tm_mday  = Day;
      TimeInfo.tm_hour  = Hour;
      TimeInfo.tm_min   = Min;
      TimeInfo.tm_sec   = Sec;
      TimeInfo.tm_isdst = 0;

#ifdef _WIN32
      return _mkgmtime(&TimeInfo);
#else
      return timegm(&TimeInfo);
#endif
   }
   return 0;
}

static COLstring SSOsamlGetAttributeValue(xmlNodePtr Node, const char* AttrName) {
   if(!Node) return COLstring();
   xmlChar* Value = xmlGetProp(Node, (const xmlChar*)AttrName);
   if(!Value) return COLstring();
   COLstring Result((const char*)Value);
   xmlFree(Value);
   return Result;
}

bool SSOsamlValidateTimestamp(xmlDocPtr Doc, COLstring& Error) {
   COL_FUNCTION(SSOsamlValidateTimestamp);
   const xmlXPathContextPtr XpathCtx = xmlXPathNewContext(Doc);
   if(!XpathCtx) {
      Error = "Failed to create XPath context for timestamp validation";
      return false;
   }

   xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"saml", (const xmlChar*)"urn:oasis:names:tc:SAML:2.0:assertion");

   const xmlXPathObjectPtr ConditionsObj =
       xmlXPathEvalExpression((const xmlChar*)"//saml:Assertion/saml:Conditions", XpathCtx);
   bool IsValid = true;

   if(ConditionsObj && ConditionsObj->nodesetval && ConditionsObj->nodesetval->nodeNr > 0) {
      const xmlNodePtr ConditionsNode = ConditionsObj->nodesetval->nodeTab[0];
      const COLstring  NotBefore      = SSOsamlGetAttributeValue(ConditionsNode, "NotBefore");
      const COLstring  NotOnOrAfter   = SSOsamlGetAttributeValue(ConditionsNode, "NotOnOrAfter");

      if(!NotBefore.is_null() || !NotOnOrAfter.is_null()) {
         const time_t CurrentTime = time(NULL);

         if(!NotBefore.is_null()) {
            const time_t NotBeforeTime = SSOsamlParseISO8601(NotBefore);
            if(NotBeforeTime > 0 && NotBeforeTime > CurrentTime) {
               Error = "SAML assertion not yet valid";
               COL_TRC(Error);
               IsValid = false;
            }
         }

         if(IsValid && !NotOnOrAfter.is_null()) {
            const time_t NotOnOrAfterTime = SSOsamlParseISO8601(NotOnOrAfter);
            if(NotOnOrAfterTime > 0 && NotOnOrAfterTime < CurrentTime) {
               Error = "SAML assertion expired";
               COL_TRC(Error);
               IsValid = false;
            }
         }

         if(IsValid) { COL_TRC("Timestamp validation passed"); }
      }
   }

   if(ConditionsObj) xmlXPathFreeObject(ConditionsObj);
   xmlXPathFreeContext(XpathCtx);

   return IsValid;
}

bool SSOsamlValidateAudience(xmlDocPtr Doc, const COLstring& ExpectedEntityId, COLstring& Error) {
   COL_FUNCTION(SSOsamlValidateAudience);
   const xmlXPathContextPtr XpathCtx = xmlXPathNewContext(Doc);
   if(!XpathCtx) {
      Error = "Failed to create XPath context for audience validation";
      return false;
   }

   xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"saml", (const xmlChar*)"urn:oasis:names:tc:SAML:2.0:assertion");

   const xmlXPathObjectPtr AudienceObj = xmlXPathEvalExpression(
       (const xmlChar*)"//saml:Assertion/saml:Conditions/saml:AudienceRestriction/saml:Audience/text()", XpathCtx);

   bool IsValid = true;

   if(AudienceObj && AudienceObj->nodesetval && AudienceObj->nodesetval->nodeNr > 0) {
      const xmlNodePtr AudienceNode = AudienceObj->nodesetval->nodeTab[0];
      if(AudienceNode->content) {
         const COLstring Audience = (const char*)AudienceNode->content;
         COL_TRC("SAML Audience: " << Audience);
         COL_TRC("Expected Entity ID: " << ExpectedEntityId);

         if(Audience != ExpectedEntityId) {
            Error = "SAML assertion not intended for this service";
            COL_TRC(Error);
            IsValid = false;
         } else {
            COL_TRC("Audience validation passed");
         }
      }
   }

   if(AudienceObj) xmlXPathFreeObject(AudienceObj);
   xmlXPathFreeContext(XpathCtx);
   return IsValid;
}
