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

!define IFW_GetProductUninstallInfo "!insertmacro getProductUninstallInfo"

!macro getProductUninstallInfo Product UninstallerPath InstallLocation
   Push "${Product}"
   Call getProductUninstallInfo
   Pop  ${InstallLocation}
   Pop  ${UninstallerPath}
!macroend

Function getProductUninstallInfo
   Pop $0   ; Product

   ; NSIS is a 32-bit application, so HKEY_LOCAL_MACHINE\Software is actually
   ; HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node in 64-bit Windows
   ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\$0" "UninstallString"
   ReadRegStr $R1 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\$0" "InstallLocation"

   Push $R0 ; UninstallerPath
   Push $R1 ; InstallLocation
FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sets product uninstall information
;
; Usage:
;    ${IFW_SetProductUninstallInfo} Product InstallLocation IconPath
;
; Parameters:
;    + Product             Product name (e.g. iNTERFACEWARE Chameleon)
;    + InstallPath         Product install location
;    + IconPath            Product icon path
;    + Version             Product version string
;
; Example:
;    ${IFW_SetProductUninstallInfo} "iNTERFACEWARE Iguana" "$INSTDIR" "$INSTDIR\favicon.ico" "6_2_1"

!define IFW_SetProductUninstallInfo "!insertmacro setProductUninstallInfo"

!macro setProductUninstallInfo Product InstallPath IconPath Version
   Push "${Version}"
   Push "${IconPath}"
   Push "${InstallPath}"
   Push "${Product}"
   Call setProductUninstallInfo
!macroend

Function setProductUninstallInfo
   Pop $0   ; Product
   Pop $1   ; InstallLocation
   Pop $2   ; IconPath
   Pop $3   ; Version

   ; GetSize DetailPrints to text for some reason
   ;${GetSize} "$INSTDIR" "/S=0K" $R1 $R2 $R2

   ; NSIS is a 32-bit application, so HKEY_LOCAL_MACHINE\Software is actually
   ; HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node in 64-bit Windows
   StrCpy $R3 "Software\Microsoft\Windows\CurrentVersion\Uninstall\$0"
   WriteRegStr   HKLM $R3 "DisplayIcon"      "$2"
   WriteRegStr   HKLM $R3 "DisplayName"      "$0"
   WriteRegStr   HKLM $R3 "DisplayVersion"   "$3"
   WriteRegStr   HKLM $R3 "InstallLocation"  "$1"
   WriteRegStr   HKLM $R3 "UninstallString"  "$1\uninstall.exe"
   WriteRegStr   HKLM $R3 "Publisher"        "iNTERFACEWARE Inc."
   WriteRegStr   HKLM $R3 "HelpLink"         "https://help.interfaceware.com"
   WriteRegStr   HKLM $R3 "URLInfoAbout"     "https://www.interfaceware.com/iguana.html"
   WriteRegStr   HKLM $R3 "URLUpdateInfo"    "https://www.interfaceware.com/downloads.html"
   ;WriteRegDWORD HKLM $R3 "EstimatedSize"    $R1
   WriteRegDWORD HKLM $R3 "NoRepair"         1
   WriteRegDWORD HKLM $R3 "NoModify"         1
FunctionEnd
