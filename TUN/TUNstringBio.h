#ifndef __TUN_STRING_BIO_H__
#define __TUN_STRING_BIO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNstringBio
//
// Description:
//
// A COLstring compatible BIO for use with openssl.  This is for writing data.
//
// Author: Eliot Muir 
// Date:   Monday 12 June 2023 - 03:30PM
// ---------------------------------------------------------------------------

class COLstring;
struct bio_method_st;
struct bio_st;
class TUNstringBio{
public:
   TUNstringBio(COLstring* pString);
   ~TUNstringBio();

   bio_st* m_pBio;
private:
   struct bio_method_st* m_pBioMethod;
};

class TUNstringReadBio{
public:
   TUNstringReadBio(const COLstring& String);
   ~TUNstringReadBio();   
   bio_st* m_pBio;   
};

#endif // end of defensive include
