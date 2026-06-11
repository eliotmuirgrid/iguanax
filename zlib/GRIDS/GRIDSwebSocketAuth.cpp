//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDSwebSocketAuth.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <GRIDS/GRIDSwebSocketAuth.h>
#include <COL/COLlog.h>
#include <GRID/GRIDserialize.h>
#include <GRIDS/GRIDSauth.h>
#include <GRIDS/GRIDSsecurity.h>
#include <TUN/TUNdiffyHellmanSecret.h>
#include <TUN/TUNkey.h>
#include <TUN/TUNkeyEllipticalGen.h>
#include <TUN/TUNkeyFingerPrint.h>
COL_LOG_MODULE;

GRIDSwebSocketAuth::GRIDSwebSocketAuth(const GRIDwebSocketMode Mode, const GRIDnodeInfo& Config,
                                     const COLstring& ExpectedFingerprint)
    : m_Mode(Mode), m_NodeInfo(Config), m_Authenticated(false), m_ExpectedFingerprint(ExpectedFingerprint) {
   COL_METHOD(GRIDSwebSocketAuth::GRIDSwebSocketAuth);
   COL_TRC("Auth handler created, mode=" << (Mode == GRIDWS_MODE_CLIENT ? "client" : "server"));
}

GRIDSwebSocketAuth::~GRIDSwebSocketAuth() {
   COL_METHOD(GRIDSwebSocketAuth::~GRIDSwebSocketAuth);
}

void GRIDSwebSocketAuth::initiateServerChallenge(COLauto<COLclosure2<const void*, int>> pSendFrame) {
   COL_METHOD(GRIDSwebSocketAuth::initiateServerChallenge);
   if(m_Mode != GRIDWS_MODE_SERVER) {
      COL_TRC("Cannot initiate challenge in client mode");
      return;
   }

   if(!m_NodeInfo.m_pPrivateKey) {
      COL_TRC("Private key not set, cannot initiate challenge");
      return;
   }

   COL_TRC("Server: generating ephemeral key and building AUTH_CHALLENGE");
   TUNgenerateEllipticalKey(&m_EphemeralKey);
   const COLstring        ChallengeFrame = GRIDSbuildChallengeFrame(*m_NodeInfo.m_pPrivateKey, m_EphemeralKey);
   GRIDauthChallengeFrame Challenge;
   if(GRIDSparseChallengeFrame(ChallengeFrame, &Challenge)) {
      m_AuthChallengeRandom = Challenge.EncryptedChallenge;
      COL_TRC("Extracted challenge random for verification, length=" << m_AuthChallengeRandom.size());
   }

   pSendFrame->run(ChallengeFrame.c_str(), ChallengeFrame.size());
   COL_TRC("Sent AUTH_CHALLENGE to client");
}

void GRIDSwebSocketAuth::processChallenge(const COLstring& Frame, COLauto<COLclosure2<const void*, int>> pSendFrame) {
   COL_METHOD(GRIDSwebSocketAuth::processChallenge);
   if(m_Mode != GRIDWS_MODE_CLIENT) {
      COL_TRC("Received AUTH_CHALLENGE but not in client mode");
      return;
   }

   GRIDauthChallengeFrame Challenge;
   if(!GRIDSparseChallengeFrame(Frame, &Challenge)) {
      COL_TRC("Failed to parse AUTH_CHALLENGE frame");
      return;
   }

   TUNkey StaticPublicKey, ServerEphemeralPublicKey;
   GRIDSloadPublicKeyPem(Challenge.StaticPublicKeyPem, &StaticPublicKey);
   GRIDSloadPublicKeyPem(Challenge.EphemeralPublicKeyPem, &ServerEphemeralPublicKey);
   m_PeerFingerprint = TUNkeyFingerPrint(StaticPublicKey);
   COL_TRC("Server fingerprint: " << m_PeerFingerprint);

   if(!m_NodeInfo.m_pPrivateKey) {
      COL_TRC("Private key not set, cannot respond to challenge");
      return;
   }

   COL_TRC("Client: generating ephemeral key");
   TUNgenerateEllipticalKey(&m_EphemeralKey);
   const COLstring ResponseFrame =
       GRIDSbuildResponseFrame(*m_NodeInfo.m_pPrivateKey, m_EphemeralKey, Challenge, ServerEphemeralPublicKey);
   pSendFrame->run(ResponseFrame.c_str(), ResponseFrame.size());
   COL_TRC("Sent AUTH_RESPONSE to server");
   const COLstring SharedSecret = TUNdiffyHellmanSecret(m_EphemeralKey, ServerEphemeralPublicKey);
   m_SessionKey                 = GRIDSderiveKey(SharedSecret, COLstring(), COLstring("GRIDS-AUTH-v1"));
   COL_TRC("Derived session key for AUTH_SUCCESS verification");
}

