;------------------------------------------------------------------------------
; Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
;
; Module: Nsis Iguana Installer Shortcut Page
;
; Author: John Qi
;
; Description:
;    Render shortcut page for Iguana. User inputs are validated and saved to the
;    following variables:
;       + IguanaDesktopShortcuts
;       + IguanaStartMenuShortcuts
;
; Usage:
;    Var IguanaDesktopShortcuts
;    Var IguanaStartMenuShortcuts
;    !include "PageShortcut.nsh"
;    !insertmacro IFW_PAGE_SHORTCUT
;------------------------------------------------------------------------------

Var IgDeskShortHWND
Var IgStartShortHWND

Function RenderShortcutPage
   ; Set header texxt and sub-text
   !insertmacro MUI_HEADER_TEXT "Desktop and Start Menu Shortcuts" "Select the shortcuts to be created with this installation."

   ; Instantiate a new dialog
   nsDialogs::Create 1018
   Pop $R0
   ${If} $R0 == error
      Abort
   ${EndIf}

   ; Data dir label, input and browse button
   ${NSD_CreateLabel} 0 0 100% 12u "Select the shortcuts to install:"
   Pop $R0  ; Don't care


   ${NSD_CreateGroupBox} 0 13u 100% 52u "Iguana"
   Pop $R0  ; Don't care

   ${NSD_CreateCheckBox} 13u 28u 100u 12u "Desktop shortcut"
   Pop $IgDeskShortHWND
   ${NSD_SetState} $IgDeskShortHWND $IguanaDesktopShortcuts

   ${NSD_CreateCheckBox} 13u 42u 100u 12u "Start menu shortcuts"
   Pop $IgStartShortHWND
   ${NSD_SetState} $IgStartShortHWND $IguanaStartMenuShortcuts

   ; Display dialog
   nsDialogs::Show
FunctionEnd

Function SaveShortcutConfig
   ${NSD_GetState} $IgDeskShortHWND $R0
   ${If} $R0 == ${BST_CHECKED}
      StrCpy $IguanaDesktopShortcuts "1"
   ${Else}
      StrCpy $IguanaDesktopShortcuts "0"
   ${EndIf}

   ${NSD_GetState} $IgStartShortHWND $R0
   ${If} $R0 == ${BST_CHECKED}
      StrCpy $IguanaStartMenuShortcuts "1"
   ${Else}
      StrCpy $IguanaStartMenuShortcuts "0"
   ${EndIf}

   ; MessageBox MB_OK "$IguanaDesktopShortcuts $IguanaStartMenuShortcuts" 
FunctionEnd

Function InitShortcutFlags
   StrCpy $IguanaDesktopShortcuts "1"
   StrCpy $IguanaStartMenuShortcuts "1"
FunctionEnd

!macro IFW_PAGE_SHORTCUT
   Page custom RenderShortcutPage SaveShortcutConfig
!macroend
