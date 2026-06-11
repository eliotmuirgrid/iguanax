; Description:
;
; Various functions and macros used in both the Chameleon and Iguana
; installers.
;
; Author: Kevin Senn


;--------------------------------
; Functions

; Insert the functions into comipling code of a script by inserting this macro
; These functions are ones which will (probably) be used by all iNTERFACEWARE
; installers.  If you get a warning that you're not using one of them,
; consider using it - they're here for a reason.
!macro DEFINE_COMMON_FUNCTIONS

!ifdef REMEMBER_UNINSTALL_DIRECTORY
Var UNINSTDIR
!endif


; This function is called when the script manually exits due to some
; problem (for example, user doesn't want to uninstaller first)
Function QuitInstaller
  MessageBox MB_OK "${INSTALL_APPLICATION_NAME} was not successfully installed."  /SD IDOK
  Quit
FunctionEnd


; Disable the "Back" button on the welcome page
Function SetupWelcomePage
  !insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "BackEnabled" "0"
FunctionEnd


; Disable the "Cancel" button on the finish page
Function DisableCancelButton
  !insertmacro MUI_INSTALLOPTIONS_WRITE "ioSpecial.ini" "Settings" "CancelEnabled" "0"
FunctionEnd

; This function is automatically called if a Reboot call fails
Function .onRebootFailed
   MessageBox "MB_OK|MB_ICONSTOP" "Reboot failed. Restart manually from the start menu." /SD IDOK
   Quit
FunctionEnd

!macroend  ; DEFINE_COMMON_FUNCTIONS


; This contains functions for use in cleaning up old iNTERFACEWARE
; applications, installed by WISE or NSIS.
!macro DEFINE_INTERFACEWARE_CLEANUP

!include "${NSI_LIB}\wise-uninstall.nsh"

; Locate old installation directories.  We have to take into account
; the user may have the old directory structure (which could possibly
; have two chameleon installations), or the new directory structure.
Function LocateOldInstallations

!ifdef REMEMBER_UNINSTALL_DIRECTORY
  StrCpy $UNINSTDIR "" ; initialize variable
!endif


  ; This will look for old iNTERFACEWARE installations.  Note that
  ; with the old installers, it is possible for a user to have two
  ; seperate instances of Chameleon installed (one in the Chameleon
  ; directory, and one in the Iguana directory).  In this case we
  ; will only ask once if they want to remove this software.

  ; We may need to use install_shot to clean up PATH, so let's
  ; install it now (into the TEMP directory).  We install it
  ; into the TEMP directory because we will delete it once this
  ; search is done.  Yes, this does mean that we will likely
  ; install install_shot twice in this installation, but at this
  ; point we don't know whether we will actually end up installing
  ; an application, or where we will install it to, but we want
  ; cleaning up PATH to be a part of the wise uninstallation, so
  ; we don't want to leave it until later in the uninstaller.
  SetOutPath $TEMP
  File ${BUILD_DIR}\install_shot\install_shot.exe

  ; If Chameleon is installed, the path of where there SHOULD be
  ; an uninstaller can be found in the registry key below.
  StrCpy $R3 "iNTERFACEWARE Chameleon"
  searchforR3:
    ReadRegStr $R0 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\$R3' 'UninstallString'
    StrCmp $R0 "" done
    ${WordFind} $R0 "UNWISE.exe" "#" $0
    StrCmp $0 1 foundwise done
    foundwise:
      StrCpy $R2 "WISE"
      ; $R0, $R2, and $R3 are arguments to FoundInstallation
      Call FoundInstallation
  done:
    ; If we've just looked for Chameleon, we still need to look for
    ; Iguana.  So if $R3 is "iNTERFACEWARE Chameleon", set it to
    ; "iNTERFACEWARE Iguana" and search for $R3 again.  Otherwise, we
    ; were looking for iNTERFACEWARE Iguana, and therefore we've
    ; already looked for iNTERFACEWARE Chameleon, so we can exit the
    ; function.
    StrCmp $R3 "iNTERFACEWARE Chameleon" 0 reallydone
    StrCpy $R3 "iNTERFACEWARE Iguana"
    Goto searchforR3
  reallydone:

    Delete "$TEMP\install_shot.exe"

FunctionEnd

; Look for an NSIS installation
Function LocateNewInstallations

  ; Now search for an NSIS installation.
  ReadRegStr $0 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALL_APPLICATION_NAME}' "UninstallString"
  ; if $0 is empty, there is no NSIS installation.
  StrCmp $0 "" done foundnsis

  foundnsis:
    StrCpy $R0 $0
    ReadRegStr $R1 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALL_APPLICATION_NAME}' "InstallLocation"
    StrCpy $R2 "NSIS"
    StrCpy $R3 "${INSTALL_APPLICATION_NAME}"
    Call FoundInstallation

