// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNideFileLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 25 April 2023 - 09:30AM
// ---------------------------------------------------------------------------

#include <TRN/TRNideFileLoad.h>
#include <TRN/TRNdecodeSampleDataFile.h>

#include <FIL/FILpathUtils.h>

#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static bool is_probably_text(const unsigned char *buf, size_t len) {
   if (len == 0) return true;

   size_t printable_bytes = 0;
   for (size_t i = 0; i < len; i++) {
      unsigned char c = buf[i];

      if (c == 0) return false; // Binary null check

      // Standard ASCII + Common Whitespace
      if ((c >= 0x20 && c < 0x7F) ||
         (c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\b')) {
         printable_bytes++;
      }
      // UTF-8 Multibyte
      else if (c >= 0xC2 && c <= 0xF4) {
         int extra = (c < 0xE0) ? 1 : (c < 0xF0) ? 2 : 3;
         if (i + extra < len) {
               bool valid = true;
               for (int j = 1; j <= extra; j++) {
                  if ((buf[i + j] & 0xC0) != 0x80) { valid = false; break; }
               }
               if (valid) {
                  printable_bytes += (1 + extra); // Count all bytes in the sequence
                  i += extra;
               }
         }
      }
   }

   double ratio = (double)printable_bytes / len;
   return ratio > 0.90; // Slightly more relaxed threshold
}

bool TRNisBinaryContent(const COLstring& Content) {
   const unsigned char* Buffer = (const unsigned char *) Content.data();
   // Known binary magic headers
   if (Content.size() >= 4) {
      if ( Buffer[0] == 0x7F && Buffer[1] == 'E'  && Buffer[2] == 'L'  &&  Buffer[3] == 'F')                         return true; // ELF
      if ((Buffer[0] == 0xFE && Buffer[1] == 0xED && Buffer[2] == 0xFA && (Buffer[3] == 0xCE || Buffer[3] == 0xCF))
       || (Buffer[0] == 0xCA && Buffer[1] == 0xFE && Buffer[2] == 0xBA &&  Buffer[3] == 0xBE))                       return true; // Mach-O / fat
      if ( Buffer[0] == 0x60 && Buffer[1] == 0x34)                                                                   return true; // Binary VMD
      if ( Buffer[0] == 0x50 && Buffer[1] == 0x4B && Buffer[2] == 0x03 && Buffer[3] == 0x04)                         return true; // ZIP (also .docx, .xlsx, .jar)
      if ( Buffer[0] == 0x1F && Buffer[1] == 0x8B)                                                                   return true; // GZIP
      if ( Buffer[0] == 0xFF && Buffer[1] == 0xD8 && Buffer[2] == 0xFF)                                              return true; // JPEG
      if ( Buffer[0] == 0x89 && Buffer[1] == 'P'  && Buffer[2] == 'N'  && Buffer[3] == 'G')                          return true; // PNG
   }
   // Note: only check up to the first 4096 bytes in case Content is huge
   return !is_probably_text(Buffer, COL_MIN(Content.size(), 4096));
}

void TRNideFileLoadImp(const COLstring& ComponentId, bool Development, const COLstring& File, const COLaddress& Address, COLvar& Result){
   COL_FUNCTION(TRNideFileLoadImp);
   COLstring Path = CFGresolveRepoFile(ComponentId, File, Development);
   if (Path.is_null()){
      COLrespondError(Address, "Cannot open files above the root.");
      return;
   }
   COLstring Content;
   COL_VAR4(Path, ComponentId, Development, File);
   Result["FileValid"] = TRNloadTranslatorFile(Path, &Content);
   Result["Code"]      = Content;
   if(TRNisBinaryContent(Content)) { return COLrespondError(Address, "Binary files cannot be shown"); }
   COLrespondSuccess(Address, Result);
}

// /file/load
void TRNideFileLoad(const COLwebRequest& Request){
   COL_FUNCTION(TRNideFileLoad);
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   bool      Development = COLvarRequiredBool  (Data, "development");
   COLstring File        = COLvarRequiredString(Data, "file");
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   ROLid RoleId = Development ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, RoleId, &Error)){
      return COLrespondError(Request.Address, PRMsetError(RoleId));
   }
   COLvar Result;
   COL_VAR3(Development, File, ComponentId);
   TRNideFileLoadImp(ComponentId, Development, File, Request.Address, Result);
}
