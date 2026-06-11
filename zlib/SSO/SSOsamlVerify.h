#ifndef _SSOSAMLVERIFY_H
#define _SSOSAMLVERIFY_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SSOsamlVerify.h
//
//  Description: SAML signature verification and validation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <libxml/tree.h>

#include <ctime>

struct SSOsamlConfig;

COLstring SSOsamlFormatCertificateToPem(const COLstring& RawCert);

bool SSOsamlVerifySignature(xmlDocPtr Doc, const COLstring& Certificate, COLstring& Error);

bool SSOsamlValidateTimestamp(xmlDocPtr Doc, COLstring& Error);
bool SSOsamlValidateAudience(xmlDocPtr Doc, const COLstring& ExpectedEntityId, COLstring& Error);

time_t SSOsamlParseISO8601(const COLstring& Timestamp);
void SSOsamlRegisterXmlIds(xmlNodePtr Node, xmlDocPtr Doc);

#endif // _SSOSAMLVERIFY_H
