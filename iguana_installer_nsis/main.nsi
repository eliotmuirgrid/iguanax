;------------------------------------------------------------------------------
; Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
;
; Module: Nsis Iguana Installer
;
; Author: John Qi
;
; Description:
;    A NSIS wrapper for the command line iguana installer iguana_installer.
;    Adapted from NSIS Modern User Interface example written by Joost Verburg
;
; References:
;    https://nsis.sourceforge.io/Examples/Modern%20UI/WelcomeFinish.nsi
;    nsis_iguana/install-script.nsi
;
; Compilation:
;    The compile command must pass in the version parameter ${IFW_VERSION} and ${IFW_OUTFILE}
;    e.g. makensis.exe /DIFW_VERSION=6_2_1 main.nsi /DIFW_OUTFILE=iguana_setup.exe
;------------------------------------------------------------------------------
; Unicode True

;------------------------------------------------------------------------------
; Variables

Var IguanaDesktopShortcuts
Var IguanaStartMenuShortcuts
Var WindowsUserName
Var WindowsPassword
Var InstallType

Var SkipExitSurvey

;------------------------------------------------------------------------------
; Defines

; Icons
!define INSTALL_ICON "assets\installer_iguana.ico"
!define UNINSTALL_ICON "assets\uninstaller_iguana.ico"

; Header image
!define PAGE_HEADER_IMAGE "assets\IguanaHeader.bmp"
!define WELCOME_SIDE_IMAGE "assets\IguanaSide.bmp"

!define PROGRAMDATA "C:\ProgramData"
!define IFW_BINARY  "$INSTDIR\iguana"

;------------------------------------------------------------------------------
; Includes

; NSIS libraries
!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"

; Iguana specific
!include "Utils.nsh"
!include "PageUninstall.nsh"
!include "PageUser.nsh"
!include "PageInstall.nsh"
!include "PageShortcuts.nsh"

;------------------------------------------------------------------------------
; General

; Name and file
Name "iNTERFACEWARE Iguana X"
OutFile "${IFW_OUTFILE}"

; Request application privileges for Windows Vista or above
RequestExecutionLevel admin

; The icon to use for the installer
Icon "${INSTALL_ICON}"

; The icon to use for the uninstaller
UninstallIcon "${UNINSTALL_ICON}"

;------------------------------------------------------------------------------
; Interface Settings

; Use an image as page headers
!define MUI_HEADERIMAGE

; Installer settings
!define MUI_ICON "${INSTALL_ICON}"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${WELCOME_SIDE_IMAGE}"
!define MUI_HEADERIMAGE_BITMAP "${PAGE_HEADER_IMAGE}"
!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "${IFW_BINARY}"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--open_gui"
!define MUI_FINISHPAGE_RUN_TEXT "Open the iNTERFACEWARE Iguana X Dashboard"

; Uninstaller settings
!define MUI_UNICON "${UNINSTALL_ICON}"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${WELCOME_SIDE_IMAGE}"
!define MUI_HEADERIMAGE_UNBITMAP "${PAGE_HEADER_IMAGE}"
!define MUI_UNABORTWARNING

;------------------------------------------------------------------------------
; Pages

; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "assets\License.txt"
!insertmacro IFW_PAGE_UNINSTALL
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro IFW_PAGE_SHORTCUT
!insertmacro MUI_PAGE_INSTFILES
!insertmacro IFW_PAGE_USER
!insertmacro IFW_INSTALL_PAGE
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;------------------------------------------------------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"

;------------------------------------------------------------------------------
; Installer Sections

Section "-Install Iguana"
   ; set actual install dir
   ${If} $InstallStep S!= "2"
      StrCpy $INSTDIR "$INSTDIR\IguanaX_${IFW_VERSION}"
      DetailPrint "Installing Iguana program files..."
      SetOutPath "$INSTDIR"
      File /r "..\Iguana\app\*"
      DetailPrint "Creating uninstaller..."
      WriteUninstaller "uninstall.exe"
      ${IFW_SetProductUninstallInfo} "iNTERFACEWARE Iguana X" "$INSTDIR" "$INSTDIR\web\favicon.ico" "${IFW_VERSION}"
   ${Else}
      DetailPrint "Starting Iguana service..."
      ${IFW_RunOrQuit} "${IFW_BINARY} --service start" 10000 "start Iguana service"
      DetailPrint "Installing shortcuts..."
      ${If} $IguanaDesktopShortcuts == 1
         SetShellVarContext all
         CreateShortCut "$DESKTOP\Iguana X.lnk" "${IFW_BINARY}" "--open_gui" "$INSTDIR\web\favicon.ico" "" SW_SHOWMINIMIZED
      ${EndIf}
      ${If} $IguanaStartMenuShortcuts == 1
         SetShellVarContext all
         CreateDirectory "$SMPROGRAMS\iNTERFACEWARE"
         CreateDirectory "$SMPROGRAMS\iNTERFACEWARE\Iguana"
         CreateShortCut  "$SMPROGRAMS\iNTERFACEWARE\Iguana\Iguana X.lnk" "${IFW_BINARY}" "--open_gui" "$INSTDIR\web\favicon.ico" "" SW_SHOWMINIMIZED
         CreateShortCut  "$SMPROGRAMS\iNTERFACEWARE\Iguana\Uninstall Iguana X.lnk" "$INSTDIR\uninstall.exe"
      ${EndIf}
      DetailPrint "Completed."
      MessageBox MB_OK "Installation completed successfully!"
   ${EndIf}
SectionEnd


;------------------------------------------------------------------------------
; Uninstaller Section

Section "un.Install Iguana"
   DetailPrint "Uninstalling Iguana service..."
   ${IFW_RunOrQuitUn} "sc delete IguanaX" 10000 "uninstall Iguana service"

   DetailPrint "Uninstalling Iguana program files..."
   RMDir /R /REBOOTOK "$INSTDIR"
   RMDir "$PROGRAMFILES64\iNTERFACEWARE"  ; remove if empty

   DetailPrint "Removing Add/Remove Programs info..."
   DeleteRegKey HKLM 'Software\Microsoft\windows\CurrentVersion\Uninstall\iNTERFACEWARE Iguana X'

   DetailPrint "Removing Iguana shortcuts..."
   SetShellVarContext all
   Delete "$DESKTOP\Iguana X.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\Iguana\Iguana X.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\Iguana\Uninstall Iguana X.lnk"
   RMDir "$SMPROGRAMS\iNTERFACEWARE\Iguana"  ; remove if empty
   RMDir "$SMPROGRAMS\iNTERFACEWARE"   ; remove if empty

   IfRebootFlag +1 Done
      FileOpen $1 "$INSTDIR\reboot_required.txt" "w"
      Delete /REBOOTOK "$INSTDIR\reboot_required.txt"
   Done:
SectionEnd

;------------------------------------------------------------------------------
; Functions

; Initialization
Function .onInit
!ifdef UNINSTALLER
   WriteUninstaller "$DESKTOP\uninstaller.exe"
   Quit  ; just bail out quickly when running the "inner" installer
!endif
   ; Default install parent dir
   StrCpy $INSTDIR "$PROGRAMFILES64\iNTERFACEWARE"
   ; MessageBox MB_OK 'Install Dir = "$INSTDIR"'

   Call InitShortcutFlags  ; in PageShortcuts.nsh
FunctionEnd

Function un.onInit
   ; Parse option /NOSURVEY
   ${un.GetParameters} $R0
   ClearErrors
   ${GetOptions} $R0 "/NOSURVEY" $R1
   IfErrors +2
      StrCpy $SkipExitSurvey 1
FunctionEnd
