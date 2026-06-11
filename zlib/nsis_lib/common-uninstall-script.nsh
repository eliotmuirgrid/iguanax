; Description:
;
; This NSIS header file contains macros and functions used in both the
; Chameleon and Iguana uninstaller.
;
; Author: Kevin Senn

;--------------------------------
; Functions

; Reads filenames from a snapshot file and deletes them.  Push the
; name of the log file onto the stack before calling the function.
; Note: this function expects the filenames in the snapshot file to be
; seperated by \r\n.  This is important because the FileRead function
; does not strip these characters, so we need to explicitly strip the
; two characters ourselves.

Function Un.Survey
   StrCmp $StandaloneUninstall "0" EndUninstall
   MessageBox MB_YESNO "Thank you for choosing iNTERFACEWARE.$\r$\n$\r$\nWould you be willing to help us improve our product by completing a 5 minute on-line survey?" IDNO +2
   ExecShell "Open" "http://www.interfaceware.com/exit_survey"
   EndUninstall:
FunctionEnd


Function un.UninstallFromSnapshot
  ; crucial step to check whether we can actually remove CHM_LIB3.dll
  ClearErrors
  Delete "$INSTDIR\CHM_LIB3.dll" ;no reboot ok flag means error flag will be set if not deletable.
  ifErrors DllInUse LookForInstallLog
  DllInUse:
  ; TODO: Call un.abortfunction
  StrCpy $StatusUninstallIguana "2"
  StrCpy $StatusErrorMessage "$INSTDIR\CHM_LIB3.dll is in use by another application. Resolve this conflict before continuing."
  Call un.iguanaAbortFunction
  

  LookForInstallLog:
  ; open log file and put file handle in $1
  FileOpen $1 "$INSTDIR\install.log" "r"
  ; If we don't find install.log quietly ignore it and skip to the end.  See #24530
  StrCmp $1 "" quitFunction loop

  loop:
    ; read file and put line into $0
    FileRead $1 $0
    ; if $0 is empty (EOF), we are done.
    StrCmp $0 "" done
    ; else
    
    ; We need to strip newline characters, since FileRead doesn't do
    ; that for us
    StrLen $5 $0 ; store length of $0 in $5
    IntOp $5 $5 - 2 ; decrease $5 by 2
    StrCpy $0 $0 $5 ; copy $0 to itself, for only $5 characters
    
    ; check if it is a directory - this is how the NSIS manual tells
    ; you to check if a file is a directory...
    IfFileExists "$INSTDIR\$0\*.*" directory file
    file:
      Delete /REBOOTOK "$INSTDIR\$0"
      Goto loop
    directory:
      Delete "$INSTDIR\$0\*.pyc" ; #8873 - compiled python files are not deleted.
                                 ; delete all the *.pyc files before deleting the directory
      RMDir "$INSTDIR\$0"
      Goto loop

  done:
    FileClose $1
   
    Delete "$INSTDIR\install.log" ; delete the log file
  quitFunction:
FunctionEnd
