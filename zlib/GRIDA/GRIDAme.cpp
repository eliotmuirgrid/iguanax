//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiSelf.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDserialize.h>
#include <GRIDA/GRIDAcomponents.h>
#include <GRIDA/GRIDAme.h>
#include <IWEB/IWEBconfig.h>
#include <SCK/SCKsystem.h>
#include <TUN/TUNkeyFingerPrint.h>
COL_LOG_MODULE;

#ifdef _WIN32
#include <iphlpapi.h>
#else
#include <ifaddrs.h>
#endif

static COLstring GRIDAgetLocalIpAddress() {
   COL_FUNCTION(GRIDAgetLocalIpAddress);
   COLstring IpAddress = "127.0.0.1";
#ifdef _WIN32
   ULONG BufferSize = 0;
   DWORD Result     = GetAdaptersInfo(NULL, &BufferSize);
   if(Result == ERROR_BUFFER_OVERFLOW) {
      COLstring Buffer;
      Buffer.setCapacity(BufferSize);
      PIP_ADAPTER_INFO AdapterInfo = (PIP_ADAPTER_INFO)Buffer.get_buffer();
      if(GetAdaptersInfo(AdapterInfo, &BufferSize) == ERROR_SUCCESS) {
         PIP_ADAPTER_INFO Adapter = AdapterInfo;
         while(Adapter) {
            if(Adapter->Type != MIB_IF_TYPE_LOOPBACK) {
               PIP_ADDR_STRING IpAddr = &Adapter->IpAddressList;
               while(IpAddr) {
                  COLstring Ip = IpAddr->IpAddress.String;
                  if(!Ip.is_null() && Ip != "0.0.0.0" && Ip.find("127.", 0) != 0) {
                     IpAddress = Ip;
                     COL_TRC("Found IP: " << IpAddress);
                     return IpAddress;
                  }
                  IpAddr = IpAddr->Next;
               }
            }
            Adapter = Adapter->Next;
         }
      }
   }
#else
   ifaddrs* IfAddrs = NULL;
   if(getifaddrs(&IfAddrs) == 0) {
      const ifaddrs* Ifa = IfAddrs;
      while(Ifa) {
         if(Ifa->ifa_addr && Ifa->ifa_addr->sa_family == AF_INET) {
            const auto pSin = (struct sockaddr_in*)Ifa->ifa_addr;
            char       IpStr[INET_ADDRSTRLEN];
            if(inet_ntop(AF_INET, &pSin->sin_addr, IpStr, sizeof(IpStr))) {
               COLstring Ip = IpStr;
               if(!Ip.is_null() && Ip != "0.0.0.0" && Ip.find("127.", 0) != 0) {
                  IpAddress = Ip;
                  COL_TRC("Found IP: " << IpAddress);
                  freeifaddrs(IfAddrs);
                  return IpAddress;
               }
            }
         }
         Ifa = Ifa->ifa_next;
      }
      freeifaddrs(IfAddrs);
   }
#endif
   COL_TRC("Using fallback IP: " << IpAddress);
   return IpAddress;
}

static COLvar GRIDapiNodeData(GRIDnode* pNode, CFGmap* pComponentMap) {
   COL_FUNCTION(GRIDapiNodeData);
   COLvar      Self;
   const auto& Info    = pNode->getNodeInfo();
   Self["id"]          = Info.m_IguanaId;
   Self["name"]        = Info.m_pWebConfig->serverDescription();
   Self["host"]        = GRIDAgetLocalIpAddress();
   Self["port"]        = Info.m_pWebConfig->webPort();
   Self["fingerprint"] = TUNkeyFingerPrint(*Info.m_pPrivateKey);
   Self["https"]       = Info.m_pWebConfig->useHttps();
   GRIDAcomponentsImpl(pComponentMap, &Self);
   return Self;
}

// /iguanas/me
void GRIDAme(const COLwebRequest& Request, GRIDnode* pNode, CFGmap* pComponentMap) {
   COL_FUNCTION(GRIDAme);
   const COLvar Response = GRIDapiNodeData(pNode, pComponentMap);
   COL_VAR(Response.json(1));
   COLrespondSuccess(Request.Address, Response);
}
