//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HTTPcookie
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, December 8th, 2006 @ 04:26:33 PM
//
//---------------------------------------------------------------------------
#include "HTTPprecomp.h"
#pragma hdrstop

#include "HTTPcookie.h"
#include "HTTPresponse.h"
#include "HTTPrequest.h"
#include "HTTPheader.h"

#include <COL/COLdateTime.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void HTTPextractCookiesImpl(const HTTPheader& Request, COLhashmap<COLstring, COLstring>& CookieJar, bool IsResponse)
{
   COL_FUNCTION(HTTPextractCookiesImpl);

   COLstring CookieHeaderName;
   if(IsResponse)
   {
      CookieHeaderName = "Set-Cookie";
   }
   else
   {
      CookieHeaderName = "Cookie";
   }

   COLuint32 HeaderIndex = Request.findHeader(CookieHeaderName);
   if (HeaderIndex != -1){
      COLstring CookieHeader(Request.headerValue(HeaderIndex));
      do
      {
         size_t EqualsIndex = CookieHeader.find("=");
         if (EqualsIndex > CookieHeader.size())
         {
            break;
         }

         COLstring CookieName = CookieHeader.substr(0, EqualsIndex);

         size_t EndIndex = CookieHeader.find(";");
         if (EndIndex > CookieHeader.size())
         {
            EndIndex = CookieHeader.size();
         }

         COLstring CookieValue = CookieHeader.substr(EqualsIndex+1, EndIndex - EqualsIndex - 1);
         COL_VAR2(CookieName, CookieValue);

         CookieName.stripAll(' ');
         CookieJar[CookieName] = CookieValue;

         CookieHeader = CookieHeader.substr(EndIndex + 1, CookieHeader.size() - EndIndex - 1);
      } while(!IsResponse);
   }
}

void HTTPextractCookies(const HTTPrequest& FullRequest, COLhashmap<COLstring,COLstring>& CookieJar)
{
   const HTTPheader& Headers = FullRequest.header();
   HTTPextractCookiesImpl(Headers, CookieJar, false);
}

void HTTPextractCookies(const HTTPresponse& FullResponse, COLhashmap<COLstring,COLstring>& CookieJar)
{
   const HTTPheader& Headers = FullResponse.header();
   HTTPextractCookiesImpl(Headers, CookieJar, true);
}

COLstring HTTPmakeCookieString
(
   const COLstring& Name,
   const COLstring& Value,
   const COLdateTime* Expiration,
   const COLstring& Path,
   const COLstring& Domain,
   bool Secure)
{
   COLstring CookieString = Name + "=" + Value;

   if (Expiration != NULL)
   {
      COLstring DayOfWeek;
      Expiration->dayOfWeekString(DayOfWeek);

      COLstring Month;
      Expiration->monthString(Month);

      char ExpirationBuffer[30];

      snprintf(ExpirationBuffer, 30, "%s, %02d-%s-%d %02d:%02d:%02d",
              DayOfWeek.c_str(), Expiration->day(), Month.c_str(), Expiration->year(),
              Expiration->hour(), Expiration->minute(), Expiration->second());
      CookieString = CookieString + "; expires=" + ExpirationBuffer;
   }
   if (Path != "")
   {
      CookieString = CookieString + "; path=" + Path;
   }
   else
   {
      CookieString = CookieString + "; path=/";
   }

   if (Domain != "")
   {
      CookieString = CookieString + "; domain=" + Domain;
   }

   CookieString = CookieString + "; HttpOnly"; // TODO: Need to check if this is valid in Iguana

   if(Secure){
      CookieString = CookieString + "; Secure";
   }

   COL_VAR2(Secure, CookieString);
   return CookieString;
}

void HTTPsetCookie
(
   HTTPresponse& Response,
   const COLstring& Name,
   const COLstring& Value,
   const COLdateTime* Expiration,
   const COLstring& Path,
   const COLstring& Domain,
   bool Secure)
{
   Response.header().addHeader("Set-Cookie", HTTPmakeCookieString(Name, Value, Expiration, Path, Domain, Secure));
}

void HTTPsetCookie
(
   HTTPrequest& Request,
   const COLstring& Name,
   const COLstring& Value,
   const COLdateTime* Expiration,
   const COLstring& Path,
   const COLstring& Domain,
   bool Secure)
{
   Request.header().addHeader("Cookie", HTTPmakeCookieString(Name, Value, Expiration, Path, Domain, Secure));
}

void HTTPsetCookies
(
   HTTPrequest& Request,
   const COLhashmap<COLstring, COLstring>& CookieJar
)
{
   COLstring Cookies;
   COLostream Stream(Cookies);

   COLhashmapPlace Place;
   for(Place = CookieJar.first(); Place != NULL; Place = CookieJar.next(Place))
   {
      if(Place != CookieJar.first())
      {
         Stream << COL_T("; ");
      }
      Stream << CookieJar.key(Place) << '=' << CookieJar[Place];
   }

   if(Cookies.size())
   {
      Request.header().addHeader("Cookie", Cookies);
   }
}