done:

FunctionEnd

; This function is called whenever an iNTERFACEWARE installation path is found
; Input :
; $R0 is the path of the uninstaller (whether it exists or not)
; $R1 is the directory where Chameleon (and/or Iguana) is installed.
; $R2 is "WISE" or "NSIS", indicating whether we have found an
; iNTERFACEWARE application installed with the old WISE installer, or
; the new NSIS installer.
; $R3 is the name of the product found (either "iNTERFACEWARE Chameleon"
; or "iNTERFACEWARE Iguana")
Function FoundInstallation
  StrCpy $StandaloneUninstall "0"
  StrCmp $R2 "WISE" wise nsis

  wise:

    ; Extract the path of the application directory
    StrLen $0 $R0
    ; The path of the uninstaller will always end with "\UNWISE.EXE"
    ; for any old installation.  We will strip off these 11 characters.
    IntOp $0 $0 - 11
    StrCpy $R1 $R0 $0

    ; We only want to bother asking the user if they want to remove old
    ; iNTERFACEWARE applications once.  So what we will do is set the
    ; variable $1 to be alreadyAsked if we've already asked the user.
    ; If we find that we've already asked, we'll just go straight to
    ; removing it.
    ; if we've already asked, goto remove
    StrCmp $2 "alreadyAsked" remove
    ; else, assign $2 the value of alreadyAsked
    StrCpy $2 "alreadyAsked"
    
    ; Extract the page (prepare it for display)
    !insertmacro MUI_INSTALLOPTIONS_EXTRACT "foundOldPage.ini"

    !insertmacro MUI_HEADER_TEXT_PAGE "Found Old Applications" "Do you want to uninstall old iNTERFACEWARE applications?"  

    ; Display the page, and get the return value (the button the user
    ; pressed).  Value is returned on the stack.
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY_RETURN "foundOldPage.ini"

    ; If user cancelled, quit.  Otherwise, remove installation.
    Pop $0
    StrCmp $0 "cancel" quit
    MessageBox MB_OKCANCEL "Setup will now remove old iNTERFACEWARE applications" /SD IDOK IDCANCEL quit
    Goto remove
    quit:
     Delete "$TEMP\install_shot.exe"
     Call QuitInstaller
     goto functionDone
    remove:
      Call RemoveOldInstallation
      goto functionDone

  nsis:
    
    ; If the uninstaller doesn't exist, there's not much we can do...
    ; They probably cleared out the entire directory manually, but
    ; we can't really be sure of anything.  So we will just assume
    ; that because the uninstaller is gone, we can safely skip
    ; the uninstallation.  We do this because of ticket #1713.
    IfFileExists "$R0" ContinueUninstall 0
      MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "It seems that the uninstaller for ${INSTALL_APPLICATION_NAME} has been removed from its original location.$\r$\n\
                                                 As a result, setup cannot continue uninstalling ${INSTALL_APPLICATION_NAME}.$\r$\n\
                                                 Continuing setup may render the current installation unusable.$\r$\n$\r$\n\
                                                 Click OK to continue with setup or CANCEL to stop the installation." IDOK +2 0
      Quit
      services::SendServiceCommand 'stop' 'Iguana'
      Goto functiondone
    ContinueUninstall:
    MessageBox MB_YESNO "A previous installation of $R3 has been found.  This must be uninstalled before re-installing $R3.  Would you like to do this now?" /SD IDNO IDYES uninstallNSIS IDNO exit

    uninstallNSIS:
      IfSilent functiondone
      ; Copy the uninstaller to a temp location
      GetTempFileName $0
      CopyFiles $R0 $0
      ; The following command runs the temp uninstaller, with an argument that
      ; tells nullsoft NOT to copy the uninstaller and spawn another process,
      ; and an argument that tells the uninstaller that we are upgrading, so
      ; not necessarily everything has to be removed.
      ExecWait '"$0" --upgrade --supress_survey _?=$R1'
      ; Delete the uninstaller, if it was left behind
      ; But don't delete it if the installer quit with errors.
      IfErrors +2 0
      Delete $R0

      ; Check to see if the user actually went through with the uninstall
      ReadRegStr $0 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${INSTALL_APPLICATION_NAME}' "UninstallString"
      StrCmp $0 "" itsgone exit
      itsgone:
        IfFileExists "$R1\reboot_required.txt" reboot noreboot
        reboot:
          ; Unfortunately we have no way of knowing what the user
          ; selected when the uninstaller asked them if they wanted to
          ; reboot, so we'll have to ask them again.
          MessageBox MB_YESNO "You must reboot your computer before $R3 can be re-installed, would like to reboot your computer now?" /SD IDNO IDNO exit
          Reboot
        noreboot:
          Goto functiondone ; continue with installer

    exit:
      Call QuitInstaller

  functiondone:
  
