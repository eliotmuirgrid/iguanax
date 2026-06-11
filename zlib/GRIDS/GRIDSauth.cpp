//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDauth.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLendian.h>
#include <COL/COLlog.h>
#include <COL/COLminimumInclude.h>
#include <GRID/GRIDframe.h>
#include <GRID/GRIDutils.h>
#include <GRID/GRIDwebSocket.h>
#include <GRIDS/GRIDSauth.h>
#include <GRIDS/GRIDSsecurity.h>
#include <SFI/SFIaesDecryptor.h>
#include <SFI/SFIaesEncryptor.h>
#include <TUN/TUNdiffyHellmanSecret.h>
#include <TUN/TUNiv.h>
#include <TUN/TUNkey.h>
COL_LOG_MODULE;

static void GRIDSappendVersionAndType(COLstring* pPayload, const COLuint8 MsgType) {
   const COLuint16 Version = COLsystemToBigEndian((COLuint16)1);
   pPayload->append((const char*)&Version, sizeof(Version));
   pPayload->append((const char*)&MsgType, sizeof(MsgType));
}

static void GRIDSappendLengthPrefixedData(COLstring* pPayload, const COLstring& Data) {
   const COLuint16 DataLen = COLsystemToBigEndian((COLuint16)Data.size());
   pPayload->append((const char*)&DataLen, sizeof(DataLen));
   pPayload->append(Data);
}

static void GRIDSappendCRC(COLstring* pPayload) {
   const COLuint32 Crc = COLsystemToBigEndian(GRIDcalculateCRC32(pPayload->c_str(), pPayload->size()));
   pPayload->append((const char*)&Crc, sizeof(Crc));
}

static bool GRIDSparseVersionAndType(const COLstring& Frame, size_t* pOffset, const COLuint8 ExpectedType) {
   COL_FUNCTION(GRIDSparseVersionAndType);
   if(Frame.size() < 3) {
      COL_TRC("Frame too small");
      return false;
   }
   COLuint16 Version;
   memcpy(&Version, Frame.c_str() + *pOffset, sizeof(Version));
   Version = COLbigEndianToSystem(Version);
   *pOffset += sizeof(Version);
   COL_TRC("Version: " << Version);
   COLuint8 MsgType;
   memcpy(&MsgType, Frame.c_str() + *pOffset, sizeof(MsgType));
   *pOffset += sizeof(MsgType);
   if(MsgType != ExpectedType) {
      COL_TRC("Invalid message type: " << (int)MsgType);
      return false;
   }
   return true;
}

static bool GRIDSparseLengthPrefixedData(const COLstring& Frame, size_t* pOffset, COLstring* pOut) {
   COL_FUNCTION(GRIDSparseLengthPrefixedData);
   COLuint16 DataLen;
   memcpy(&DataLen, Frame.c_str() + *pOffset, sizeof(DataLen));
   DataLen = COLbigEndianToSystem(DataLen);
   *pOffset += sizeof(DataLen);
   COL_TRC("Data length: " << DataLen);
   if(*pOffset + DataLen > Frame.size()) {
      COL_TRC("Invalid data length");
      return false;
   }
   *pOut = Frame.substr(*pOffset, DataLen);
   *pOffset += DataLen;
   return true;
}

static bool GRIDSverifyCRC(const COLstring& Frame, const size_t PayloadSize) {
   COL_FUNCTION(GRIDSverifyCRC);
   if(PayloadSize + 4 > Frame.size()) {
      COL_TRC("Frame too small for CRC");
      return false;
   }
   COLuint32 ReceivedCrc;
   memcpy(&ReceivedCrc, Frame.c_str() + PayloadSize, sizeof(ReceivedCrc));
   ReceivedCrc                   = COLbigEndianToSystem(ReceivedCrc);
   const COLstring FrameForCrc   = Frame.substr(0, PayloadSize);
   const COLuint32  CalculatedCrc = GRIDcalculateCRC32(FrameForCrc.c_str(), FrameForCrc.size());
   COL_TRC("Received CRC: " << ReceivedCrc << ", calculated CRC: " << CalculatedCrc);
   if(ReceivedCrc != CalculatedCrc) {
      COL_TRC("CRC mismatch");
      return false;
   }
   return true;
}

