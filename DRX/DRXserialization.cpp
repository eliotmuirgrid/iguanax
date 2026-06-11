//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXserialization
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, May 13th, 2008 @ 02:21:53 PM
//
//---------------------------------------------------------------------------
#include "DRXserialization.h"
#include "DRXinformation.h"
#include "DRXfilterDecrypt.h"
#include "DRXfilterEncrypt.h"

#include <SFI/SFIhexDecodeFilter.h>
#include <SFI/SFIhexEncodeFilter.h>
#include <SFI/SFIhexValidFilter.h>
#include <SFI/SFImd5.h>

#include <COL/COLsinkString.h>
#include <COL/COLstring.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring DRXserializeLicenseData(const DRXinformation& License) {
   COL_FUNCTION(DRXserializeLicenseData);
   COLstring Out;
   COLostream Stream(Out);
   Stream << License.expiryTimestamp() << ";" << License.componentLimit() << ";" << License.logSearchLimit() << ";" << License.messageRateLimit();
   // if all features are off, don't serialize features, so it's identical to the old feature-less license
   if (!License.allFeaturesOff()) {
      COLstring FeatureData;
      License.features().toBinary(&FeatureData);
      // can't guarantee FeatureData doesn't contain ';', so we need include FeatureData size
      Stream << ";" << FeatureData.size() << ";" << FeatureData;
   }
   COL_DBG_DUMP("license data", Out.data(), Out.size());
   return Out;
}

void DRXserializeLicense(const DRXinformation& License, const COLstring& IguanaId, COLstring* pOut) {
   COL_FUNCTION(DRXserializeLicense);
   COLstring LicenseString = DRXserializeLicenseData(License);

   // Encrypt the serialized license instance, and convert to HEX so that it may be easily copied/pasted.
   COLsinkString      EncryptedLicenseSink;
   SFIhexEncodeFilter ToHexFilter(&EncryptedLicenseSink);
   DRXfilterEncrypt   EncryptFilter(&ToHexFilter);
   COL_VAR3(&EncryptedLicenseSink, &ToHexFilter, &EncryptFilter);

   EncryptFilter.setEncryptionKey((const unsigned char*)IguanaId.c_str(), IguanaId.size());
   EncryptFilter.write(LicenseString.c_str(), (COLuint32)LicenseString.size());
   EncryptFilter.flush();

   // hash of the product name to ensure that this license is used only for the specified product.
   COLstring ProductNameHash = SFImd5((void*)License.ProductName.c_str(), License.ProductName.size());
   // hash of the license version for the version check during decryption.
   COLstring Version = COLintToString(License.Version);
   COLstring LicenseVersionHash = SFImd5((void*)Version.c_str(), Version.size());

   // hash of the entire license to detect possible data corruption.
   COLstring LicenseData = ProductNameHash + LicenseVersionHash;
   LicenseData.write(EncryptedLicenseSink.string().data(), EncryptedLicenseSink.string().size());
   COLstring LicenseDataHash = SFImd5(LicenseData);
   *pOut = LicenseDataHash + ProductNameHash + LicenseVersionHash;
   pOut->append((const char*) EncryptedLicenseSink.string().data(), EncryptedLicenseSink.string().size());
}

// assumes Data begins with "FeatureMap Size ; FeatureMap Binary"
// there may be more license parameter after Features in future versions of license code. the rest is returned through Data
static bool DRXparseLicenseFeatures(COLstring& Data, COLvar* pFeatureMap, COLstring* pError) {
   COLstring Field, Rest;
   // extract the size of FeatureMap Binary
   if (!Data.split(Field, Rest, ";"))                                                     { *pError = "Failed to extract Feature Map Size."; return false; }
   int FeatureSize = atoi(Field.c_str());
   if (FeatureSize <= 0)                                                                  { *pError = "Invalid Feature Map Size parameter."; return false; }
   // extract FeatureMap COLvar using the FeatureMap Binary size
   // can't use ";" here because FeatureMap Binary may contain ';'
   int Consumed = 0;
   if (COLvar::kSUCCESS != pFeatureMap->parseBinary(Rest.data(), FeatureSize, &Consumed)) { *pError = "Failed to extract Feature Map.";      return false; }
   if (FeatureSize != Consumed)                                                           { *pError = "Failed to extract Feature Map.";      return false; }
   // truncate FeatureMap Size + ';' + FeatureMap Binary
   Data.shift(Field.size() + 1 + FeatureSize);
   return true;
}