!ifdef REMEMBER_UNINSTALL_DIRECTORY
  StrCpy $UNINSTDIR $R1 ; Remember for later where we're uninstalling from
!endif

FunctionEnd


; Removes an old installation of Chameleon or Iguana
; $R0 is the path of the uninstaller.  The old WISE installer, it
; seems, does not always properly install the UNWISE.EXE uninstaller.
; so we are forced to write our own.
; This does NOT remove licensing information from the registry.
; $R1 is the main application's directory
; $R2 is the type of uninstall to perform (see description for
; FoundInstallation)
; $R3 is the name of the product found (either "iNTERFACEWARE Chameleon"
; or "iNTERFACEWARE Iguana")
Function RemoveOldInstallation
  StrCmp $R2 "WISE" wise done

  wise:
    
    ; Remove all files that WISE put there, and clean up the registry
    ; Important: calling this function requires that $R1 is the
    ; applications directory.
    
    ; WISE_UNINSTALL likes to know what application we're trying to
    ; install with this installer, so we pass it in as $R4
    StrCpy $R4 ${INSTALL_APPLICATION_NAME}
    !insertmacro WISE_UNINSTALL

  done:

FunctionEnd

!macroend ; DEFINE_INTERFACEWARE_CLEANUP

!macro CREATE_CHAMELEON_SHORCUT_PAGE
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Settings" "NumFields" "7"
   
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 5" "Type" "Groupbox"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 5" "Left" "4"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 5" "Top" "76"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 5" "Right" "291"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 5" "Bottom" "126"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 5" "Text" "Chameleon"

   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "Type" "CheckBox"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "Left" "12"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "Top" "89"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "Right" "200"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "Bottom" "100"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "Text" "Desktop shortcut"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 6" "State" "1"
   
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "Type" "CheckBox"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "Left" "12"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "Top" "105"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "Right" "200"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "Bottom" "113"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "Text" "Start menu shortcuts"
   !insertmacro MUI_INSTALLOPTIONS_WRITE "shortcutsPage.ini" "Field 7" "State" "1"
   
!macroend

!macro SETUP_SHORTCUTS_PAGE_NEW
   SetShellVarContext all
   ;WriteINIStr "$PLUGINSDIR\configurationPage.ini" "Field 2" "State" "$3"
   ;WriteINIStr "$PLUGINSDIR\configurationPage.ini" "Field 5" "State" "$4"
   
   !insertmacro MUI_INSTALLOPTIONS_EXTRACT "shortcutsPage.ini"
   !insertmacro MUI_HEADER_TEXT "Desktop and Start Menu Shortcuts" "Select the shortcuts to be created with this installation."

   StrCmp $SkipChameleonShortcuts "1" renderPage chm_install
   chm_install:
      !insertmacro CREATE_CHAMELEON_SHORCUT_PAGE
   renderPage:
   ; This initializes the page, but doesn't show it yet.
   !insertmacro MUI_INSTALLOPTIONS_INITDIALOG "shortcutsPage.ini"
   ; Show the page
   !insertmacro MUI_INSTALLOPTIONS_SHOW
