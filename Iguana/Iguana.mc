;//---------------------------------------------------------------------------
;// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
;//
;// Description:
;//
;// SDM Event Log Messages Definitions
;//
;// This file defines the format of the messages that the SDMserver sends
;// to the Windows Event Viewer. Currently, we only support one message.
;// The message content is "%1". The %1 behaves like a printf format string,
;// i.e. it can be replaced by a string sent to the _WIN32 ReportEvent function.
;// Each message defined in this file can have different formats and even
;// multiple escape sequences (%1, %2, etc.). However, we don't really need that
;// functionality, we just want one free form message. This way the entire
;// content of the message is determined at runtime.
;//
;// Author: Filip Petr
;// Date:   Mon 26/07/2004
;//---------------------------------------------------------------------------

MessageId = 100
Facility = Application
SymbolicName = SDM_EVENT_MESSAGE
Language = English
%1 