static bool DRXparseLicenseDataEx(const COLstring& Data, DRXinformation* pLicense, COLstring* pError) {
   COL_FUNCTION(DRXparseLicenseDataEx);
   COLstring Field, Rest;

   if (!Data.split(Field, Rest, ";"))        { *pError = "Failed to extract Timestamp.";                    return false; }
   COL_VAR(Field);
   time_t Timestamp = atoll(Field.c_str());
   if (Timestamp < 0)                        { *pError = "Invalid Timestamp parameter.";                    return false; }

   if (!Rest.split(Field, Rest, ";"))        { *pError = "Failed to extract ComponentLimit parameter.";     return false; }
   int ComponentLimit = atoi(Field.c_str());
   if (ComponentLimit < 0)                   { *pError = "Invalid ComponentLimit parameter.";               return false; }

   if (!Rest.split(Field, Rest, ";"))        { *pError = "Failed to extract Log Search Limit parameter.";   return false; }
   int LogSearchLimit = atoi(Field.c_str());
   if (LogSearchLimit < 0)                   { *pError = "Invalid Log Search Limit parameter.";             return false; }

   if (!Rest.split(Field, Rest, ";"))        { *pError = "Failed to extract Message Rate Limit parameter."; return false; }
   int MessageRateLimit = atoi(Field.c_str());
   if (MessageRateLimit < 0)                 { *pError = "Invalid Message Rate Limit parameter.";           return false; }

   COLvar FeatureMap;
   if (!DRXparseLicenseFeatures(Rest, &FeatureMap, pError)) { return false; }

   // Rest should be empty here, but don't check so that we can read future versions of license code that has even more parameters

   *pLicense = DRXinformation(Timestamp, ComponentLimit, LogSearchLimit, MessageRateLimit, FeatureMap);
   return true;
}

static bool DRXparseLicenseData(const COLstring& Data, DRXinformation* pLicense, COLstring* pError) {
   COL_FUNCTION(DRXparseLicenseData);
   COLstring Field, Rest;
   if (!Data.split(Field, Rest, ";"))        { *pError = "Failed to extract Timestamp.";  return false; }
   COL_VAR(Field);
   time_t Timestamp = atoll(Field.c_str());
   if (Timestamp < 0)                        { *pError = "Invalid Timestamp parameter.";  return false; }
   if (!Rest.split(Field, Rest, ";"))        { *pError = "Failed to extract ComponentLimit parameter.";   return false; }
   int ComponentLimit = atoi(Field.c_str());
   if (ComponentLimit < 0)                   { *pError = "Invalid ComponentLimit parameter.";             return false; }
   if (!Rest.split(Field, Rest, ";"))        { *pError = "Failed to extract Log Search Limit parameter."; return false; }
   int LogSearchLimit = atoi(Field.c_str());
   if (LogSearchLimit < 0)                   { *pError = "Invalid Log Search Limit parameter.";             return false; }
   if ( Rest.split(Field, Rest, ";"))        { *pError = "Failed to extract Message Rate Limit parameter."; return false; }
   int MessageRateLimit = atoi(Field.c_str());
   if (MessageRateLimit < 0)                   { *pError = "Invalid Message Rate Limit parameter.";         return false; }
   *pLicense = DRXinformation(Timestamp, ComponentLimit, LogSearchLimit, MessageRateLimit);
   return true;
}

