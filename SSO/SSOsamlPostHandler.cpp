//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlPostHandler.cpp
//
//  Description: SAML POST handler - user extraction and session creation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SFI/SFIuriUnescapeFilter.h>
#include <SSO/SSOconfig.h>
#include <SSO/SSOsamlPostHandler.h>
#include <SSO/SSOsamlUtils.h>
#include <SSO/SSOsamlVerify.h>
#include <SSO/SSOsession.h>
#include <SSO/SSOuser.h>
#include <USR/USRuser.h>
#include <WEB/WEBcookie.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBsession.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
COL_LOG_MODULE;

// XPath query constants
static auto XPATH_SAML_NAMEID      = "//saml:Assertion/saml:Subject/saml:NameID/text()";
static auto XPATH_SAML_ATTRIBUTES  = "//saml:Assertion/saml:AttributeStatement/saml:Attribute";
static auto XPATH_SAML_ISSUER      = "//samlp:Response/saml:Issuer/text()";
static auto XPATH_SAML_STATUS_CODE = "//samlp:Response/samlp:Status/samlp:StatusCode";

// SAML namespace URIs
static auto SAML_NS_ASSERTION = "urn:oasis:names:tc:SAML:2.0:assertion";
static auto SAML_NS_PROTOCOL  = "urn:oasis:names:tc:SAML:2.0:protocol";

// SAML attribute schema URIs
static auto SAML_ATTR_EMAIL     = "http://schemas.xmlsoap.org/ws/2005/05/identity/claims/emailaddress";
static auto SAML_ATTR_OBJECTID  = "http://schemas.microsoft.com/identity/claims/objectidentifier";
static auto SAML_ATTR_TENANTID  = "http://schemas.microsoft.com/identity/claims/tenantid";
static auto SAML_ATTR_WIDS      = "http://schemas.microsoft.com/ws/2008/06/identity/claims/wids";
static auto SAML_ATTR_GIVENNAME = "http://schemas.xmlsoap.org/ws/2005/05/identity/claims/givenname";
static auto SAML_ATTR_SURNAME   = "http://schemas.xmlsoap.org/ws/2005/05/identity/claims/surname";
static auto SAML_ATTR_ROLE      = "http://schemas.microsoft.com/ws/2008/06/identity/claims/role";

struct SSOsamlUserAttributes {
   COLstring           Email;
   COLstring           ObjectId;
   COLstring           TenantId;
   COLstring           GivenName;
   COLstring           Surname;
   COLarray<COLstring> Roles;
};

struct SSOsamlProviderPattern {
   const char* Pattern;
   SSOprovider Provider;
};

static const SSOsamlProviderPattern PROVIDER_PATTERNS[] = {
    {"microsoft", MICROSOFT}, {"windows.net", MICROSOFT}, {"microsoftonline.com", MICROSOFT},
    {"okta.com", OKTA},       {"google.com", GOOGLE},     {NULL, NONE}};

enum SSOsamlAttributeType {
   ATTR_EMAIL,
   ATTR_OBJECTID,
   ATTR_TENANTID,
   ATTR_WIDS,
   ATTR_GIVENNAME,
   ATTR_SURNAME,
   ATTR_ROLE,
   ATTR_UNKNOWN
};
struct SSOsamlAttributePattern {
   SSOsamlAttributeType Type;
   const char*          ShortName;      // Lowercase short name
   const char*          SchemaUri;      // Full schema URI
   const char*          AlternateName;  // Alternative name (can be NULL)
};

static const SSOsamlAttributePattern ATTRIBUTE_PATTERNS[] = {
    {ATTR_EMAIL, "email", SAML_ATTR_EMAIL, "mail"},
    {ATTR_EMAIL, "emailaddress", SAML_ATTR_EMAIL, NULL},
    {ATTR_OBJECTID, "objectidentifier", SAML_ATTR_OBJECTID, NULL},
    {ATTR_TENANTID, "tenantid", SAML_ATTR_TENANTID, NULL},
    {ATTR_WIDS, "wids", SAML_ATTR_WIDS, NULL},
    {ATTR_GIVENNAME, "givenname", SAML_ATTR_GIVENNAME, "firstname"},
    {ATTR_SURNAME, "surname", SAML_ATTR_SURNAME, "lastname"},
    {ATTR_ROLE, "role", SAML_ATTR_ROLE, "groups"},
    {ATTR_ROLE, "group", SAML_ATTR_ROLE, NULL},
    {ATTR_UNKNOWN, NULL, NULL, NULL}};