void GRIDSwebSocketAuth::processResponse(const COLstring& Frame, COLauto<COLclosure2<const void*, int>> pSendFrame) {
   COL_METHOD(GRIDSwebSocketAuth::processResponse);
   if(m_Mode != GRIDWS_MODE_SERVER) {
      COL_TRC("Received AUTH_RESPONSE but not in server mode");
      return;
   }

   const COLstring       ServerEphemeralKeyPem = GRIDSgetPublicKeyPem(m_EphemeralKey);
   GRIDauthResponseFrame Response;
   if(!GRIDSparseResponseFrame(Frame, ServerEphemeralKeyPem, &Response)) {
      COL_TRC("Failed to parse AUTH_RESPONSE frame");
      return;
   }

   TUNkey StaticPublicKey, EphemeralPublicKey;
   GRIDSloadPublicKeyPem(Response.StaticPublicKeyPem, &StaticPublicKey);
   GRIDSloadPublicKeyPem(Response.EphemeralPublicKeyPem, &EphemeralPublicKey);
   m_PeerFingerprint = TUNkeyFingerPrint(StaticPublicKey);
   COL_TRC("Extracted peer fingerprint: " << m_PeerFingerprint);
   bool Authorized = m_PeerFingerprint == m_ExpectedFingerprint;
   COL_VAR3(Authorized, m_PeerFingerprint, m_ExpectedFingerprint);
   if(!Authorized) {
      COL_TRC("Fingerprint authorization failed, rejecting connection");
      return;
   }

   if(GRIDSverifyResponse(m_EphemeralKey, Response, m_AuthChallengeRandom, EphemeralPublicKey, ServerEphemeralKeyPem)) {
      m_Authenticated = true;
      COL_TRC("Client authenticated successfully with ephemeral keys");
      const COLstring SharedSecret = TUNdiffyHellmanSecret(m_EphemeralKey, EphemeralPublicKey);
      m_SessionKey                 = GRIDSderiveKey(SharedSecret, COLstring(), COLstring("GRIDS-AUTH-v1"));
      COL_TRC("Derived session key, size=" << m_SessionKey.size());
      const COLstring SuccessFrame = GRIDSbuildSuccessFrame(m_SessionKey);
      pSendFrame->run(SuccessFrame.c_str(), SuccessFrame.size());
      COL_TRC("Sent AUTH_SUCCESS to client");
      if(m_pOnAuthenticated.get()) { m_pOnAuthenticated->run(); }
   } else {
      COL_TRC("Client authentication failed, signature or challenge verification failed");
   }
}

void GRIDSwebSocketAuth::processSuccess(const COLstring& Frame) {
   COL_METHOD(GRIDSwebSocketAuth::processSuccess);
   if(m_Mode != GRIDWS_MODE_CLIENT) {
      COL_TRC("Received AUTH_SUCCESS but not in client mode");
      return;
   }

   if(!GRIDSverifySuccessFrame(Frame, m_SessionKey)) {
      COL_TRC("AUTH_SUCCESS verification failed");
      return;
   }

   m_Authenticated = true;
   COL_TRC("Authentication successful, peer fingerprint: " << m_PeerFingerprint);
   if(m_pOnAuthenticated.get()) { m_pOnAuthenticated->run(); }
}