COLstring GRIDSbuildChallengeFrame(const TUNkey& MyStaticPrivateKey, const TUNkey& MyEphemeralPrivateKey) {
   COL_FUNCTION(GRIDSbuildChallengeFrame);
   const COLstring StaticPublicKeyPem    = GRIDSgetPublicKeyPem(MyStaticPrivateKey);
   const COLstring EphemeralPublicKeyPem = GRIDSgetPublicKeyPem(MyEphemeralPrivateKey);
   COL_TRC("Static public key size: " << StaticPublicKeyPem.size());
   COL_TRC("Ephemeral public key size: " << EphemeralPublicKeyPem.size());
   COLstring RandomChallenge;
   TUNiv(&RandomChallenge, 16);
   COL_TRC("Random challenge size: " << RandomChallenge.size());
   const COLstring DataToSign = EphemeralPublicKeyPem + RandomChallenge;
   const COLstring Signature  = GRIDSsign(MyStaticPrivateKey, DataToSign);
   COL_TRC("Signature size: " << Signature.size());
   COLstring Payload;
   GRIDSappendVersionAndType(&Payload, GRIDWS_MSG_AUTH_CHALLENGE);
   GRIDSappendLengthPrefixedData(&Payload, StaticPublicKeyPem);
   GRIDSappendLengthPrefixedData(&Payload, EphemeralPublicKeyPem);
   GRIDSappendLengthPrefixedData(&Payload, RandomChallenge);
   GRIDSappendLengthPrefixedData(&Payload, Signature);
   GRIDSappendCRC(&Payload);
   COL_TRC("Built AUTH_CHALLENGE frame, size=" << Payload.size());
   return Payload;
}

COLstring GRIDSbuildResponseFrame(const TUNkey& MyStaticPrivateKey, const TUNkey& MyEphemeralPrivateKey,
                                     const GRIDauthChallengeFrame& Challenge, const TUNkey& ServerEphemeralPublicKey) {
   COL_FUNCTION(GRIDSbuildResponseFrame);
   const COLstring StaticPublicKeyPem    = GRIDSgetPublicKeyPem(MyStaticPrivateKey);
   const COLstring EphemeralPublicKeyPem = GRIDSgetPublicKeyPem(MyEphemeralPrivateKey);
   COL_TRC("Static public key size: " << StaticPublicKeyPem.size());
   COL_TRC("Ephemeral public key size: " << EphemeralPublicKeyPem.size());
   const COLstring SharedSecret = TUNdiffyHellmanSecret(MyEphemeralPrivateKey, ServerEphemeralPublicKey);
   COLstring       EncryptedResponse;
   EncryptedResponse.setCapacity(16);
   EncryptedResponse.setSize(16);
   const SFIaesEncryptor Encryptor((const unsigned char*)SharedSecret.c_str(), SharedSecret.size());
   Encryptor.ProcessBlock((const unsigned char*)Challenge.EncryptedChallenge.c_str(),
                          (unsigned char*)EncryptedResponse.get_buffer());
   COL_TRC("Encrypted response size: " << EncryptedResponse.size());
   const COLstring ServerEphemeralKeyPem = GRIDSgetPublicKeyPem(ServerEphemeralPublicKey);
   const COLstring DataToSign = ServerEphemeralKeyPem + EphemeralPublicKeyPem + EncryptedResponse;
   const COLstring Signature  = GRIDSsign(MyStaticPrivateKey, DataToSign);
   COL_TRC("Signature size: " << Signature.size());
   COLstring Payload;
   GRIDSappendVersionAndType(&Payload, GRIDWS_MSG_AUTH_RESPONSE);
   GRIDSappendLengthPrefixedData(&Payload, StaticPublicKeyPem);
   GRIDSappendLengthPrefixedData(&Payload, EphemeralPublicKeyPem);
   GRIDSappendLengthPrefixedData(&Payload, EncryptedResponse);
   GRIDSappendLengthPrefixedData(&Payload, Signature);
   GRIDSappendCRC(&Payload);
   COL_TRC("Built AUTH_RESPONSE frame, size=" << Payload.size());
   return Payload;
}

COLstring GRIDSbuildSuccessFrame(const COLstring& SessionKey) {
   COL_FUNCTION(GRIDSbuildSuccessFrame);
   COLstring Payload;
   GRIDSappendVersionAndType(&Payload, GRIDWS_MSG_AUTH_SUCCESS);
   const COLstring Mac = GRIDScomputeHMAC(SessionKey, Payload);
   GRIDSappendLengthPrefixedData(&Payload, Mac);
   GRIDSappendCRC(&Payload);
   COL_TRC("Built AUTH_SUCCESS frame, size=" << Payload.size());
   return Payload;
}

