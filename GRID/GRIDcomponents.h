#ifndef _GRIDCOMPONENTS_H
#define _GRIDCOMPONENTS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc. All Rights Reserved
//
//  Module: GRIDpeerComponents.h
//
//  Description: Polls peer component metrics using GRIDrequestAll
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLvar;
class GRIDnode;
class SCKloop;

class GRIDcomponents {
  public:
   GRIDcomponents(GRIDnode* pNode, SCKloop* pLoop);
   ~GRIDcomponents();

   void startPolling(int IntervalMs = 5000);
   void stopPolling();

  private:
   GRIDnode* m_pNode;
   SCKloop*  m_pLoop;
   int       m_TimerId;
   int       m_IntervalMs;

   void startOnMain();
   void pollMetrics();
   void handleResponse(const COLvar& Results) const;

   GRIDcomponents(const GRIDcomponents&) = delete;
   GRIDcomponents& operator=(const GRIDcomponents&) = delete;
};

#endif  // end of defensive include