static COLstring SSOsanitizeForDirectory(const COLstring& Input) {
   COLstring Safe = Input;
   Safe.replace("/", "_");
   Safe.replace("\\", "_");
   Safe.replace(":", "_");
   Safe.replace("*", "_");
   Safe.replace("?", "_");
   Safe.replace("\"", "_");
   Safe.replace("<", "_");
   Safe.replace(">", "_");
   Safe.replace("|", "_");
   Safe.replace("#", "_");
   Safe.replace("@", "_at_");
   Safe.replace(" ", "_");
   Safe.replace("\t", "_");
   Safe.replace("\n", "_");
   Safe.replace("\r", "_");
   Safe.replace(".", "_");
   return Safe;
}

static COLstring SSOgetNodeText(const xmlNodePtr Node) {
   if(!Node || !Node->children) return COLstring();
   for(xmlNodePtr Child = Node->children; Child; Child = Child->next) {
      if(Child->type == XML_TEXT_NODE && Child->content) { return COLstring((const char*)Child->content); }
   }
   return COLstring();
}

static COLstring SSOgetAttributeValue(const xmlNodePtr Node, const char* AttrName) {
   if(!Node) return COLstring();
   xmlChar* Value = xmlGetProp(Node, (const xmlChar*)AttrName);
   if(!Value) return COLstring();
   COLstring Result((const char*)Value);
   xmlFree(Value);
   return Result;
}

static xmlNodePtr SSOfindChildElement(const xmlNodePtr Parent, const char* Name) {
   if(!Parent) return NULL;
   for(xmlNodePtr Child = Parent->children; Child; Child = Child->next) {
      if(Child->type == XML_ELEMENT_NODE && xmlStrcmp(Child->name, (const xmlChar*)Name) == 0) { return Child; }
   }
   return NULL;
}

static COLstring SSOhttpResponse(const COLstring& Status, const COLstring& ContentType, const COLstring& Body,
                                 const COLstring& ExtraHeaders) {
   COLstring Response = "HTTP/1.1 " + Status + "\r\n";
   Response += "Content-Type: " + ContentType + "\r\n";
   Response += "Content-Length: " + COLintToString(Body.size()) + "\r\n";
   if(!ExtraHeaders.is_null()) { Response += ExtraHeaders; }
   Response += "Connection: close\r\n";
   Response += "\r\n";
   Response += Body;
   return Response;
}

static COLstring SSOhttpResponseWithCookie(const COLstring& Status, const COLstring& ContentType, const COLstring& Body,
                                           const COLstring& CookieName, const COLstring& CookieValue,
                                           const bool IsHttps) {
   COLstring Cookie = "Set-Cookie: " + CookieName + "=" + CookieValue + "; path=/; HttpOnly; SameSite=Strict";
   if(IsHttps) { Cookie += "; Secure"; }
   Cookie += "\r\n";
   return SSOhttpResponse(Status, ContentType, Body, Cookie);
}

static bool SSOsamlValidateConfig(const SSOsamlConfig& Config, COLstring& Error) {
   COL_FUNCTION(SSOsamlValidateConfig);
   if(Config.m_EntityId.is_null()) {
      Error = "SAML configuration error: Entity ID not configured";
      COL_TRC(Error);
      return false;
   }
   if(Config.m_Certificate.is_null()) {
      Error = "SAML configuration error: Certificate not configured";
      COL_TRC(Error);
      return false;
   }
   COL_TRC("SAML configuration validated");
   return true;
}

static COLstring SSOsamlExtractNameId(const xmlXPathContextPtr XpathCtx) {
   COL_FUNCTION(SSOsamlExtractNameId);
   const xmlXPathObjectPtr NameIdObj = xmlXPathEvalExpression((const xmlChar*)XPATH_SAML_NAMEID, XpathCtx);
   COLstring               NameId;
   if(NameIdObj && NameIdObj->nodesetval && NameIdObj->nodesetval->nodeNr > 0) {
      const xmlNodePtr Node = NameIdObj->nodesetval->nodeTab[0];
      if(Node->content) {
         NameId = (const char*)Node->content;
         COL_TRC("Found NameID: " << NameId);
      }
   }
   if(NameIdObj) xmlXPathFreeObject(NameIdObj);
   return NameId;
}