!macroend

; This macro can be used inside a function to diplay the shortcuts
; page
!macro SETUP_SHORTCUTS_PAGE

  ; Set the installer to always install to "All Users" desktop and
  ; start menu.
  SetShellVarContext all

  ; This puts the configuration file on the target machine.  When the
  ; user checks/unchecks a box, the ini file will be changed to
  ; reflect this.  We can read these changes easily.  NSIS will
  ; automatically delete this ini file from the target machine once
  ; the installation is complete.
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "shortcutsPage.ini"

  ; This (somehow) makes the line at the bottom of the screen (that
  ; appears just above the back, next, and cancel buttons) turn into a
  ; solid line that goes across the page, instead of saying "nullsoft
  ; installer vX.X".  This is done because this page is meant to
  ; resemble the finish page.  This code was taken from the
  ; MUI_FUNCTION_FINISHPAGE macro in
  ; NSIS program files\contrib\MUI\System.nsh.
  LockWindow on
    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1028
    ShowWindow $MUI_TEMP1 ${SW_HIDE}

    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1256
    ShowWindow $MUI_TEMP1 ${SW_HIDE}

    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1035
    ShowWindow $MUI_TEMP1 ${SW_HIDE}

    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1037
    ShowWindow $MUI_TEMP1 ${SW_HIDE}

    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1038
    ShowWindow $MUI_TEMP1 ${SW_HIDE}

    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1039
    ShowWindow $MUI_TEMP1 ${SW_HIDE}

    GetDlgItem $MUI_TEMP1 $HWNDPARENT 1045
    ShowWindow $MUI_TEMP1 ${SW_NORMAL}
  LockWindow off

  ; This initializes the page, but doesn't show it yet.
  !insertmacro MUI_INSTALLOPTIONS_INITDIALOG "shortcutsPage.ini"

  ; This sets the background colour of the page, and all items on the
  ; page (for example, text and checkboxes) to white.  This code was
  ; also taken from the MUI_FUNCTION_FINISHPAGE macro.
    Pop $MUI_HWND
    SetCtlColors $MUI_HWND "" "${MUI_BGCOLOR}"

    GetDlgItem $MUI_TEMP1 $MUI_HWND 1201
    SetCtlColors $MUI_TEMP1 "" "${MUI_BGCOLOR}"

    CreateFont $MUI_TEMP2 "$(^Font)" "12" "700"
    SendMessage $MUI_TEMP1 ${WM_SETFONT} $MUI_TEMP2 0

    GetDlgItem $MUI_TEMP1 $MUI_HWND 1202
    SetCtlColors $MUI_TEMP1 "" "${MUI_BGCOLOR}"

    GetDlgItem $MUI_TEMP1 $MUI_HWND 1203
    SetCtlColors $MUI_TEMP1 "" "${MUI_BGCOLOR}"

    GetDlgItem $MUI_TEMP1 $MUI_HWND 1204
    SetCtlColors $MUI_TEMP1 "" "${MUI_BGCOLOR}"

  ; Show the page
  !insertmacro MUI_INSTALLOPTIONS_SHOW

!macroend

; Create a shortcut (the smart way) This function will intelligently
; create shortcuts by doing the following:
;  1. Only creating the shortcut if the target file exists, and
;  2. Not allowing duplicate shortcuts.
!macro CREATE_SHORTCUT shortcut_name target_file args icon working_dir
  StrCpy $R0 '${shortcut_name}'
  StrCpy $R1 '${target_file}'
  StrCpy $R2 '${args}'
  StrCpy $R3 '${icon}'
  StrCpy $R4 '${working_dir}'
  Call CreateShortCutWrapper
!macroend
Function CreateShortCutWrapper
  IfFileExists $R1 create done
  create:
    ; To prevent duplicate shortcuts, we will first delete the
    ; shortcut name
    Delete $R0
    SetOutPath $R4
    CreateShortCut '$R0' '$R1' '$R2' '$R3'
  done:
FunctionEnd


;;;;;;;;;;;;;;;;;;;
; Snapshot macros ;
;;;;;;;;;;;;;;;;;;;

