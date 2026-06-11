//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LNETutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   18/11/24 3:59 PM
//  ---------------------------------------------------------------------------
#include <BLUA/BLUAinstance.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <LNET/LNETutils.h>
COL_LOG_MODULE;


bool LNETcheckCertAndPrivateKey(const COLstring& Cert, const COLstring& Key, COLstring* pError){
	COL_FUNCTION(LNETcheckCertAndPrivateKey);
	if (Cert.is_null() && Key.is_null()) return true;
	if ((Cert.is_null() && !Key.is_null()) || (Key.is_null() && !Cert.is_null())){
		COL_TRC("Certificate or Private Key was provided but the other wasn't.");
		if (Cert.is_null()) { *pError = "Argument \'key\' was provided but \'cert\' was not. In order to set up HTTPS, both must be provided."; return false; }
		else                { *pError = "Argument \'cert\' was provided but \'key\' was not. In order to set up HTTPS, both must be provided."; return false; }
	}
	COL_TRC("Making sure cert and key exist.");
	if (!FILfileExists(Cert)) { *pError = "The file specified by \'cert\' was not found."; return false; }
	if (!FILfileExists(Key))  { *pError = "The file specified by \'key\' was not found.";  return false; }
	return true;
}
