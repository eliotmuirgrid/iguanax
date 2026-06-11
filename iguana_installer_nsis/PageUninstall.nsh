;------------------------------------------------------------------------------
; Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
;
; Module: Nsis Iguana Installer Uninstall Page
;
; Author: John Qi
;
; Description:
;    This is not an actual page. Instead, it checks whether an existing Iguana
;    installation exists and uninstalls it.
;
; Usage:
;    !include "PageUninstall.nsh"
;    !insertmacro IFW_PAGE_UNINSTALL
;
; TODO:
;    Using a custome page to uninstall before install is an overkill.
;    Check this out: https://stackoverflow.com/q/719631
;------------------------------------------------------------------------------

Var UninstallButton
Var UninstallResult
Var UninstallNext

Function UninstallIguana
   ; Copy the uninstaller to a temp location
   GetTempFileName $R2
   CopyFiles $R0 $R2

   ; The following command runs the temp uninstaller, with an argument that
   ; tells nullsoft NOT to copy the uninstaller and spawn another process.
   ClearErrors
   ExecWait '"$R2" /NOSURVEY _?=$R1'
   IfErrors +2 +1
      Delete $R0  ; Remove actual uninstaller

   Delete $R2  ; Remove temp uninstaller

   ; Check to see if the user actually went through with the uninstall
   ${IFW_GetProductUninstallInfo} "iNTERFACEWARE Iguana X" $R3 $R4
   ${If} $R3 != ""   ; User didn't uninstall Iguana
      Return
   ${EndIf}

   IfFileExists "$R1\reboot_required.txt" +1 Done
      MessageBox MB_YESNO "\
         You must reboot your computer before iNTERFACEWARE Iguana X can be \
         re-installed, would like to reboot your computer now?" \
      /SD IDNO IDNO RebootLater
         Reboot
      RebootLater:
         MessageBox MB_OK "\
            iNTERFACEWARE Iguana X was not successfully installed.$\r$\n\
            Please try again after you have rebooted your computer." /SD IDOK
         Quit
   Done:
      ${NSD_SetText} $UninstallResult "Iguana has been removed successfully.$\r$\n$\r$\nClick Next to continue."
      EnableWindow   $UninstallButton 0  ; Disable uninstall iguana button
      EnableWindow   $UninstallNext 1    ; Enable next button
FunctionEnd

Function UninstallIguanaIfExists
   ${IFW_GetProductUninstallInfo} "iNTERFACEWARE Iguana X" $R0 $R1
   ${If} $R0 == ""   ; No Iguana installation found
      Abort
   ${EndIf}

   IfFileExists "$R0" UninstallerFound  ; Jump to Found
      MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "\
         Unable to find the iNTERFACEWARE Iguana X uninstaller at its original location.$\r$\n\
         As a result, setup cannot continue uninstalling iNTERFACEWARE Iguana X.$\r$\n\
         Continuing setup may render the current installation unusable.$\r$\n$\r$\n\
         Click OK to continue with setup or CANCEL to stop the installation." IDOK SkipUninstall
      Quit
   UninstallerFound:
      ; Set header text and sub-text
      !insertmacro MUI_HEADER_TEXT "Remove Current Installation" "Please remove the current installation before installing iNTERFACEWARE Iguana X."

      ; Instantiate a new dialog
      nsDialogs::Create 1018
      Pop $R9  ; Dont' care

      ${NSD_CreateLabel} 0 0 100% 12u "An existing Iguana installation has been detected at the following location:"
      Pop $R9  ; Dont' care

      ${NSD_CreateLabel} 8u 16u 100% 12u "$R1"
      Pop $R9  ; Dont' care

      ${NSD_CreateLabel} 0 32u 100% 24u "In order to install iNTERFACEWARE Iguana X, the current installation must be removed.$\r$\n$\r$\nClick Uninstall Iguana to launch the uninstaller."
      Pop $R9  ; Dont' care

      ${NSD_CreateButton} 25% 64u 50% 16u "Uninstall Iguana"
      Pop $UninstallButton
      ${NSD_OnClick} $UninstallButton UninstallIguana

      ${NSD_CreateLabel} 0 90u 100% 24u ""
      Pop $UninstallResult

      ; Get the Next button and disable it
      GetDlgItem $UninstallNext $HWNDPARENT 1
      EnableWindow $UninstallNext 0

      ; Display dialog
      nsDialogs::Show

   SkipUninstall:
FunctionEnd

!macro IFW_PAGE_UNINSTALL
   Page custom UninstallIguanaIfExists
!macroend