!macro BEFORE_SNAPSHOT
  Section "-Take Initial Snapshot"
    SetDetailsPrint textonly
    DetailPrint "Initializing..."
    SetDetailsPrint listonly
    SetOutPath "$INSTDIR"
    File ${BUILD_DIR}\install_shot\install_shot.exe ; Put install_shot on the target machine
    nsExec::ExecToLog /TIMEOUT=3000 '"$INSTDIR\install_shot" before-install.log'
  SectionEnd
!macroend

!macro AFTER_SNAPSHOT
  Section "-Log Files"
    SetOutPath "$INSTDIR"
    nsExec::ExecToLog /TIMEOUT=5000 '"$INSTDIR\install_shot" after-install.log'
    nsExec::ExecToLog /TIMEOUT=3000 '"$INSTDIR\install_shot" --before before-install.log --after after-install.log install.log'
    Delete "$INSTDIR\before-install.log"
    Delete "$INSTDIR\after-install.log"
    Delete "$INSTDIR\install_shot.exe"
  SectionEnd
!macroend


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Get command line parameter values ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; This macro is from the NSIS wiki
; Example:
; !insertmacro GetParameterValue "/L=" "1033"
; pop $R0
 ; load language from command line /L=1033
 ; foo.exe /S /L=1033 /D=C:\Program Files\Foo
 ; or:
 ; foo.exe /S "/L=1033" /D="C:\Program Files\Foo"
 ; gpv "/L=" "1033"
 !macro GET_PARAMETER_VALUE SWITCH DEFAULT
   Push $0
   Push $1
   Push $2
   Push $3
   Push $4
 
 ;$CMDLINE='"My Setup\Setup.exe" /L=1033 /S'
   Push "$CMDLINE"
   Push '${SWITCH}"'
   !insertmacro StrStr
   Pop $0
   StrCmp "$0" "" gpv_notquoted
 ;$0='/L="1033" /S'
   StrLen $2 "$0"
   Strlen $1 "${SWITCH}"
   IntOp $1 $1 + 1
   StrCpy $0 "$0" $2 $1
 ;$0='1033" /S'
   Push "$0"
   Push '"'
   !insertmacro StrStr
   Pop $1
   StrLen $2 "$0"
   StrLen $3 "$1"
   IntOp $4 $2 - $3
   StrCpy $0 $0 $4 0
   Goto gpv_done
 
   gpv_notquoted:
   Push "$CMDLINE"
   Push "${SWITCH}"
   !insertmacro StrStr
   Pop $0
   StrCmp "$0" "" gpv_done
 ;$0='/L="1033" /S'
   StrLen $2 "$0"
   Strlen $1 "${SWITCH}"
   StrCpy $0 "$0" $2 $1
 ;$0=1033 /S'
   Push "$0"
   Push ' '
   !insertmacro StrStr
   Pop $1
   StrLen $2 "$0"
   StrLen $3 "$1"
   IntOp $4 $2 - $3
   StrCpy $0 $0 $4 0
   Goto gpv_done
 
   gpv_done:
   StrCmp "$0" "" 0 +2
   StrCpy $0 "${DEFAULT}"
 
   Pop $4
   Pop $3
   Pop $2
   Pop $1
   Exch $0
 !macroend
 
; And I had to modify StrStr a tiny bit.
; Possible upgrade switch the goto's to use ${__LINE__}
 
!macro STRSTR
  Exch $R1 ; st=haystack,old$R1, $R1=needle
  Exch    ; st=old$R1,haystack
  Exch $R2 ; st=old$R1,old$R2, $R2=haystack
  Push $R3
  Push $R4
  Push $R5
  StrLen $R3 $R1
  StrCpy $R4 0
  ; $R1=needle
  ; $R2=haystack
  ; $R3=len(needle)
  ; $R4=cnt
  ; $R5=tmp
 ;  loop;
    StrCpy $R5 $R2 $R3 $R4
    StrCmp $R5 $R1 +4
    StrCmp $R5 "" +3
    IntOp $R4 $R4 + 1
    Goto -4
 ;  done;
  StrCpy $R1 $R2 "" $R4
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch $R1
!macroend