static SSOsamlAttributeType SSOsamlDetectAttributeType(const COLstring& AttrName, const COLstring& AttrNameLower) {
   COL_FUNCTION(SSOsamlDetectAttributeType);
   COL_VAR2(AttrName, AttrNameLower);
   for(int i = 0; ATTRIBUTE_PATTERNS[i].Type != ATTR_UNKNOWN; i++) {
      const SSOsamlAttributePattern& Pattern = ATTRIBUTE_PATTERNS[i];
      COL_VAR(Pattern.ShortName);
      if(Pattern.ShortName && AttrNameLower.find(Pattern.ShortName) != npos) { return Pattern.Type; }
      if(Pattern.SchemaUri && AttrName == Pattern.SchemaUri) { return Pattern.Type; }
      if(Pattern.AlternateName && AttrNameLower == Pattern.AlternateName) { return Pattern.Type; }
   }
   return ATTR_UNKNOWN;
}

static void SSOsamlProcessAttributeValue(const COLstring& AttrValue, const SSOsamlAttributeType Type,
                                         SSOsamlUserAttributes& Attributes) {
   switch(Type) {
      case ATTR_EMAIL: Attributes.Email = AttrValue; break;
      case ATTR_OBJECTID:
         Attributes.ObjectId = AttrValue;
         COL_TRC("Found ObjectId: " << Attributes.ObjectId);
         break;
      case ATTR_TENANTID:
         Attributes.TenantId = AttrValue;
         COL_TRC("Found TenantId: " << Attributes.TenantId);
         break;
      case ATTR_WIDS:
         Attributes.Roles.push_back("#" + AttrValue);
         COL_TRC("Found role (wids): " << AttrValue);
         break;
      case ATTR_GIVENNAME: Attributes.GivenName = AttrValue; break;
      case ATTR_SURNAME: Attributes.Surname = AttrValue; break;
      case ATTR_ROLE: Attributes.Roles.push_back("#" + AttrValue); break;
      case ATTR_UNKNOWN: break;
   }
}

static void SSOsamlProcessAttribute(const xmlNodePtr AttrNode, SSOsamlUserAttributes& Attributes) {
   COL_FUNCTION(SSOsamlProcessAttribute);
   const COLstring            AttrName      = SSOgetAttributeValue(AttrNode, "Name");
   const COLstring            AttrNameLower = COLlowerCase(AttrName);
   const SSOsamlAttributeType Type          = SSOsamlDetectAttributeType(AttrName, AttrNameLower);
   
   for(xmlNodePtr Child = AttrNode->children; Child; Child = Child->next) {
      if(Child->type == XML_ELEMENT_NODE && xmlStrcmp(Child->name, (const xmlChar*)"AttributeValue") == 0) {
         const COLstring AttrValue = SSOgetNodeText(Child);
         COL_VAR2(AttrName, AttrValue);
         if(AttrValue.is_null()) continue;
         
         COL_TRC("SAML Attribute: " << AttrName << " = " << AttrValue);
         SSOsamlProcessAttributeValue(AttrValue, Type, Attributes);
      }
   }
}

static void SSOsamlExtractAttributes(const xmlXPathContextPtr XpathCtx, SSOsamlUserAttributes& Attributes) {
   COL_FUNCTION(SSOsamlExtractAttributes);
   const xmlXPathObjectPtr AttrsObj = xmlXPathEvalExpression((const xmlChar*)XPATH_SAML_ATTRIBUTES, XpathCtx);
   if(AttrsObj && AttrsObj->nodesetval) {
      for(int i = 0; i < AttrsObj->nodesetval->nodeNr; i++) {
         SSOsamlProcessAttribute(AttrsObj->nodesetval->nodeTab[i], Attributes);
      }
   }
   if(AttrsObj) xmlXPathFreeObject(AttrsObj);
}

static COLstring SSOsamlDetermineUserId(const COLstring& NameId, const SSOsamlUserAttributes& Attributes,
                                        const SSOprovider Provider) {
   COL_FUNCTION(SSOsamlDetermineUserId);
   if(Provider == MICROSOFT && !Attributes.ObjectId.is_null() && !Attributes.TenantId.is_null()) {
      COLstring UserId = Attributes.ObjectId + "_" + Attributes.TenantId;
      COL_TRC("Using Microsoft objectId_tenantId as userId: " << UserId);
      return UserId;
   }
   return NameId;
}

