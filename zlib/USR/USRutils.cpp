
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/12/24 10:21 AM
//  ---------------------------------------------------------------------------
#include <USR/USRutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool USRdoesContainDigit(const COLstring& S){
	COL_FUNCTION(USRdoesContainDigit);
	for (int i = 0; i < S.size(); i++){ if(COLisdigit(S[i])){ return true; }}
	return false;
}

bool USRvalidPassword(const COLstring& Password, COLstring* pErr) {
	COL_FUNCTION(USRvalidPassword);
	*pErr = "Password must contain ";
	if (Password.size() < 8)                      { pErr->append("at least 8 characters"); return false; }
	if (Password.equals(COLlowerCase(Password)))  { pErr->append("an uppercase letter");   return false; }
	if (Password.equals(COLupperCase(Password)))  { pErr->append("a lowercase letter");    return false; }
	if (!USRdoesContainDigit(Password))           { pErr->append("a numerical digit");     return false; }
	return true;
}