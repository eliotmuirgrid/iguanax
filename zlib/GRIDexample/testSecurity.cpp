//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDsecurityExample.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GRIDS/GRIDSauth.h>
#include <GRIDS/GRIDSsecurity.h>
#include <SCK/SCKloop.h>
#include <TUN/TUNchallenge.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkeyFingerPrint.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

const COLstring PrivateKey =
"-----BEGIN PRIVATE KEY-----\n"
"MIGEAgEAMBAGByqGSM49AgEGBSuBBAAKBG0wawIBAQQglPJqPxIXxCRs4zae59GE\n"
"sl17pO+nOQWYACrGoZLf1qahRANCAASqT6uLWx9QettkBxheqpLgCeELbPxdovkx\n"
"/iQTDTSrF39iHRA2POA1Z4XO8NjSlI9em1OjeV4CDI4VHy9aXzAu\n"
"-----END PRIVATE KEY-----\n";

static void genKey(TUNkey* pKey) { TUNkeyPrivateFromPemString(PrivateKey, pKey); }

void GRIDsecurityExampleBasicEncryption() {
   COL_FUNCTION(GRIDsecurityExampleBasicEncryption);
   TUNkey AliceKey, BobKey;
   genKey(&AliceKey);
   genKey(&BobKey);
   COL_TRC("Alice fingerprint: " << TUNkeyFingerPrint(AliceKey));
   COL_TRC("Bob fingerprint: " << TUNkeyFingerPrint(BobKey));
   const COLstring Message = "Hello from Alice to Bob!";
   COL_TRC("Original: " << Message);
   const COLstring Encrypted = GRIDSencrypt(AliceKey, BobKey, Message);
   COL_TRC("Encrypted length: " << Encrypted.size());
   const COLstring Decrypted = GRIDSdecrypt(BobKey, AliceKey, Encrypted);
   COL_TRC("Decrypted: " << Decrypted);
   UNIT_ASSERT(Decrypted == Message);
}

void GRIDsecurityExampleChallengeResponse() {
   COL_FUNCTION(GRIDsecurityExampleChallengeResponse);
   TUNkey ServerKey, ClientKey;
   genKey(&ServerKey);
   genKey(&ClientKey);
   COLstring SharedSecret, RandomChallenge, EncryptedChallenge;
   TUNchallenge(ServerKey, ClientKey, &SharedSecret, &RandomChallenge, &EncryptedChallenge);
   COL_TRC("Server generated challenge, length: " << EncryptedChallenge.size());
   const COLstring ClientResponse = TUNchallengeResponse(ClientKey, ServerKey, EncryptedChallenge);
   COL_TRC("Client decrypted challenge, length: " << ClientResponse.size());
   UNIT_ASSERT(ClientResponse == RandomChallenge);
}

void GRIDsecurityExampleAuthenticationFlow() {
   COL_FUNCTION(GRIDsecurityExampleAuthenticationFlow);
   TUNkey ServerPrivateKey, ClientPrivateKey;
   // Load or generate static keys (long-term identity keys)
   genKey(&ServerPrivateKey);
   genKey(&ClientPrivateKey);
   COL_TRC("=== Step 1: Server generates ephemeral keypair and sends AUTH_CHALLENGE ===");
   TUNkey ServerEphemeralKey, ClientEphemeralKey;
   TUNgenerateEllipticalKey(&ServerEphemeralKey);
   COL_TRC("Server ephemeral key generated");
   const COLstring ChallengeFrame = GRIDSbuildChallengeFrame(ServerPrivateKey, ServerEphemeralKey);
   COL_TRC("Challenge frame size: " << ChallengeFrame.size());
   COL_TRC("=== Step 2: Client parses and verifies challenge ===");
   GRIDauthChallengeFrame Challenge;
   UNIT_ASSERT_MESSAGE(GRIDSparseChallengeFrame(ChallengeFrame, &Challenge), "Could not parse challenge frame");
   COL_TRC("Parsed server static public key, length: " << Challenge.StaticPublicKeyPem.size());
   COL_TRC("Parsed server ephemeral public key, length: " << Challenge.EphemeralPublicKeyPem.size());
   TUNkey ServerStaticPublicKey, ServerEphemeralPublicKey;
   GRIDSloadPublicKeyPem(Challenge.StaticPublicKeyPem, &ServerStaticPublicKey);
   GRIDSloadPublicKeyPem(Challenge.EphemeralPublicKeyPem, &ServerEphemeralPublicKey);
   COL_TRC("Server fingerprint: " << TUNkeyFingerPrint(ServerStaticPublicKey));
   COL_TRC("=== Step 3: Client generates ephemeral keypair and sends AUTH_RESPONSE ===");
   TUNgenerateEllipticalKey(&ClientEphemeralKey);
   COL_TRC("Client ephemeral key generated");
   const COLstring ResponseFrame = GRIDSbuildResponseFrame(ClientPrivateKey, ClientEphemeralKey, Challenge, ServerEphemeralPublicKey);
   COL_TRC("Response frame size: " << ResponseFrame.size());
   COL_TRC("=== Step 4: Server verifies response ===");
   const COLstring ServerEphemeralKeyPem = GRIDSgetPublicKeyPem(ServerEphemeralKey);
   GRIDauthResponseFrame Response;
   UNIT_ASSERT_MESSAGE(GRIDSparseResponseFrame(ResponseFrame, ServerEphemeralKeyPem, &Response), "Could not parse response frame");
   TUNkey ClientStaticPublicKey, ClientEphemeralPublicKey;
   GRIDSloadPublicKeyPem(Response.StaticPublicKeyPem, &ClientStaticPublicKey);
   GRIDSloadPublicKeyPem(Response.EphemeralPublicKeyPem, &ClientEphemeralPublicKey);
   COL_TRC("Client fingerprint: " << TUNkeyFingerPrint(ClientStaticPublicKey));

   UNIT_ASSERT_MESSAGE(GRIDSverifyResponse(ServerEphemeralKey, Response, Challenge.EncryptedChallenge, ClientEphemeralPublicKey, ServerEphemeralKeyPem), "Response verification failed.");
}

static void GRIDsecurityExampleEncryptedCommunication() {
   COL_FUNCTION(GRIDsecurityExampleEncryptedCommunication);
   TUNkey Node1Key, Node2Key;
   genKey(&Node1Key);
   genKey(&Node2Key);
   const COLstring Messages[] = {"Hello, World!", "This is a test message", "GRID failover coordination data",
                                 "{\"status\":\"active\",\"timestamp\":1234567890}"};
   for(size_t i = 0; i < sizeof(Messages) / sizeof(Messages[0]); i++) {
      const COLstring& Original = Messages[i];
      COL_TRC("--- Message " << (i + 1) << " ---");
      COL_TRC("Original (" << Original.size() << " bytes): " << Original);
      const COLstring Encrypted = GRIDSencrypt(Node1Key, Node2Key, Original);
      COL_TRC("Encrypted: " << Encrypted.size() << " bytes");
      const COLstring Decrypted = GRIDSdecrypt(Node2Key, Node1Key, Encrypted);
      COL_TRC("Decrypted (" << Decrypted.size() << " bytes): " << Decrypted);
      UNIT_ASSERT(Decrypted == Original);
   }
}

void testSecurity(UNITapp* pApp) {
   pApp->add("basic",     &GRIDsecurityExampleBasicEncryption);
   pApp->add("comm",      &GRIDsecurityExampleEncryptedCommunication);
   pApp->add("auth",      &GRIDsecurityExampleAuthenticationFlow);
   pApp->add("challenge", &GRIDsecurityExampleChallengeResponse);
}