static COLstring SSOsamlDetermineDisplayName(const SSOsamlUserAttributes& Attributes, const COLstring& NameId) {
   COL_FUNCTION(SSOsamlDetermineDisplayName);
   if(!Attributes.Email.is_null()) { return Attributes.Email; }
   if(!NameId.is_null()) { return NameId; }
   if(!Attributes.GivenName.is_null() && !Attributes.Surname.is_null()) {
      return Attributes.GivenName + " " + Attributes.Surname;
   }
   if(!Attributes.GivenName.is_null()) { return Attributes.GivenName; }
   if(!Attributes.Surname.is_null()) { return Attributes.Surname; }
   return NameId;
}

static bool SSOsamlCreateUser(const COLstring& UserId, const COLstring& DisplayName, const COLarray<COLstring>& Roles,
                              const SSOprovider Provider, USRuser& User, COLstring& Error) {
   COL_FUNCTION(SSOsamlCreateUser);
   if(UserId.is_null() || DisplayName.is_null()) {
      Error = "Missing required user information (userId or displayName)";
      return false;
   }
   const COLstring SafeUserId = SSOsanitizeForDirectory(UserId);
   if(SafeUserId.is_null()) {
      Error = "Failed to create safe username";
      return false;
   }
   User = USRuser(SafeUserId, "", Roles, false);
   User.setProvider(SSOproviderToString(Provider));
   User.setDisplayName(DisplayName);
   User.setExternal();
   COL_TRC("Extracted SAML user: " << DisplayName << " (ID: " << SafeUserId << ") with " << Roles.size() << " roles");
   return true;
}

static bool SSOsamlExtractUserInfo(const xmlDocPtr Doc, USRuser& User, const SSOprovider Provider, COLstring& Error) {
   COL_FUNCTION(SSOsamlExtractUserInfo);
   const xmlXPathContextPtr XpathCtx = xmlXPathNewContext(Doc);

   if(!XpathCtx) {
      Error = "Failed to create XPath context";
      return false;
   }
   xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"saml", (const xmlChar*)SAML_NS_ASSERTION);
   const COLstring       NameId = SSOsamlExtractNameId(XpathCtx);
   SSOsamlUserAttributes Attributes;
   SSOsamlExtractAttributes(XpathCtx, Attributes);
   xmlXPathFreeContext(XpathCtx);
   const COLstring UserId      = SSOsamlDetermineUserId(NameId, Attributes, Provider);
   const COLstring DisplayName = SSOsamlDetermineDisplayName(Attributes, NameId);
   for(auto& role : Attributes.Roles) { role = COLremoveAllWhiteSpace(role); }
   return SSOsamlCreateUser(UserId, DisplayName, Attributes.Roles, Provider, User, Error);
}

static SSOprovider SSOsamlDetectProvider(const xmlDocPtr Doc) {
   COL_FUNCTION(SSOsamlDetectProvider);
   const xmlXPathContextPtr XpathCtx = xmlXPathNewContext(Doc);
   if(!XpathCtx) return NONE;
   xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"samlp", (const xmlChar*)SAML_NS_PROTOCOL);
   xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"saml", (const xmlChar*)SAML_NS_ASSERTION);
   const xmlXPathObjectPtr IssuerObj = xmlXPathEvalExpression((const xmlChar*)XPATH_SAML_ISSUER, XpathCtx);
   SSOprovider             Provider  = NONE;
   if(IssuerObj && IssuerObj->nodesetval && IssuerObj->nodesetval->nodeNr > 0) {
      const xmlNodePtr Node = IssuerObj->nodesetval->nodeTab[0];
      if(Node->content) {
         const COLstring IssuerValue = COLlowerCase((const char*)Node->content);
         COL_TRC("Detecting IdP from issuer: " << IssuerValue);
         for(int i = 0; PROVIDER_PATTERNS[i].Pattern != NULL; i++) {
            if(IssuerValue.find(PROVIDER_PATTERNS[i].Pattern) != npos) {
               Provider = PROVIDER_PATTERNS[i].Provider;
               break;
            }
         }
      }
   }
   if(IssuerObj) xmlXPathFreeObject(IssuerObj);
   xmlXPathFreeContext(XpathCtx);
   COL_TRC("Detected provider: " << SSOproviderToString(Provider));
   return Provider;
}

