;------------------------------------------------------------------------------

; NSIS libraries
!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"

; Iguana libraries
!include "PageUser.nsh"
!include "FinishInstall.nsh"
!include "Utils.nsh"

;------------------------------------------------------------------------------
; General

; Name and file
Name "iNTERFACEWARE Iguana X"
OutFile "${IFW_OUTFILE}"

;------------------------------------------------------------------------------
; Interface Settings

; Use an image as page headers
!define MUI_HEADERIMAGE

;------------------------------------------------------------------------------
; Pages

; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_INSTFILES
!insertmacro IFW_PAGE_USER
!insertmacro IFW_FINISH_INSTALL
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;------------------------------------------------------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"

;------------------------------------------------------------------------------
; Installer Sections


Section "-Install Iguana"
   ${If} $InstallStep S!= "2"
   ; set actual install dir
      DetailPrint "Installing Iguana program files..."
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

