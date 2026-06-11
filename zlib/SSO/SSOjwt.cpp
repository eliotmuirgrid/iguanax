//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOjwt.cpp
//
//  Description: JWT token verification implementation for Single Sign-On (SSO).
//  Validates Microsoft ID tokens by fetching public keys from corresponding JWKS endpoint,
//  verifying RSA-SHA256 signatures, and validating standard JWT claims (iss, aud, exp).
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvarUtils.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SSO/SSOdiscovery.h>
#include <SSO/SSOjwt.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>
COL_LOG_MODULE;

struct JwksCache {
   COLvar m_Jwks;
   time_t m_CacheTime = 0;
};
static JwksCache g_Jwks[3];  // indexed by SSOprovider

static const int JWKS_CACHE_DURATION = 300;

static COLstring SSObase64Decode(const COLstring& String) {
   COL_FUNCTION(SSObase64Decode);
   COLstring Decoded;
   COLstring Padded = String;
   const int pad    = (4 - Padded.size() % 4) % 4;
   for(int i = 0; i < pad; ++i) Padded += "=";
   Padded.replace("-", "+");
   Padded.replace("_", "/");
   SFIbase64Decode(Padded, &Decoded);
   return Decoded;
}

static EVP_PKEY* SSOevpFromNE(const COLstring& N, const COLstring& E) {
   COL_FUNCTION(SSOevpFromNE);
   EVP_PKEY*     pkey = nullptr;
   EVP_PKEY_CTX* ctx  = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
   if(!ctx) return nullptr;
   if(EVP_PKEY_fromdata_init(ctx) <= 0) {
      EVP_PKEY_CTX_free(ctx);
      return nullptr;
   }
   BIGNUM* bn_n = BN_bin2bn(reinterpret_cast<const unsigned char*>(N.data()), N.size(), NULL);
   BIGNUM* bn_e = BN_bin2bn(reinterpret_cast<const unsigned char*>(E.data()), E.size(), NULL);
   if(!bn_n || !bn_e) {
      EVP_PKEY_CTX_free(ctx);
      if(bn_n) BN_free(bn_n);
      if(bn_e) BN_free(bn_e);
      return nullptr;
   }
   OSSL_PARAM_BLD* bld    = OSSL_PARAM_BLD_new();
   if(bld) {
      OSSL_PARAM* params = nullptr;
      OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, bn_n);
      OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, bn_e);
      params = OSSL_PARAM_BLD_to_param(bld);
      EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_PUBLIC_KEY, params);
      OSSL_PARAM_free(params);
      OSSL_PARAM_BLD_free(bld);
   }
   EVP_PKEY_CTX_free(ctx);
   BN_free(bn_n);
   BN_free(bn_e);
   return pkey;
}

static bool SSOverifySignature(const COLstring& HeaderB64, const COLstring& PayloadB64, const COLstring& Sig,
                               const COLstring& N_b64, const COLstring& E_b64) {
   COL_FUNCTION(SSOverifySignature);
   auto n = SSObase64Decode(N_b64);
   auto e = SSObase64Decode(E_b64);

   while(!n.is_null() && n[0] == 0x00) n.remove(0, 1);

   EVP_PKEY* pkey = SSOevpFromNE(n, e);
   if(!pkey) {
      COL_ERR("failed to build EVP_PKEY");
      return false;
   }
   const COLstring SigningInput = HeaderB64 + "." + PayloadB64;

   EVP_MD_CTX* ctx = EVP_MD_CTX_new();
   bool        ok  = false;
   if(EVP_DigestVerifyInit(ctx, NULL, EVP_sha256(), NULL, pkey) == 1 &&
      EVP_DigestVerifyUpdate(ctx, SigningInput.data(), SigningInput.size()) == 1 &&
      EVP_DigestVerifyFinal(ctx, reinterpret_cast<const unsigned char*>(Sig.data()), Sig.size()) == 1) {
      ok = true;
   }
   EVP_MD_CTX_free(ctx);
   EVP_PKEY_free(pkey);

   if(!ok) COL_ERR("signature check failed");
   return ok;
}

static bool SSOfetchJwks(const COLstring& Url, COLvar* pJwks, COLstring& Error) {
   COL_FUNCTION(SSOfetchJwks);
   WEBrequest Req;
   Req.Url                   = Url;
   Req.Headers["Accept"]     = "*";
   Req.Headers["User-Agent"] = "IguanaX-SSO";
   Req.Timeout               = 10000;
   WEBresponse Resp;
   WEBrequestBlockingGet(Req, &Resp);
   if(Resp.ReturnCode != 200) {
      Error = "Failed to fetch jwks. HTTP " + COLint32ToString(Resp.ReturnCode);
      return false;
   }
   if(!pJwks->parse(Resp.Body) || !(*pJwks)["keys"].isArray()) {
      Error = "Invalid keys format.";
      return false;
   }
   return true;
}

static bool SSOfindKeyInJwks(const COLvar& Jwks, const COLstring& Kid, COLstring& N, COLstring& E, COLstring& Error) {
   COL_FUNCTION(SSOfindKeyInJwks);
   for(const auto& it : Jwks["keys"].array()) {
      if(it["kid"].asString() == Kid) {
         const COLstring Kty = it["kty"].asString();
         const COLstring Alg = it["alg"].asString();
         if(Kty != "RSA") {
            Error = "Unsupported key type: " + Kty;
            return false;
         }
         N = it["n"].asString();
         E = it["e"].asString();
         return true;
      }
   }
   Error = Kid + " not found in JWKS.";
   return false;
}

