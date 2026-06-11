#ifndef __TUN_MSG_CHALLENGE_QUESTION_H__
#define __TUN_MSG_CHALLENGE_QUESTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNmsgChallengeQuestion
//
// Description:
//
// Handles the challenge response from the router.
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 04:46PM
// ---------------------------------------------------------------------------

class TUNrequestItem;
class TUNstateClient;

void TUNmsgChallengeQuestion(TUNrequestItem* pRequest, TUNstateClient* pState);

#endif // end of defensive include
