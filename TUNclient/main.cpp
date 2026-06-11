//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNclient
//
// Description:
//
// TUNclient - Tunnel Client
//
// Author: Eliot Muir
// Date:   Fri 16 Jun 2023 13:27:03 EDT
//---------------------------------------------------------------------------

#include <SCK/SCKutils.h>

#include <CMD/CMDlineParser.h>

#include <stdlib.h>

#include <TUN/TUNclientNetwork.h>

#include <TUN/TUNstateClient.h>
#include <TUN/TUNclientLoad.h>
#include <TUN/TUNkeyFingerPrint.h>
#include <TUN/TUNsendPeerData.h>
#include <TUN/TUNkeyConvert.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure.h>
#include <COL/COLthreadPool.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNCparseCommand(TUNstateClient* pState, const COLstring& Line){
   COL_FUNCTION(TUNCparseCommand);
   COLstring Label, Data;
   if (!Line.split(Label, Data, " ")){
      COLcout << "Could not parse input." << newline;
      return;
   }
   if (pState->m_Peers.count(Label) == 0){
      COLcout << "No such peer." << newline;
      return;
   }
   if (!pState->m_Peers[Label].m_Authenticated){
      COLcout << "Sorry " << Label << " isn't online now." << newline;
      return;
   }
   TUNsendPeerData(pState, Data, Label);
}

void TUNCstandardInRead(COLstring* pBuffer, TUNstateClient* pState){
   COL_FUNCTION(TUNstandardInRead);
   int AmountRequired = pBuffer->size() + 256;
   if (AmountRequired >= pBuffer->capacity()){
      pBuffer->setCapacity(AmountRequired);
   }
   int AmountRead = read(STDIN_FILENO, pBuffer->get_buffer()+pBuffer->size(), pBuffer->capacity()-pBuffer->size());
   COL_VAR2(AmountRead, AmountRequired);
   pBuffer->setSize(pBuffer->size() + AmountRead);
   if (pBuffer->find("\n") != npos){
      TUNCparseCommand(pState, *pBuffer);
      pBuffer->clear();
   }
}

void TUNConPeerData(const COLstring& PeerLabel, const COLstring& Data, TUNstateClient* pState){
   COL_FUNCTION(TUNConPeerData);
   COLcout << PeerLabel << " said " << Data;
}

void TUNConPeerConnect(const COLstring& PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TUNConPeerConnect);
   COLcout << PeerLabel << " is connected." << newline;
}

void TUNConPeerDisconnect(const COLstring& PeerLabel, TUNstateClient* pState){
   COL_FUNCTION(TUNConPeerConnect);
   COLcout << PeerLabel << " disconnected." << newline;
}

void TUNConRouterConnect(){
   COL_FUNCTION(TUNConRouterConnect);
   COLcout << "Connected to router." << newline; 
}

void TUNConRouterDisconnect(){
   COL_FUNCTION(TUNConRouterDisconnect);
   COLcout << "No connection to router.  Trying to reconnect in 5 seconds." << newline;
}

void TUNCclient(const COLstring& ConfigFile, const COLstring& AMessage){
   COL_FUNCTION(TUNCclient);
   COLthreadPool Pool;
   SCKloop Loop(&Pool);
   TUNstateClient State;
   State.m_pOnData       = COLnewClosure2(&TUNConPeerData,       &State);
   State.m_pOnConnect    = COLnewClosure1(&TUNConPeerConnect,    &State);
   State.m_pOnDisconnect = COLnewClosure1(&TUNConPeerDisconnect, &State);
   State.m_pOnRouterConnect    = COLnewClosure0(&TUNConRouterConnect);
   State.m_pOnRouterDisconnect = COLnewClosure0(&TUNConRouterDisconnect);
   
   TUNclientLoad(ConfigFile, &State);
   State.m_pLoop = &Loop;
   //Peer.m_BufferOut = AMessage;

   COLcout << "Client " << State.m_Name << " (" << ConfigFile << ")" << newline;
   COLcout << TUNkeyPublicAsPemString(State.m_MyKey) << indent;
   for (auto i=State.m_Peers.begin(); i != State.m_Peers.end(); i++){
      COLcout << newline << i->first << " - " << TUNkeyFingerPrint(i->second.m_PublicKey);
   }
   COLcout << unindent << newline;
   TUNclientNetworkInit(&State);
   COLstring Buffer;
   Loop.registerCallbacks(STDIN_FILENO, COLnewClosure0(&TUNCstandardInRead, &Buffer, &State),
                                        COLnewClosure0(&TUNCstandardInRead, &Buffer, &State), 
                                        COLnewClosure0(&TUNCstandardInRead, &Buffer, &State));
   Loop.addReadCallback(STDIN_FILENO);
   
   Pool.start();
   Loop.start();
   Loop.removeAllCallback(STDIN_FILENO);
}

int main(int argc, const char* argv[]){
   SCKinitWinsock();
   try{
      CMDlineParser Parser;
      Parser.setDescription("iNTERFACEWARE Tunnel Client 1.0");
      Parser.addFlagWithArgument("config", "config file");
      Parser.addFlagWithArgument("message", "data");
      
      Parser.parseArgs(argc, argv);
      if (Parser.parsingErrorsPresent(COLcout)){
         Parser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      COLstring ConfigFile = "client/client1.config";
      if (Parser.isFlagPresent("config")){
         ConfigFile = Parser.flagArgument("config");   
      }  
      COLstring Data; 
      if (Parser.isFlagPresent("message")){
         Data = Parser.flagArgument("message");
      }

      TUNCclient(ConfigFile, Data);

   } catch (const COLerror& Error){
      COLcout << Error;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}
