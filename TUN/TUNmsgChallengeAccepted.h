#ifndef __TUN_MSG_CHALLENGE_ACCEPTED_H__
#define __TUN_MSG_CHALLENGE_ACCEPTED_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgChallengeAccepted
//
// Description:
//
// Authenticated
//
// Author: Eliot Muir 
// Date:   Thursday 15 June 2023 - 12:03PM
// ---------------------------------------------------------------------------

class TUNrequestItem;
class TUNstateClient;

void TUNmsgChallengeAccepted(TUNrequestItem* pRequest, TUNstateClient* pState);

#endif // end of defensive include
