;------------------------------------------------------------------------------
; Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
;
; Module: Nsis Iguana Installer Utility Functions
;
; Author: John Qi
;
; Description:
;    Common functions used to un/installs Chameleon and Iguana.
;------------------------------------------------------------------------------
!include "FileFunc.nsh" ; GetSize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Runs the specified command and quit on failure
;
; Usage:
;    ${IFW_RunOrQuit} Command Timeout Error
;
; Parameters:
;    + Command             Command to run
;    + Timeout             Timeout
;    + Error               Error message
;
; Note:
;     Use ${IFW_RunOrQuitUn} for uninstaller.
;
; Example:
;    ${IFW_RunOrQuit} "iguana --version" 3000 "iguana version"

!define IFW_RunOrQuit "!insertmacro runOrQuit ''"        ; for installer
!define IFW_RunOrQuitUn "!insertmacro runOrQuit 'un.'"   ; for uninstaller

!macro runOrQuit un Command Timeout Error
   Push "${Error}"
   Push "${Timeout}"
   Push "${Command}"
   Call ${un}runOrQuit
!macroend

!macro MYMACRO un    ; logic shared by installer and uninstaller
Function ${un}runOrQuit
   Pop $0   ; Command
   Pop $1   ; Timeout
   Pop $2   ; Error

   nsExec::ExecToLog /TIMEOUT=$1 '$0'
   Pop $R0
   ${If} $R0 != 0
      MessageBox MB_OK|MB_ICONEXCLAMATION "\
         Failed to $2.$\r$\n\
         Error Code: $R0"
      Quit
   ${EndIf}
FunctionEnd
!macroend

!insertmacro MYMACRO ""       ; installer version of the function
!insertmacro MYMACRO "un."    ; uninstaller version of the function

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Extracts product uninstall information from registry
;
; Usage:
;    ${IFW_GetProductUninstallInfo} Product UninstallerPath InstallLocation
;
; Parameters:
;    + Product             Product name (e.g. iNTERFACEWARE Chameleon)
;
; Returns:
;    + UninstallerPath     Path to product uninstaller
;    + InstallLocation     Production install path
;
; Example:
;    ${IFW_GetProductUninstallInfo} "iNTERFACEWARE Iguana" $R0 $R1