static bool SSOdecodeJwtParts(const COLstring& Jwt, COLstring& HeaderStr, COLstring& PayloadStr, COLstring& SigStr) {
   COL_FUNCTION(SSOdecodeJwtParts);
   COLarray<COLstring> Parts;
   COLsplit(&Parts, Jwt, ".");
   if(Parts.size() != 3) return false;
   HeaderStr  = SSObase64Decode(Parts[0]);
   PayloadStr = SSObase64Decode(Parts[1]);
   SigStr     = SSObase64Decode(Parts[2]);
   if(HeaderStr.is_null() || PayloadStr.is_null() || SigStr.is_null() || SigStr.size() == 0) return false;
   return true;
}

static bool SSOparseJwtPart(const COLstring& String, COLvar* pOut, COLstring& Error) {
   COL_FUNCTION(SSOparseJwtPart);
   if(pOut->parse(String)) return true;
   Error = "Failed to parse JWT string.";
   return false;
}

static bool SSOvalidateCommonClaims(const COLvar& Payload, const COLstring& ClientId, COLstring& Error) {
   COL_FUNCTION(SSOvalidateCommonClaims);
   const time_t    Now    = time(NULL);
   const long long Leeway = 60;
   if(Payload["aud"].asString() != ClientId) {
      Error = "Audience mismatch: " + Payload["aud"].asString();
      return false;
   }
   const COLstring Exp = Payload["exp"].asString();
   if(!Exp.is_null() && Now > atoll(Exp.c_str()) + Leeway) {
      Error = "Token expired.";
      return false;
   }
   const COLstring Nbf = Payload["nbf"].asString();
   if(!Nbf.is_null() && Now + Leeway < atoll(Nbf.c_str())) {
      Error = "Token not yet valid.";
      return false;
   }
   const COLstring Iat = Payload["iat"].asString();
   if(!Iat.is_null()) {
      const long long MaxAge = 3600;
      const long long iat    = atoll(Iat.c_str());
      if(iat > Now + Leeway) {
         Error = "Issued-at time is in the future.";
         return false;
      }
      if(Now > iat + MaxAge + Leeway) {
         Error = "Token issued too long ago.";
         return false;
      }
   }
   return true;
}

static bool SSOvalidateClaimsMicrosoft(COLvar& Payload, const COLstring& ClientId, COLstring& Error) {
   COL_FUNCTION(SSOvalidateClaimsMicrosoft);
   const COLstring Iss = Payload["iss"].asString();
   if(Iss.find("https://login.microsoftonline.com/") != 0) {
      Error = "Issuer mismatch: " + Iss;
      return false;
   }
   if(!SSOvalidateCommonClaims(Payload, ClientId, Error)) return false;
   Payload["email"]    = Payload["preferred_username"];
   const COLstring Oid = Payload["oid"].asString();
   const COLstring Tid = Payload["tid"].asString();
   if(Oid.is_null() || Tid.is_null()) {
      Payload["iguana_unique_id"] = Payload["sub"].asString();
   } else {
      Payload["iguana_unique_id"] = Oid + "_" + Tid;
   }
   return true;
}

bool SSOverifyToken(const COLstring& Jwt, const COLstring& ClientId, COLvar* pOut, COLstring& Error, const SSOprovider Provider) {
   COL_FUNCTION(SSOverifyToken);
   COLstring HeaderStr, PayloadStr, SigStr;
   if(!SSOdecodeJwtParts(Jwt, HeaderStr, PayloadStr, SigStr)) {
      Error = "JWT decode step failed.";
      return false;
   }
   COLvar HeaderVar, PayloadVar;
   if(!SSOparseJwtPart(HeaderStr, &HeaderVar, Error)) return false;
   if(!SSOparseJwtPart(PayloadStr, &PayloadVar, Error)) return false;

   const COLstring Kid    = HeaderVar["kid"].asString();
   const COLstring Backup = "https://login.microsoftonline.com/common/discovery/v2.0/keys";
   const COLstring Url    = SSOdocLoaded(Provider) ? SSOgetDiscoveryDoc()["jwks_uri"].asString() : Backup;

   const time_t Now    = time(NULL);
   const bool UseCache = Now - g_Jwks[Provider].m_CacheTime < JWKS_CACHE_DURATION && !g_Jwks[Provider].m_Jwks.isNull();
   if(!UseCache) {
      if(!SSOfetchJwks(Url, &g_Jwks[Provider].m_Jwks, Error)) return false;
      g_Jwks[Provider].m_CacheTime = Now;
   }

   COLstring N_b64, E_b64;
   if(!SSOfindKeyInJwks(g_Jwks[Provider].m_Jwks, Kid, N_b64, E_b64, Error)) return false;

   COLarray<COLstring> Parts;
   COLsplit(&Parts, Jwt, ".");
   if(!SSOverifySignature(Parts[0], Parts[1], SigStr, N_b64, E_b64)) {
      Error = "Signature verification failed.";
      return false;
   }

   if(Provider == MICROSOFT) {
      if(!SSOvalidateClaimsMicrosoft(PayloadVar, ClientId, Error)) return false;
   }
   *pOut = PayloadVar;
   COL_VAR(pOut->json(1));
   return true;
}