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
!include nsDialogs.nsh
!include LogicLib.nsh
!include "StrRep.nsh"

Var UserInput
Var PasswordInput
Var ButtonSystem
Var ButtonSystemState
Var ButtonUser
Var ButtonUserState 
Var SystemInstallType
Var Step2

Function RenderUserName
   !insertmacro MUI_HEADER_TEXT "Who should this application be installed for?" "iNTERFACEWARE IguanaX service will run as specified user."
   nsDialogs::Create 1018
   ${NSD_CreateLabel} 0 0 100% 14u "Which account should the IguanaX Service run under?"
   Pop $R0 ; Don't care

   ; Add radio buttons
   ${NSD_CreateRadioButton} 0 15u 50% 14u "Install under Local System Account"
   Pop $ButtonSystem
   ${NSD_OnClick} $ButtonSystem onInstallTypeChange
   ${NSD_CreateRadioButton} 0 30u 50% 14u "Install as User"
   Pop $ButtonUser
   ${NSD_OnClick} $ButtonUser onInstallTypeChange

   ${If} $SystemInstallType == "User"
      ${NSD_Check} $ButtonUser
      ${NSD_GetState} $ButtonSystem $ButtonSystemState
      ${NSD_GetState} $ButtonUser $ButtonUserState
   ${Else}
      ${NSD_Check} $ButtonSystem
      StrCpy $SystemInstallType "System"
      ${NSD_GetState} $ButtonSystem $ButtonSystemState
      ${NSD_GetState} $ButtonUser $ButtonUserState
   ${EndIf}

   ; Add User Credentials field
   ${NSD_CreateGroupBox} 0 48u 100% 80u "Windows User Credentials:"
   Pop $R0  ; Don't care
   ${NSD_CreateLabel} 13u 63u 100u 12u "User:"
   Pop $R0
   System::Call "advapi32::GetUserName(t .r0, *i ${NSIS_MAX_STRLEN} r1) i.r2"
   ${NSD_CreateText} 13u 76u 250u 12u "$0"
   Pop $UserInput
	${NSD_CreateLabel} 13u 93u 100u 12u "Password:"
   Pop $R0
	${NSD_CreatePassword} 13u 106u 250u 12u ""
   Pop $PasswordInput
   nsDialogs::Show
FunctionEnd

Function onInstallTypeChange
   ${NSD_GetState} $ButtonSystem $ButtonSystemState
   ${NSD_GetState} $ButtonUser $ButtonUserState

   ${If} $ButtonSystemState == ${BST_CHECKED}
      StrCpy $SystemInstallType "System"
   ${Else}
      StrCpy $SystemInstallType "User"
   ${EndIf}
FunctionEnd

Function RememberInputs
   ${NSD_GetText} $UserInput $WindowsUserName
   ${NSD_GetText} $PasswordInput $WindowsPassword
   ${StrRep} $WindowsUserName $WindowsUserName '"' '\"'
   ${StrRep} $WindowsPassword $WindowsPassword '"' '\"'
   StrCpy $InstallType $SystemInstallType
   StrCpy $Step2 "Step 2"
FunctionEnd

!macro IFW_PAGE_USER
   Page Custom RenderUserName RememberInputs
!macroend
