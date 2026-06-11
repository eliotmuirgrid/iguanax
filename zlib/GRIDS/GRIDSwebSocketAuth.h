#ifndef _GRIDWEBSOCKETAUTH_H
#define _GRIDWEBSOCKETAUTH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDwebSocketAuth.h
//
//  Description: Authentication orchestration for WebSocket connections
//               Manages auth state machine and coordinates challenge/response flow
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <GRID/GRIDwebSocket.h>
#include <TUN/TUNkey.h>

struct GRIDnodeInfo;

class GRIDSwebSocketAuth {
  public:
   GRIDSwebSocketAuth(GRIDwebSocketMode Mode, const GRIDnodeInfo& Config, const COLstring& ExpectedFingerprint);
   ~GRIDSwebSocketAuth();

   void setOnAuthenticated(COLauto<COLclosure0> pCallback) { m_pOnAuthenticated = pCallback; }
   void initiateServerChallenge(COLauto<COLclosure2<const void*, int>> pSendFrame);
   void processChallenge(const COLstring& Frame, COLauto<COLclosure2<const void*, int>> pSendFrame);
   void processResponse(const COLstring& Frame, COLauto<COLclosure2<const void*, int>> pSendFrame);
   void processSuccess(const COLstring& Frame);

   bool isAuthenticated() const { return m_Authenticated; }

  private:
   GRIDwebSocketMode    m_Mode;
   const GRIDnodeInfo&  m_NodeInfo;
   bool                 m_Authenticated;
   TUNkey               m_EphemeralKey;
   COLstring            m_AuthChallengeRandom;
   COLstring            m_PeerFingerprint;
   COLstring            m_ExpectedFingerprint;
   COLstring            m_SessionKey;
   COLauto<COLclosure0> m_pOnAuthenticated;

   GRIDSwebSocketAuth(const GRIDSwebSocketAuth&)            = delete;
   GRIDSwebSocketAuth& operator=(const GRIDSwebSocketAuth&) = delete;
};

#endif
