; Description:
;
; Contains scripts for ensuring that Chameleon is installed,
; and, if it is not installed, installing it.
; To do a Chameleon "search-and-install", just put the following
; code somewhere in your page sequence.
;
; Page custom LookForAndInstallChameleon
;
; Author: Kevin

!macro install_chameleon

Function ChameleonCheck

  ReadRegStr $R0 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\iNTERFACEWARE Chameleon' "UninstallString"
  ReadRegStr $R1 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\iNTERFACEWARE Chameleon' "InstallLocation"
FunctionEnd

 ; The arguments to Chameleon's installer (in addition to /S and /DIRECTORY=...)
Var listener_and_simulator
Var command_line_tools
Var SkipChameleonInstall

Section "-Initialize Variables" InitVar
  StrCpy $listener_and_simulator 0
  StrCpy $command_line_tools 0
SectionEnd

SectionGroup /e "Chameleon" SecGroupChameleon
  
   
   Section "HL7 Simulator/Listener" SecListener
      StrCpy $listener_and_simulator 1
   SectionEnd

   
   Section "Command Line Tools" SecCmdLine
      StrCpy $command_line_tools 1
   SectionEnd
   
   Section "-Silently Uninstall Chameleon" UninstallChameleon
      StrCpy $StatusUninstallChameleon "1"
      ; **********
      ; Silently Uninstall Chameleon
      ; **********
      ; ChameleonCheck, which gets the location of the uninstaller and the installation directory
      ; is never called in silent mode, it is only called in a page.
      ifSilent GetLocation ContinueUninstall

      GetLocation:
      StrCmp $SilentInstallChm "" NoChmUninstall
      Call ChameleonCheck

      ContinueUninstall:
      SetDetailsPrint textonly
      DetailPrint "Uninstalling Chameleon.  This may take a few seconds..."
      SetDetailsPrint listonly
      StrCmp $R0 "" DoneUninstall
      
      ; Actually check to see if the uninstaller is actualler there or not
      IfFileExists "$R0" ContinueUninstallChameleon 0
      MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION "It seems that the Uninstaller for Chameleon has been removed from its original location.$\r$\nAs a result, setup cannot continue with the Chameleon part of the installation.$\r$\n$\r$\nClick OK to continue and skip Chameleon Installation or CANCEL to stop the installation." IDOK +2 0
      Quit
      StrCpy $SkipChameleonInstall 1
      Goto NoChmUninstall
      ContinueUninstallChameleon:
      ; Copy the uninstaller to a temp location
      GetTempFileName $0
      CopyFiles $R0 $0
      ; The following command runs the temp uninstaller, with an argument that
      ; tells nullsoft NOT to copy the uninstaller and spawn another process.
      ClearErrors
      ExecWait '"$0" --supress_survey /S _?=$R1'
      IfErrors AbortUninstall DoneUninstall

      AbortUninstall:
      StrCpy $StatusUninstallChameleon "2"
      StrCpy $StatusErrorMessage "Please close any iNTERFACEWARE applications you may have open,$\r$\n\
                                  such as Chameleon, HL7 Listener, or HL7 Simulator, and run the installer again."
      ; TODO: need to deal with things that might have installed here
      ;       so the installer as a whole acts more elegantly
      IfSilent WarnSilentCHMUsers ProceedSilentCHMUsers

      WarnSilentCHMUsers:
      StrCpy $StatusErrorMessage "iNTERFACEWARE applications such as Chameleon, HL7 Listener, or HL7 Simulator may have been open.$\r$\n\
                                 Remove all remaining files in iNTERFACEWARE\Chameleon\ and iNTERFACEWARE\doc\, then run the silent installer again."
      DeleteRegKey HKLM 'Software\Microsoft\windows\CurrentVersion\Uninstall\iNTERFACEWARE Chameleon'
      !insertmacro CHAMELEON_MANUALLY_DELETE_SHORTCUTS
      
      ProceedSilentCHMUsers:
      delete $0
      Call iguanaAbortFunction
      Quit
      
      DoneUninstall:
      IfSilent 0 +2
      !insertmacro CHAMELEON_MANUALLY_DELETE_SHORTCUTS
      ; This needs to be done after the success of the success of the uninstaller
      ; is checked, otherwise you will get into some wierd problems. (#4354)
      Delete $R0

      NoChmUninstall:
   SectionEnd
   ; **************
   ; Silently install Chameleon
   ; **************
   Section "-Silently Install Chameleon" InstallChameleon
      StrCmp $SkipChameleonInstall 1 NoChmInstall 0
      ifSilent 0 +2
         StrCmp $SilentInstallChm "" NoChmInstall
         
      SetDetailsPrint textonly
      DetailPrint "Installing Chameleon.  This may take a few minutes..."
      SetDetailsPrint listonly
      IfFileExists "$INSTDIR\Chameleon\reboot_required.txt" reboot_required ContinueInstall
    
      reboot_required:
      StrCpy $StatusUninstallChameleon "2"
      StrCpy $StatusErrorMessage "Some Chameleon files could not be deleted.$\r$\n\
                                  You must reboot your computer before iNTERFACEWARE Chameleon and Iguana can be installed."
      Call iguanaAbortFunction
      Quit
    
      ContinueInstall:
      StrCpy $StatusUninstallChameleon "3"
      StrCpy $StatusInstallChameleon "1"
      ; Important!!!
      ; For this section to work, you must map \\release\release
      ; to windows' R: drive.
      ;
      ; CHAMELEON_INSTALLER_EXE_PATH must be defined before this macro is called
      ; where R: is the shared network drive to release server
      ; e.g "R:\chameleon\windows\latest\chameleon_windows.exe"
      ;
      SetOutPath "$TEMP"
      File /oname=chameleon.exe ${CHAMELEON_INSTALLER_EXE_PATH}
      ClearErrors
      ExecWait '"$TEMP\chameleon.exe" /S \
              /LISTENERANDSIMULATOR=$listener_and_simulator \
              /TOOLS=$command_line_tools \
              /STUBCODE=1 \
              /DOTNET=1 \
              /SHORTCUTS=0 \
              /DIRECTORY="$ChameleonDirectory"' $0
      IfErrors AbortInstall DoneInstall
      AbortInstall:
         Call iguanaAbortFunction
         Quit
      DoneInstall:
      # There were no errors finding/executing the Chameleon installer, but the Chameleon
      # installer itself might have had an error.  If so, we'll warn the user that they
      # may need to install Chameleon.
      StrCmp $0 0 DeleteChameleonInstaller
         MessageBox MB_OK|MB_ICONSTOP "An error was encountered while installing Chameleon.$\r$\n\
                                       You may need to re-install it (it can be downloaded separately from http://www.interfaceware.com/chameleon.html)."

      DeleteChameleonInstaller:
         # /REBOOTOK says that if this file can't be deleted, we'll just schedule for
         # deletion on the next reboot.  But we don't want the installer's reboot flag
         # to be set just for this.
         Delete /REBOOTOK "$TEMP\chameleon.exe"
         SetRebootFlag false

      ; Shortcuts are installed somewhere in the pages, so we must
      ; do it in silent mode SOMEWHERE!

      installShortcuts:
      SetShellVarContext all
      ; in silent mode, DesktopShortcuts and StartmenuShortcuts will be set automatically
      
      ifSilent +2 0
      StrCmp $ChmDesktopShortcut "1" 0 CheckChameleonSm
      !insertMacro CHAMELEON_CREATE_DESKTOP_SHORTCUT

      CheckChameleonSm:
      SetShellVarContext Current

      IfSilent +2 0
      StrCmp $ChmStartmenuShortcuts "1" 0 skipShortcuts
      !insertMacro CHAMELEON_CREATE_STARTMENU_SHORTCUTS

      skipShortcuts:
      StrCpy $StatusInstallChameleon "3"

      NoChmInstall:
   SectionEnd
   
SectionGroupEnd

LangString DESC_SecGroupChameleon ${LANG_ENGLISH} "iNTERFACEWARE's HL7 messaging toolkit.  If Chameleon is already installed, it will be upgraded."
LangString DESC_SecListener ${LANG_ENGLISH} "HL7 Simulator and HL7 Listener tools."
LangString DESC_SecCmdLine ${LANG_ENGLISH} "Command line tools msgdiff, msgtransform, and msgxml."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
   !insertmacro MUI_DESCRIPTION_TEXT ${SecGroupChameleon} $(DESC_SecGroupChameleon)
   !insertmacro MUI_DESCRIPTION_TEXT ${SecListener} $(DESC_SecListener)
   !insertmacro MUI_DESCRIPTION_TEXT ${SecCmdLine} $(DESC_SecCmdLine)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onSelChange
  !insertmacro SectionFlagIsSet ${SecGroupChameleon} ${SF_SELECTED} chm_install no_chm_install
   no_chm_install:
      StrCpy $SkipChameleonShortcuts "1"
      goto done
   chm_install:
      StrCpy $SkipChameleonShortcuts "0"
   done:
FunctionEnd

Function chameleonDirPre
   !insertmacro SectionFlagIsSet ${SecGroupChameleon} ${SF_SELECTED} done skipChameleonDirectorySelect
   skipChameleonDirectorySelect:
      Abort
   done:
FunctionEnd

Function chameleonDirShow
   !insertmacro MUI_HEADER_TEXT "Choose Chameleon Install Location" "Choose the folder in which to install iNTERFACEWARE Chameleon."
   !insertmacro MUI_INNERDIALOG_TEXT 1019 "$PROGRAMFILES\iNTERFACEWARE"
   !insertmacro MUI_INNERDIALOG_TEXT 1006 "Setup will install iNTERFACEWARE Chameleon in the following folder. To install in a different folder, click Browse and select another folder. Click Next to continue."
FunctionEnd

Function chameleonDirLeave
   GetInstDirError $0
   StrCmp $0 "1" 0 +3
      StrCpy $StatusErrorMessage "Chameleon directory is invalid."

   StrCmp $0 "2" 0 +3
      StrCpy $StatusErrorMessage "The partition does not have enough space available.$\r$\nFree up some space before proceeding."

   StrCmp $0 "0" NoDirLeaveErrors 0
   MessageBox MB_OK "$StatusErrorMessage"
   StrCpy $StatusErrorMessage ""
   Abort

   NoDirLeaveErrors:
      StrCpy $ChameleonDirectory $INSTDIR
      StrCpy $INSTDIR $IguanaDirectory

   done:
FunctionEnd

!macroend

; Desktop and startmenu shortcuts.

; Create Chameleon desktop shortcut
; extra_path will be appended to the instdir.
; This is used when chameleon is installed as part of a different
; installation, and so $INSTDIR refers to some directory other than
; the one in which Chameleon is installed.
!macro CHAMELEON_CREATE_DESKTOP_SHORTCUT
   SetShellVarContext current
   Delete "$DESKTOP\iNTERFACEWARE Chameleon.lnk"
   Delete "$DESKTOP\Chameleon.lnk"

   SetShellVarContext all
   Delete "$DESKTOP\iNTERFACEWARE Chameleon.lnk"
   Delete "$DESKTOP\Chameleon.lnk"
   
   ReadRegStr $0 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\iNTERFACEWARE Chameleon' "InstallLocation"
   IfFileExists "$DESKTOP\Chameleon.lnk" lblNoChameleonIcon
      !insertmacro CREATE_SHORTCUT "$DESKTOP\Chameleon.lnk" "$0\VMstudio.exe" "" "" "$0"
   lblNoChameleonIcon:
!macroend


Function DeleteAllChameleonShortcuts
   Delete "$DESKTOP\Chameleon.lnk"
   Delete "$DESKTOP\iNTERFACEWARE Chameleon.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\Chameleon\iNTERFACEWARE Chameleon.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon Help Manual.lnk"
   ;#9614 - Chameleon uninstalls the manual... and Iguana does not install manual. So uninstall Chameleon means uninstall Iguana manual
   Delete "$SMPROGRAMS\iNTERFACEWARE\Iguana\Iguana Help Manual.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Uninstall Chameleon.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\HL7 Listener.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\HL7 Simulator.lnk"
   Delete "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\Message Browser.lnk"
   RMDir "$SMPROGRAMS\iNTERFACEWARE\Chameleon"
   RMDir "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools"
FunctionEnd

!macro CHAMELEON_MANUALLY_DELETE_SHORTCUTS
   SetShellVarContext all
   Call DeleteAllChameleonShortcuts
   SetShellVarContext current
   Call DeleteAlLChameleonShortcuts
!macroend
; Create Chameleon start menu shortcuts
; extra_path will be appended to the instdir.
; This is used when chameleon is installed as part of a different
; installation, and so $INSTDIR refers to some directory other than
; the one in which Chameleon is installed.
!macro CHAMELEON_CREATE_STARTMENU_SHORTCUTS

  ; Start menu directories
  CreateDirectory "$SMPROGRAMS\iNTERFACEWARE"
  CreateDirectory "$SMPROGRAMS\iNTERFACEWARE\Chameleon"
  CreateDirectory "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools"
  
  ReadRegStr $0 HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\iNTERFACEWARE Chameleon' "InstallLocation"
  StrCmp $0 "" EndChameleonShortcutLabel
  ; Chameleon.lnk
  IfFileExists "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon.lnk" lblChameleonManual
  !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon.lnk" "$0\VMstudio.exe" "" "" "$0"

  ; Chameleon Help Manual.lnk
  lblChameleonManual:
  IfFileExists "$0\manual.chm" inChameleonDir inDocDir
  inChameleonDir:
    IfFileExists "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon Help Manual.lnk" doneManualShortcut
       !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon Help Manual.lnk" "$0\manual.chm" "" "" "$0"
    Goto doneManualShortcut
  inDocDir:
    IfFileExists "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon Help Manual.lnk" doneManualShortcut
       !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon Help Manual.lnk" "$0\..\doc\manual.chm" "" "" "$0\..\doc"
  doneManualShortcut:

  ; Uninstall Chameleon.lnk
  IfFileExists "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Uninstall Chameleon.lnk" lblListenerShortcut
     !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Uninstall Chameleon.lnk" "$0\uninstall.exe" "" "" "$0"

  ; HL7 Listener.lnk
  lblListenerShortcut:
  IfFileExists "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\HL7 Listener.lnk" lblSimulatorShortcut
     !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\HL7 Listener.lnk" "$0\Listener.exe" "" "" "$0"

  ; HL7 Simulator.lnk
  lblSimulatorShortcut:
  IfFileExists "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\HL7 Simulator.lnk" lblBrowserShortcut
     !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\HL7 Simulator.lnk" "$0\Simulator.exe" "" "" "$0"

  ; Message Browser.lnk
  lblBrowserShortcut:
  IfFileExists "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\Message Browser.lnk" lblLastStepShortcut
     !insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools\Message Browser.lnk" "$0\VMstudio.exe" "-B" "$0\browser.ico" "$0"
  ; Chameleon Change Log.lnk
  ;!insertmacro CREATE_SHORTCUT "$SMPROGRAMS\iNTERFACEWARE\Chameleon\Chameleon Change Log.lnk" "$INSTDIR\change_log_chameleon.txt" "" ""

  ; Now, if HL7 tools weren't installed, the HL7 Analytical tools
  ; directory will be empty.  Same goes for the Chameleon directory.
  ; So, we will just issue a command to delete these directories.
  ; They will only actually be deleted if they are empty.
  lblLastStepShortcut:
  RMDir "$SMPROGRAMS\iNTERFACEWARE\Chameleon"
  RMDir "$SMPROGRAMS\iNTERFACEWARE\HL7 Analytical Tools"

  EndChameleonShortcutLabel:
!macroend

