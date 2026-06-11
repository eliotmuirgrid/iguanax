#ifndef __SDM_MESSAGES_H__
#define __SDM_MESSAGES_H__

//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// SDM Event Log Messages Definitions
//
// This file defines the format of the messages that the SDMserver sends
// to the Windows Event Viewer. Currently, we only support one message.
// The message content is "%1". The %1 behaves like a printf format string,
// i.e. it can be replaced by a string sent to the _WIN32 ReportEvent function.
// Each message defined in this file can have different formats and even
// multiple escape sequences (%1, %2, etc.). However, we don't really need that
// functionality, we just want one free form message. This way the entire
// content of the message is determined at runtime.
//
// Author: Filip Petr
// Date:   Mon 26/07/2004
// Revision: $Revision: 1.1 $
//
// Last Edit Date: $Date: 2004-08-05 20:54:38 $
// Source: $Source: /home/cvs/cvsroot/SDM/SDMmessages.mc,v $
//---------------------------------------------------------------------------

//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: SDM_EVENT_MESSAGE
//
// MessageText:
//
// %1
//
#define SDM_EVENT_MESSAGE                0x00000064L

#endif // end of defensive include
