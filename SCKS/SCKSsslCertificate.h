#ifndef __SCKS_SSL_MAKE_CERTIFICATE_H__
#define __SCKS_SSL_MAKE_CERTIFICATE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKSsslCertificate
//
// Author: John Qi
//
// Description:
//
// Utilities for making a SSL certificate.
//---------------------------------------------------------------------------

class COLstring;

// Serial defaults to 1
// Days is the number of days the certificate is valid (it's self signed)
// Bits we typically set to 2048
bool SCKSsslMakeCertificate(const COLstring& ConfigDir, int Bits, int Serial, int Days);

#endif // End of defensive include
