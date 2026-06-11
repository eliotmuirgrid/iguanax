#ifndef __SFI_CRYPT3_H__
#define __SFI_CRYPT3_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIcrypt3
//
// Originally this class was called CRYcript3 and was in the CRY library.
// 
// However it made sense to move it over into the SFI library along with
// the MD5 code it uses.
//
// Class for generating encrypted passwords currently used
// in /etc/passwd files on unix, CVS passwords, etc. Based on
// a Java implementation of an ANSI C implementation of fcrypt
// by Eric Young, can be found at 
// http://www.dynamic.net.au/christos/crypt/UnixCrypt.txt
//
// md5crypt was borrowed and changed around to suit our various
// naming conventions and standards from this page:
// http://cvs.pld.org.pl/shadow/lib/Attic/md5crypt.c?rev=1.4
// with the following license:
//
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
// ----------------------------------------------------------------------------
//
// Note: DO NOT use crypt3 functions for standard purposes, e.g. encrypting
// a block of text. This is because the algorithms used here are
// one-way.
//
// Finally, I'd say that it's best that this class is only
// used for generating unix (et. al.)/cvs passwords. 
// Do NOT use it to generate passwords for applications, 
// etc., as the algorithm crypt uses is quite easy to break nowadays
// because of its small keysize (56 bits). Same goes for md5crypt, since
// recently someone figured out how to get the same md5 hash from
// two different strings by applying appropriate padding.
//
// crypt works by using a hybrid form of DES encryption using
// the input string as a key to encrypt a buffer of nullchars,
// while using a 2-byte "salt" to perturb the algorithm. 
// This "salt" value can be specified by you, or you
// can have the class generate a (pretty random) salt for you, which
// is generally the preferred case. If your custom salt value is
// longer than 2 bytes, only the first 2 bytes will be used.
//
// md5crypt is a hybrid form of md5; salt rules same as above, except
// the salt's max size is 8 bytes. If you specify a longer salt
// value, only the first 8 bytes will be used. 
//
// If you specify the salt yourself, it can be composed of any combination
// of the following set:
//
// a b c d e f g h i j k l m n o p q r s t u v w x y z
// A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
// 0 1 2 3 4 5 6 7 8 9 . /
//
// Usage sample:
//
//{
//   // note that usage is identical for CRYcrypt3::md5crypt()
//
//   // using randomized salt
//   COLstring MyHashedPassword;
//   SFIcrypt3::crypt("abc123", NULL, MyHashedPassword);
//
//   // using your own specified salt
//   COLstring MySaltyHashedPassword;
//   SFIcrypt3::crypt("abc123", "g7", MySaltyHashedPassword);
//}
//
// The output of crypt is a 13-byte string whose first
// two bytes are the salt that was used to perturb the algorithm.
// Indeed, this is how unix checks if the password you entered is
// correct -- takes first two bytes of password from /etc/passwd, encrypts
// the password you entered, and compares to the encrypted one to
// whatever the result is.
//
// md5crypt outputs a string prepended by $1$, appended by a salt followed by $, and
// a 22-byte hash.
//
// At the time of this writing, crypt and md5crypt have been successfully tested
// to generate valid unix user passwords. crypt, in addition, generates valid
// cvs/samba/etc. passwords.
//
// Notes for the future:
// ---------------------
// OpenBSD uses another algorithm called bcrypt, which is a variant of
// the Blowfish encryption algorithm. If we ever decide to support this platform,
// it is required that bcrypt is added to this library in order to be able
// to generate valid OpenBSD system passwords.
//
// Description:
//
// SFIcrypt3 class.
//
// Author: Kiril Zorin (ported to SFI by Eliot Muir)
// Date:   Tuesday, January 15th, 2013 @ 03:23:19 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SFIcrypt3
{
public:
   static void crypt(const COLstring& PlainText, const COLstring* pSalt, COLstring& ReturnCipherText); // pSalt can be NULL (see above comments)
   static void md5crypt(const COLstring& PlainText, const COLstring* pSalt, COLstring& CipherText); // pSalt can be NULL (see above comments)

private:  
   SFIcrypt3() {} // not allowed
   // no need to disallow copy constructor/assignment operator,
   // since there's no way to instantiate.

   static const COLuint32 m_DESSBox[8][64];
   static const COLuint32 m_DESPC2[8][64];
   static const COLuint32 m_DESIP[64];
   static const COLuint32 m_SaltTable[128];
   static bool m_RandomizerReset;

   // a, b, n and m are inputs to a permutation formula
   static void PERM_OP(COLuint32 a, COLuint32 b, COLuint32 n, COLuint32 m, COLuint32 KeySchedule[32]);
   static void buildKeySchedule(const unsigned char Key[8], COLuint32 KeySchedule[32]);
   static void cryptMain(const COLuint32 KeySchedule[32], COLuint32 ExpansionSwap[2], COLuint32 Cipher[2]);

   // a, n and m are inputs to a permutation formula
   static COLuint32 HPERM_OP(COLuint32 a, COLint32 n, COLuint32 m);
   static COLuint32 DESEncrypt(COLuint32 Left, COLuint32 Right, COLuint32 KeyIndex, COLuint32 ExpansionSwap[2], const COLuint32 KeySchedule[32]);

   // used for MD5crypt mapping
   static void to64(char* pString, COLuint32 Number, COLint32 Size);
};

#endif // end of defensive include
