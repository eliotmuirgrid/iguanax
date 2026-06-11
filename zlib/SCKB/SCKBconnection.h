#ifndef __SCKB_CONNECTION_H__
#define __SCKB_CONNECTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKBconnection
//
// Description:
//
// Simple blocking socket connection with timeout.
//
// Author: John Q
// Date:   Tue  1 Nov 2022 21:00:25 EDT
//---------------------------------------------------------------------------

class COLstring;

class SCKBtimeoutSetter {
public:
   SCKBtimeoutSetter(int Socket, int TimeoutMs, int Direction);
   ~SCKBtimeoutSetter();
private:
   int m_Socket;
   int m_Timeout;
   int m_Direction;
   int m_OldTimeout;
};

class SCKBconnection {
public:
   SCKBconnection();
   virtual ~SCKBconnection();
   SCKBconnection(const SCKBconnection& Orig) = delete;
   SCKBconnection& operator=(const SCKBconnection& Orig) = delete;

   virtual bool connect(const COLstring& Host, int Port);
   virtual int  read (COLstring* pBuffer);
   virtual int  write(const COLstring& Buffer);
   virtual void close();

   void setTimeout(int DurationInMilliseconds) { m_Timeout = DurationInMilliseconds; }  
   int  timeout()   const        { return m_Timeout; }
   int  lastError() const        { return m_LastError; }

protected:
   int  m_Handle    { -1 };
   int  m_LastError { 0 };
   int  m_Timeout   { 0 };
};

#endif // end of defensive include