void DRXparseLicense(const COLstring& License, const COLstring& IguanaId, DRXinformation* pOut) {
   COL_FUNCTION(DRXparseLicense);

   COLsinkString CleanedBuffer;
   SFIhexValidFilter ValidHexFilter( &CleanedBuffer );
   ValidHexFilter.write(License.c_str(), (COLuint32)License.size());

   const COLuint32 SizeOfHashBlockBinary = DRX_AES_SIZE_OF_BLOCK;
   const COLuint32 SizeOfHashBlockHex = 2 * SizeOfHashBlockBinary; // 2 HEX characters per binary byte.
   const COLuint32 CountOfHashBlock = 3; // 1 block for entire license data hash, 1 block for product name hash,
                                         // and 1 block for license version hash. Encryption/decryption key hash
                                         // block is embedded in the encrypted license.

   if ( CleanedBuffer.string().size() < (CountOfHashBlock * SizeOfHashBlockHex) ) {
      COL_ERROR_STREAM_PLAIN( "License string is too short.", COLerror::PreCondition );
   }

   // Converting both hash blocks back to binary in order to compare hash values
   // before decryption.
   COLsinkString CombinedHashSink;

   SFIhexDecodeFilter CombinedHashFromHexFilter( &CombinedHashSink );
   // We write out CountOfHashBlock * SizeOfHashBlockHex from the CleanedBuffer to the CombinedHashFromHexFilter. This used to be done
   // chunk by chunk using a COLsimpleBuffer/DRXsimpleBuffer but the licenses are small enough that this shouldn't matter
   CombinedHashFromHexFilter.write(CleanedBuffer.string().data(), CountOfHashBlock * SizeOfHashBlockHex);

   // First block is the hash of the entire license data.
   SFImd5Hasher LicenseDataHasher;
   LicenseDataHasher.write( CleanedBuffer.string().data() + SizeOfHashBlockHex, CleanedBuffer.string().size() - SizeOfHashBlockHex );
   if ( ::memcmp( LicenseDataHasher.hash().data(), CombinedHashSink.string().data(), SizeOfHashBlockBinary ) != 0 ) {
      COL_ERROR_STREAM_PLAIN( "Your IguanaX license is corrupted." << newline
                               << "Contact the iNTERFACEWARE Sales Office to obtain a new license." << newline,
                               COLerror::PreCondition );
   }

   // Second block is the hash of the product name.
   SFImd5Hasher ProductNameHasher;
   ProductNameHasher.write(DRXinformation::ProductName.c_str(), DRXinformation::ProductName.size());
   if ( ::memcmp(ProductNameHasher.hash().data(),
                 CombinedHashSink.string().data() + SizeOfHashBlockBinary,
                 SizeOfHashBlockBinary) != 0 )
   {
      COL_ERROR_STREAM_PLAIN( "This license is not supported by IguanaX" << newline
                               << "Contact the iNTERFACEWARE Sales Office to obtain a license for IguanaX" << newline,
                               COLerror::PreCondition );
   }

   // Third block is the hash of the license version.
   SFImd5Hasher LicenseVersionHasher;
   COLstring Version = COLintToString(pOut->Version);
   LicenseVersionHasher.write(Version.c_str(), Version.size() );
   if ( ::memcmp(LicenseVersionHasher.hash().data(),
                 CombinedHashSink.string().data() + SizeOfHashBlockBinary + SizeOfHashBlockBinary,
                 SizeOfHashBlockBinary) != 0 )
   {
      COL_ERROR_STREAM_PLAIN( "The version of your IguanaX license is no longer supported." << newline
                               << "Contact the iNTERFACEWARE Sales Office to obtain a new license version." << newline,
                               COLerror::PreCondition );
   }

   // Decrypt the actual license data into the serialized license string format
   // used during encryption.
   COLsinkString SerializedLicenseSink;
   DRXfilterDecrypt DecryptFilter(&SerializedLicenseSink);
   DecryptFilter.setDecryptionKey((const unsigned char*)IguanaId.c_str(), IguanaId.size());
   SFIhexDecodeFilter FromHexFilter(&DecryptFilter);
   try {
      // Here we write from the CleanedBuffer starting at position (CountOfHashBlock * SizeOfHashBlockHex) and writing out the remainder of the buffer
      // (which is CleanedBuffer.string().size() - (CountOfHashBlock * SizeOfHashBlockHex)) to the FromHexFilter.
      // This used to be done using COLsimpleBuffer/DRXsimpleBuffer, and so worked in a chunk by chunk manner, but with the size of licenses it shouldn't matter to just
      // write it all out at once.
      FromHexFilter.write(CleanedBuffer.string().data() + (CountOfHashBlock * SizeOfHashBlockHex), CleanedBuffer.string().size() - (CountOfHashBlock * SizeOfHashBlockHex));
   } catch(const COLerror& Error) {
      if (Error.code() == DRX_KEY_MISMATCH) {
         //throw the correct message, this means Iguana ID mismatch
         COL_ERROR_STREAM( "The license code was created for another Iguana instance and cannot be used here.", DRX_IGUANA_ID_MISMATCH );
      } else {
         throw;
      }
   }

   if ( SerializedLicenseSink.string().size() == 0 ) {
      COL_ERROR_STREAM_PLAIN( "License string did not contain any actual license data.", COLerror::PreCondition );
   }

   COLstring Error;
   // pretend we have a new license with feature map
   if (!DRXparseLicenseDataEx(SerializedLicenseSink.string(), pOut, &Error)) {
      // either new license has corrupted feature map or we have an old license with out feature map
      if (!DRXparseLicenseData(SerializedLicenseSink.string(), pOut, &Error)) {
         COL_ERROR_STREAM_PLAIN( "License string did not contain any actual license data. " + Error, COLerror::PreCondition );
      }
   }
}
