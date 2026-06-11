//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlUtils.cpp
//
//  Description: Shared SAML utility functions
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <SFI/SFIbase64EncodeFilter.h>
#include <SFI/SFIuriEscapeFilter.h>
#include <SSO/SSOsamlUtils.h>
#include <zlib.h>
#include <cstdio>
COL_LOG_MODULE;

COLstring SSOsamlFormatIso8601(time_t Time) {
   struct tm TimeStruct;
#ifdef _WIN32
   gmtime_s(&TimeStruct, &Time);
#else
   gmtime_r(&Time, &TimeStruct);
#endif
   char Buffer[32];
   strftime(Buffer, sizeof(Buffer), "%Y-%m-%dT%H:%M:%SZ", &TimeStruct);
   COLstring Result(Buffer);
   COL_TRC("Formatted timestamp: " << Result);
   return Result;
}

void SSOsamlDeflate(const COLstring& Input, COLstring* pOutput) {
   COL_FUNCTION(SSOsamlDeflate);
   COL_TRC("Starting DEFLATE compression of " << Input.size() << " bytes");
   z_stream Zstream = {};
   const int StreamState =
       deflateInit2(&Zstream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
   COLASSERT(StreamState == Z_OK);
   int SizeRequired = deflateBound(&Zstream, Input.size()) + 10;
   SizeRequired     = COL_MAX(24, SizeRequired);
   pOutput->setCapacity(SizeRequired);
   Zstream.avail_in  = Input.size();
   Zstream.next_in   = (unsigned char*)Input.c_str();
   Zstream.avail_out = pOutput->capacity() - 1;
   Zstream.next_out  = (unsigned char*)pOutput->c_str();
   const int Ret     = deflate(&Zstream, Z_FINISH);
   COLASSERT(Z_STREAM_END == Ret);
   pOutput->setSize(pOutput->capacity() - Zstream.avail_out - 1);
   pOutput->setCapacity(pOutput->capacity() - Zstream.avail_out);
   deflateEnd(&Zstream);
   COL_TRC("DEFLATE compressed " << Input.size() << " bytes to " << pOutput->size() << " bytes");
}

COLstring SSOsamlHtmlEscape(const COLstring& Input) {
   COLstring Escaped = Input;
   Escaped.replace("&", "&amp;");
   Escaped.replace("<", "&lt;");
   Escaped.replace(">", "&gt;");
   Escaped.replace("\"", "&quot;");
   Escaped.replace("'", "&#39;");
   return Escaped;
}

COLstring SSOsamlHtmlErrorPageWithScript(const COLstring& SafeError) {
   COL_FUNCTION(SSOsamlHtmlErrorPageWithScript);
   return "<!DOCTYPE html>"
          "<html>"
          "<head>"
          "<title>Redirecting...</title>"
          "</head>"
          "<body style='display:none;'>"
          "<div id='saml_error' style='display:none;'>" +
          SafeError +
          "</div>"
          "<script>"
          "try {"
          "  const errorDiv = document.getElementById('saml_error');"
          "  if(errorDiv && errorDiv.textContent) {"
          "    sessionStorage.setItem('saml_error', errorDiv.textContent);"
          "  }"
          "} catch(e) { console.error('Failed to store SAML error:', e); }"
          "window.location.href = '/';"
          "</script>"
          "<noscript><meta http-equiv='refresh' content='0; url=/'></noscript>"
          "</body>"
          "</html>";
}

COLstring SSOsamlHtmlSuccessPageWithRedirect(const COLstring& RedirectPath) {
   COL_FUNCTION(SSOsamlHtmlSuccessPageWithRedirect);
   return "<!DOCTYPE html>"
          "<html>"
          "<head>"
          "<title>Authentication Successful</title>"
          "</head>"
          "<body style='display:none;'>"
          "<script>"
          "window.location.href = '" +
          RedirectPath +
          "';"
          "</script>"
          "<noscript><meta http-equiv='refresh' content='0; url=" +
          RedirectPath +
          "'></noscript>"
          "</body>"
          "</html>";
}

COLstring SSOsamlBuildAuthnRequest(const SSOsamlRequestParams& Params) {
   COL_FUNCTION(SSOsamlBuildAuthnRequest);
   COLstring Request =
       "<samlp:AuthnRequest "
       "xmlns:samlp=\"urn:oasis:names:tc:SAML:2.0:protocol\" "
       "xmlns:saml=\"urn:oasis:names:tc:SAML:2.0:assertion\" "
       "ID=\"_" +
       Params.RequestId +
       "\" "
       "Version=\"2.0\" "
       "IssueInstant=\"" +
       Params.Timestamp +
       "\" "
       "Destination=\"" +
       Params.Destination + "\"";

   if(Params.ForceAuthn || Params.IsPassive) {
      Request += " ForceAuthn=\"" + COLstring(Params.ForceAuthn ? "true" : "false") + "\"";
      Request += " IsPassive=\"" + COLstring(Params.IsPassive ? "true" : "false") + "\"";
   }

   Request += " ProtocolBinding=\"urn:oasis:names:tc:SAML:2.0:bindings:HTTP-" + Params.Binding + "\" "
              "AssertionConsumerServiceURL=\"" +
              Params.AcsUrl +
              "\">"
              "<saml:Issuer>" +
              Params.EntityId +
              "</saml:Issuer>"
              "<samlp:NameIDPolicy Format=\"urn:oasis:names:tc:SAML:1.1:nameid-format:emailAddress\" "
              "AllowCreate=\"true\"/>"
              "</samlp:AuthnRequest>";

   return Request;
}

bool SSOsamlBuildRedirectUrl(const SSOprovider Provider, const SSOsamlConfig& Config, const COLstring& SamlRequest,
                             const COLstring& RelayState, COLstring& RedirectUrl, COLstring& Error) {
   COL_FUNCTION(SSOsamlBuildRedirectUrl);
   try {
      COLstring CompressedRequest;
      SSOsamlDeflate(SamlRequest, &CompressedRequest);
      COLstring EncodedRequest;
      SFIbase64EncodeOneLine(CompressedRequest, &EncodedRequest);
      const char Separator = Config.m_SsoLoginUrl.find('?') != npos ? '&' : '?';
      RedirectUrl          = Config.m_SsoLoginUrl + Separator + "SAMLRequest=" + SFIuriEscape(EncodedRequest);
      if(!RelayState.is_null()) { RedirectUrl += "&RelayState=" + SFIuriEscape(RelayState); }
      COL_TRC("Generated SAML redirect URL");
      return true;
   } catch(const COLerror& E) {
      Error = "Failed to build SAML redirect URL: " + E.description();
      COL_TRC(Error);
      return false;
   }
}