static bool SSOsamlValidateStatus(const xmlDocPtr Doc, COLstring& Error) {
   COL_FUNCTION(SSOsamlValidateStatus);
   const xmlXPathContextPtr XpathCtx = xmlXPathNewContext(Doc);
   if(!XpathCtx) {
      Error = "Failed to create XPath context for status validation";
      COL_TRC(Error);
      return false;
   }
   xmlXPathRegisterNs(XpathCtx, (const xmlChar*)"samlp", (const xmlChar*)SAML_NS_PROTOCOL);

   const xmlXPathObjectPtr StatusObj = xmlXPathEvalExpression((const xmlChar*)XPATH_SAML_STATUS_CODE, XpathCtx);
   bool                    Success   = false;

   if(StatusObj && StatusObj->nodesetval && StatusObj->nodesetval->nodeNr > 0) {
      const xmlNodePtr StatusNode = StatusObj->nodesetval->nodeTab[0];
      const COLstring  StatusCode = SSOgetAttributeValue(StatusNode, "Value");
      COL_TRC("SAML Status Code: " << StatusCode);

      if(StatusCode == "urn:oasis:names:tc:SAML:2.0:status:Success") {
         Success = true;
      } else {
         Error = "SAML authentication failed with status: " + StatusCode;
         COL_TRC(Error);
      }
   } else {
      Error = "No status code found in SAML response";
      COL_TRC(Error);
   }

   if(StatusObj) xmlXPathFreeObject(StatusObj);
   xmlXPathFreeContext(XpathCtx);
   return Success;
}

static bool SSOsamlParseAndValidate(const COLstring& SamlResponse, const SSOsamlConfig& Config, USRuser& User,
                                    COLstring& Error) {
   COL_FUNCTION(SSOsamlParseAndValidate);
   COLstring DecodedResponse;
   SFIbase64Decode(SamlResponse, &DecodedResponse);
   COL_TRC("Decoded SAML Response length: " << DecodedResponse.size());
   COL_TRC("Decoded SAML Response XML:\n" << DecodedResponse);
   const xmlDocPtr Doc = xmlParseMemory(DecodedResponse.c_str(), DecodedResponse.size());
   if(!Doc) {
      Error = "Failed to parse SAML XML";
      return false;
   }
   COL_TRC("Successfully parsed SAML XML");

   if(!SSOsamlValidateStatus(Doc, Error)) {
      xmlFreeDoc(Doc);
      return false;
   }

   if(!SSOsamlVerifySignature(Doc, Config.m_Certificate, Error)) {
      xmlFreeDoc(Doc);
      return false;
   }

   if(!SSOsamlValidateTimestamp(Doc, Error)) {
      xmlFreeDoc(Doc);
      return false;
   }

   if(!SSOsamlValidateAudience(Doc, Config.m_EntityId, Error)) {
      xmlFreeDoc(Doc);
      return false;
   }

   const SSOprovider Provider = SSOsamlDetectProvider(Doc);
   const bool        Result   = SSOsamlExtractUserInfo(Doc, User, Provider, Error);
   xmlFreeDoc(Doc);
   return Result;
}

static void SSOsamlPostCallback(const bool Success, const COLstring Message, const USRuser& User, const bool IsHttps,
                                COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap,
                                const COLuint64 ConnId) {
   if(!Success) { return COLsendWebResponse(ConnId, SSOsamlHtmlErrorPageWithScript(Message)); }
   (*pUserMap)[User.username()] = User;
   const COLstring SessionId    = WEBcreateSession(pSessionMap, User.username());
   WEBsession&     Session      = (*pSessionMap)[SessionId];
   SSOcreateNewSession(Session, SAML, SSOproviderToEnum(User.provider()), User.username());
   SDBIlogService(User.displayName() + " has logged in via " + User.provider() + " SAML.", "#login");
   const COLstring HtmlBody = SSOsamlHtmlSuccessPageWithRedirect("/");
   const COLstring Response = SSOhttpResponseWithCookie("200 OK", "text/html", HtmlBody, WEBdefaultCookieName(IsHttps), SessionId, IsHttps);
   COLsendWebResponse(ConnId, Response);
   COL_TRC("SAML authentication successful for: " << User.username());
}

static COLstring SSOextractSamlResponseFromBody(const COLstring& Body) {
   COL_FUNCTION(SSOextractSamlResponseFromBody);
   const COLstring Prefix = "SAMLResponse=";
   const size_t    Pos    = Body.find(Prefix);
   if(Pos == npos) return COLstring();
   const size_t Start = Pos + Prefix.size();
   const size_t End   = Body.find("&", Start);
   return End == npos ? Body.substr(Start) : Body.substr(Start, End - Start);
}

static COLstring SSOextractRelayState(const COLstring& Body) {
   COL_FUNCTION(SSOextractRelayState);
   const COLstring Prefix = "RelayState=";
   const size_t    Pos    = Body.find(Prefix);
   if(Pos == npos) return COLstring();
   const size_t Start = Pos + Prefix.size();
   const size_t End   = Body.find("&", Start);
   return End == npos ? Body.substr(Start) : Body.substr(Start, End - Start);
}

