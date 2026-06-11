// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcontent
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 24 February 2023 - 10:58AM
// ---------------------------------------------------------------------------
#include <SDB/SDBcontent.h>

#include <SDB/SDBmessage.h>
#include <SDB/SDBmagicMarker.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBposition.h>
#include <SDB/SDBpositionMap.h>
#include <SDB/SDBpositionMap1.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

typedef SDBcontent* (*SDBcreateContentF)();
typedef void        (*SDBdeleteContentF)(      SDBcontent* pContent);
typedef int         (*SDBcontentSizeF)  (const SDBcontent* pContent);
typedef int         (*SDBreadContentF)  (      SDBcontent* pContent, const char* pBuffer, int BufferSize);
typedef int         (*SDBwriteContentF) (const SDBcontent* pContent,       char* pBuffer, int BufferSize);
typedef COLostream& (*SDBprintContentF) (const SDBcontent* pContent, COLostream& Stream);

struct SDBcontentEntry{
   SDBcontentSizeF   pSizeFunc;
   SDBreadContentF   pReadFunc;
   SDBwriteContentF  pWriteFunc;
   SDBcreateContentF pCreateFunc;
   SDBdeleteContentF pDeleteFunc;
   SDBprintContentF  pPrintFunc;
};

SDBcontentEntry s_ContentMap[SDB_EOT] {0};

// TODO: let SDBmessage and SDBmagicMarker do the registration?
//          they need to include SDBcontent for type enum
//          they don't need to expose the SDBcontent version of size, read, and write function
void SDBcontentMapInit(){
    COL_FUNCTION(SDBcontentMapInit);
    SDBcontentEntry PositionMapEntry = {
        &SDBpositionMapSize,
        &SDBreadPositionMap,
        &SDBwritePositionMap,
        &SDBcreatePositionMap,
        &SDBdeletePositionMap,
        &SDBprintPositionMap
    };
    SDBcontentEntry PositionEntry = {
        &SDBpositionSize,
        &SDBreadPosition,
        &SDBwritePosition,
        &SDBcreatePosition,
        &SDBdeletePosition,
        &SDBprintPosition
    };
    SDBcontentEntry MarkerEntry = {
        &SDBmagicMarkerSize,
        &SDBreadMagicMarker,
        &SDBwriteMagicMarker,
        &SDBmagicMarkerCreate,
        &SDBmagicMarkerDelete,
        &SDBprintMagicMarker
    };
    SDBcontentEntry MessageEntry = {
        &SDBmessageSize,
        &SDBreadMessage,
        &SDBwriteMessage,
        &SDBmessageCreate,
        &SDBmessageDelete,
        &SDBprintMessage
    };
    SDBcontentEntry PositionMapEntry1 = {
        &SDBpositionMapSize1,
        &SDBreadPositionMap1,
        &SDBwritePositionMap1,
        &SDBcreatePositionMap1,
        &SDBdeletePositionMap1,
        &SDBprintPositionMap1
    };
    s_ContentMap[SDB_MARKER ]  = MarkerEntry;
    s_ContentMap[SDB_POSMAP ]  = PositionMapEntry;
    s_ContentMap[SDB_POSITION] = PositionEntry;
    s_ContentMap[SDB_MESSAGE]  = MessageEntry;
    s_ContentMap[SDB_TRACE ]   = MessageEntry;
    s_ContentMap[SDB_INFO   ]  = MessageEntry;
    s_ContentMap[SDB_DEBUG  ]  = MessageEntry;
    s_ContentMap[SDB_WARNING]  = MessageEntry;
    s_ContentMap[SDB_ERROR  ]  = MessageEntry;
    s_ContentMap[SDB_SUCCESS]  = MessageEntry;
    s_ContentMap[SDB_POSMAP1]  = PositionMapEntry1;
}


int SDBcontentSize(const SDBcontent* pContent){
    COL_FUNCTION(SDBcontentSize);
    COL_VAR(pContent->Type);
    SDBcontentEntry& ContentEntry = s_ContentMap[pContent->Type];
    COL_VAR((void*)ContentEntry.pSizeFunc);     // did you init SDB with SDBcontentMapInit()?
    return (*ContentEntry.pSizeFunc)(pContent);
}

int SDBreadContent(SDBcontent* pContent, const char* pBuffer, int BufferSize) {
    COL_FUNCTION(SDBreadContent);
    SDBcontentEntry& ContentEntry = s_ContentMap[pContent->Type];
    return (*ContentEntry.pReadFunc)(pContent, pBuffer, BufferSize);
}

int SDBwriteContent(const SDBcontent* pContent, char* pBuffer, int BufferSize) {
    COL_FUNCTION(SDBwriteContent);
    SDBcontentEntry& ContentEntry = s_ContentMap[pContent->Type];
    return (*ContentEntry.pWriteFunc)(pContent, pBuffer, BufferSize);
}

SDBcontent* SDBcontentCreate(COLuint64 ContentType){
   COL_FUNCTION(SDBcontentCreate);
   if (ContentType == SDB_NULL || ContentType > SDB_EOT) {
      COL_WRN("Invalid content type: " << SDBtypeAsString((SDBtype)ContentType));
      return NULL;
   }
   SDBcreateContentF pCreate = s_ContentMap[ContentType].pCreateFunc;
   if (NULL == pCreate) {
      COL_ERR("Content map not initialized with SDBcontentMapInit()");
      return NULL;
   }
   return (*pCreate)();
}

void SDBcontentDestroy(SDBcontent* pContent){
   COL_FUNCTION(SDBcontentDestroy);
   (*s_ContentMap[pContent->Type].pDeleteFunc)(pContent);
}

COLostream& operator<<(COLostream& Stream, const SDBcontent& Obj) {
    return (*s_ContentMap[Obj.Type].pPrintFunc)(&Obj, Stream);
}

COLuint64 SDBcontentType(const char* pBuffer, int BufferSize) {
   COL_FUNCTION(SDBcontentType);
   COLuint64 Type = 0;
   SDBreadNumber(&Type, pBuffer, BufferSize);
   return Type;
}

const char* SDBtypeAsString(SDBtype Type){
   COL_FUNCTION(SDBtypeAsString);
   switch(Type){
      case SDB_NULL:     return "";     // not set
      case SDB_MARKER:   return "";
      case SDB_POSMAP:   return "";
      case SDB_POSITION: return "";
      case SDB_MESSAGE:  return "message";
      case SDB_TRACE:    return "";
      case SDB_INFO:     return "";
      case SDB_DEBUG:    return "";
      case SDB_WARNING:  return "";
      case SDB_ERROR:    return "error";
      case SDB_SUCCESS:  return "";
   default:
      return "";
   }
}

SDBtype SDBtypeFromString(const COLstring& Type){
   COL_FUNCTION(SDBtypeFromString);
   if ("null"     == Type) { return SDB_NULL;     }
   if ("marker"   == Type) { return SDB_MARKER;   }
   if ("posmap"   == Type) { return SDB_POSMAP;   }
   if ("position" == Type) { return SDB_POSITION; }
   if ("message"  == Type) { return SDB_MESSAGE;  }
   if ("trace"    == Type) { return SDB_TRACE;    }
   if ("info"     == Type) { return SDB_INFO;     }
   if ("debug"    == Type) { return SDB_DEBUG;    }
   if ("warning"  == Type) { return SDB_WARNING;  }
   if ("error"    == Type) { return SDB_ERROR;    }
   if ("success"  == Type) { return SDB_SUCCESS;  }
   return SDB_EOT;
}