bool GRIDSparseChallengeFrame(const COLstring& Frame, GRIDauthChallengeFrame* pOut) {
   COL_FUNCTION(GRIDSparseChallengeFrame);
   COL_TRC("Parsing AUTH_CHALLENGE frame, size=" << Frame.size());
   size_t Offset = 0;
   if(!GRIDSparseVersionAndType(Frame, &Offset, GRIDWS_MSG_AUTH_CHALLENGE))        { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->StaticPublicKeyPem))    { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->EphemeralPublicKeyPem)) { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->EncryptedChallenge))    { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->Signature))             { return false; }
   if(!GRIDSverifyCRC(Frame, Offset))                                              { return false; }
   TUNkey StaticPublicKey;
   GRIDSloadPublicKeyPem(pOut->StaticPublicKeyPem, &StaticPublicKey);
   const COLstring DataToVerify = pOut->EphemeralPublicKeyPem + pOut->EncryptedChallenge;
   if(!GRIDSverifySignature(StaticPublicKey, DataToVerify, pOut->Signature)) {
      COL_TRC("Signature verification failed");
      return false;
   }
   COL_TRC("Successfully parsed and verified AUTH_CHALLENGE frame");
   return true;
}

bool GRIDSparseResponseFrame(const COLstring& Frame, const COLstring& ServerEphemeralKeyPem, GRIDauthResponseFrame* pOut) {
   COL_FUNCTION(GRIDSparseResponseFrame);
   COL_TRC("Parsing AUTH_RESPONSE frame, size=" << Frame.size());
   size_t Offset = 0;
   if(!GRIDSparseVersionAndType(Frame, &Offset, GRIDWS_MSG_AUTH_RESPONSE))         { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->StaticPublicKeyPem))    { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->EphemeralPublicKeyPem)) { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->DecryptedChallenge))    { return false; }
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &pOut->Signature))             { return false; }
   if(!GRIDSverifyCRC(Frame, Offset))                                              { return false; }
   TUNkey StaticPublicKey;
   GRIDSloadPublicKeyPem(pOut->StaticPublicKeyPem, &StaticPublicKey);
   const COLstring DataToVerify = ServerEphemeralKeyPem + pOut->EphemeralPublicKeyPem + pOut->DecryptedChallenge;
   if(!GRIDSverifySignature(StaticPublicKey, DataToVerify, pOut->Signature)) {
      COL_TRC("Signature verification failed");
      return false;
   }
   COL_TRC("Successfully parsed and verified AUTH_RESPONSE frame");
   return true;
}

bool GRIDSverifyResponse(const TUNkey& MyEphemeralPrivateKey, const GRIDauthResponseFrame& Response,
                         const COLstring& OriginalChallenge, const TUNkey& ClientEphemeralPublicKey,
                         const COLstring& ServerEphemeralKeyPem) {
   COL_FUNCTION(GRIDSverifyResponse);
   TUNkey ClientStaticPublicKey;
   GRIDSloadPublicKeyPem(Response.StaticPublicKeyPem, &ClientStaticPublicKey);
   const COLstring DataToVerify = ServerEphemeralKeyPem + Response.EphemeralPublicKeyPem + Response.DecryptedChallenge;
   if(!GRIDSverifySignature(ClientStaticPublicKey, DataToVerify, Response.Signature)) {
      COL_TRC("Static signature verification failed in GRIDSverifyResponse");
      return false;
   }
   COL_TRC("Static signature verified successfully");
   const COLstring SharedSecret = TUNdiffyHellmanSecret(MyEphemeralPrivateKey, ClientEphemeralPublicKey);
   COLstring       DecryptedResponse;
   DecryptedResponse.setCapacity(16);
   DecryptedResponse.setSize(16);
   const SFIaesDecryptor Decryptor((unsigned char*)SharedSecret.c_str(), SharedSecret.size());
   Decryptor.ProcessBlock((const unsigned char*)Response.DecryptedChallenge.c_str(), (unsigned char*)DecryptedResponse.get_buffer());
   if(DecryptedResponse != OriginalChallenge) {
      COL_TRC("Challenge verification failed");
      return false;
   }
   COL_TRC("Authentication successful");
   return true;
}

bool GRIDSverifySuccessFrame(const COLstring& Frame, const COLstring& SessionKey) {
   COL_FUNCTION(GRIDSverifySuccessFrame);
   COL_TRC("Verifying AUTH_SUCCESS frame, size=" << Frame.size());
   size_t Offset = 0;
   if(!GRIDSparseVersionAndType(Frame, &Offset, GRIDWS_MSG_AUTH_SUCCESS)) { return false; }
   COLstring ReceivedMac;
   if(!GRIDSparseLengthPrefixedData(Frame, &Offset, &ReceivedMac)) { return false; }
   if(!GRIDSverifyCRC(Frame, Offset)) { return false; }
   const COLstring PayloadToVerify = Frame.substr(0, 3);
   const COLstring ComputedMac     = GRIDScomputeHMAC(SessionKey, PayloadToVerify);
   if(ReceivedMac != ComputedMac) {
      COL_TRC("AUTH_SUCCESS MAC verification failed");
      return false;
   }
   COL_TRC("AUTH_SUCCESS verified successfully");
   return true;
}