static bool SSOisRevalidation(const COLstring& RelayState, COLstring& Hash) {
   const COLstring Prefix = "revalidate:";
   if(RelayState.find(Prefix) == 0) {
      Hash = RelayState.substr(Prefix.size());
      return !Hash.is_null();
   }
   return false;
}

static void SSOsamlRevalidateCallback(const bool Success, const COLstring Message, const USRuser& User,
                                      const bool IsHttps, const COLstring Hash, COLmap<COLstring, USRuser>* pUserMap,
                                      COLmap<COLstring, WEBsession>* pSessionMap, const COLuint64 ConnId) {
   if(!Success) {
      const COLstring SafeError = SSOsamlHtmlEscape(Message);
      const COLstring HtmlBody  = SSOsamlHtmlErrorPageWithScript(SafeError);
      const COLstring Response  = SSOhttpResponse("200 OK", "text/html; charset=utf-8", HtmlBody, COLstring());
      return COLsendWebResponse(ConnId, Response);
   }

   (*pUserMap)[User.username()] = User;
   const COLstring SessionId    = WEBcreateSession(pSessionMap, User.username());
   WEBsession&     Session      = (*pSessionMap)[SessionId];
   SSOcreateNewSession(Session, SAML, SSOproviderToEnum(User.provider()), User.username());

   const COLstring RedirectPath = Hash.is_null() ? "/" : ("/" + Hash);
   const COLstring HtmlBody     = SSOsamlHtmlSuccessPageWithRedirect(RedirectPath);
   const COLstring Response     = SSOhttpResponseWithCookie("200 OK", "text/html", HtmlBody, WEBdefaultCookieName(IsHttps), SessionId, IsHttps);
   COLsendWebResponse(ConnId, Response);
   COL_TRC("SAML revalidation successful for: " << User.username());
}

void SSOsamlPostHandler(const WEBrequestData& RequestData, COLmap<COLstring, USRuser>* pUserMap,
                        COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop) {
   COL_FUNCTION(SSOsamlHandlePost);
   try {
      const COLstring EncodedResponse = SSOextractSamlResponseFromBody(RequestData.Body);
      if(EncodedResponse.is_null()) {
         return COLsendWebResponse(RequestData.ConnectionId, SSOsamlHtmlErrorPageWithScript("Missing SAMLResponse parameter"));
      }

      COLstring       Hash, Error;
      const COLstring RelayState     = SFIuriUnescape(SSOextractRelayState(RequestData.Body));
      const bool      IsRevalidation = SSOisRevalidation(RelayState, Hash);
      const COLstring SamlResponse   = SFIuriUnescape(SSOextractSamlResponseFromBody(RequestData.Body));
      const SSOconfig Config         = SSOgetConfig();

      if(!SSOsamlValidateConfig(Config.m_SamlConfig, Error)) {
         return COLsendWebResponse(RequestData.ConnectionId, SSOsamlHtmlErrorPageWithScript(Error));
      }

      USRuser User;
      if(!SSOsamlParseAndValidate(SamlResponse, Config.m_SamlConfig, User, Error)) {
         COL_TRC("Failed to parse SAML response: " << Error);
         return COLsendWebResponse(RequestData.ConnectionId, SSOsamlHtmlErrorPageWithScript(Error));
      }
      COLauto<COLclosure3<bool, COLstring, const USRuser&>> pCallback;
      if(IsRevalidation) {
         COL_TRC("Processing SAML revalidation with hash: " << Hash);
         pCallback = COLnewClosure3(&SSOsamlRevalidateCallback, RequestData.IsSecure, Hash, pUserMap, pSessionMap, RequestData.ConnectionId);
      } else {
         pCallback = COLnewClosure3(&SSOsamlPostCallback, RequestData.IsSecure, pUserMap, pSessionMap, RequestData.ConnectionId);
      }
      const SSOprovider Provider = SSOproviderToEnum(User.provider());
      if(!SSOsetupUser(User, Provider, pUserMap, pCallback, pLoop, Error)) {
         return COLsendWebResponse(RequestData.ConnectionId, SSOsamlHtmlErrorPageWithScript(Error));
      }
   } catch(const COLerror& E) {
      COL_TRC("SAML callback error: " << E.description());
      return COLsendWebResponse(RequestData.ConnectionId, SSOsamlHtmlErrorPageWithScript(E.description()));
   }
}
