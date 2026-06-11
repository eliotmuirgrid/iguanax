; Description:
;
; IguanaLicense Application
;
; Author: Eliot Muir

!include "MUI.nsh" ; modern user interface

; The build directory.  This is where we grab all the files that we
; want to install from.
!define BUILD_DIR ".."
!define APPLICATION_NAME "IguanaLicense"

; Name - this is the name of the application which appears throughout
; the installer
Name "${APPLICATION_NAME}"

; Filename - the installer filename
OutFile "${APPLICATION_NAME}_setup.exe"

; Default installation folder.
InstallDir "C:\interfaceware_bin"

;--------------------------------
; Installer Pages

!insertmacro MUI_PAGE_DIRECTORY ; Select directory
!insertmacro MUI_PAGE_INSTFILES ; Install files (with progress bar)


;--------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"


;--------------------------------
; Installer Sections

Section "Main"
  SetOutPath "$INSTDIR"
  ; Install build\module\filename out the "OutPath" set above
  File "${BUILD_DIR}\IguanaLicense\IguanaLicense.exe"
  WriteUninstaller "$INSTDIR\uninstall_${APPLICATION_NAME}.exe"
SectionEnd

Section "un.Main" ; un.<something> is automatically an uninstaller section
  Delete "$INSTDIR\IguanaLicense.exe"
  Delete "$INSTDIR\uninstall_${APPLICATION_NAME}.exe"
SectionEnd
