; Description:
;
; Contains a function used to register .NET components.
;
; Author: Kevin Senn

; If the .NET framework is found, go to Found, otherwise,
; go to NotFound.
; If it is found, $8 will be set with the path of fusion.dll,
; otherwise, $8 will be set as an empty string
!macro SEARCH_FOR_DOTNET Found NotFound

  StrCpy $8 "" ; $8 will be the path of fusion.dll

  ; Find the first .NET directory.  $0 is the "handle" to this search.
  ; This can be used in the FindNext function.  The filename is put
  ; into $1.
  ; $2 is the .NET Framework directory.
  StrCpy $2 "$WINDIR\Microsoft.NET\Framework"
  FindFirst $0 $1 "$2\v*"
  ; First thing it will find is "." (current directory) so we don't
  ; need to check if it contains fusion.dll
  loop:
    StrCmp $1 "" endloop ; if nothing found, end the loop
    IfFileExists "$2\$1\fusion.dll" foundFusion doneFusionSearch
    foundFusion:
      StrCmp $8 "" writeFusion warnUser
      warnUser:
        StrCmp $AlreadyWarnedUser 1 writeFusion
        StrCpy $AlreadyWarnedUser 1
        ; Because Chameleon can handle generating code for .NET 1.1 or 2.0 (see #4124) I have reduced this
        ; message to just an informational message for users about the fact they have multiple .NET SDKs on their machine.
        ; This has been removed as per #4959.
        ; MessageBox MB_OK "Note that multiple versions of the Microsoft .NET Framework were detected on your computer." /SD IDOK
      writeFusion:
      StrCpy $8 "$2\$1\fusion.dll"
    doneFusionSearch:
    FindNext $0 $1
    Goto loop
  endloop:

  StrCmp $8 "" ${NotFound} ${Found}
  
!macroend

; Register .NET component pushed on the top of the stack.  If multiple
; .NET versions exist on the target machine, warn users that the most
; current version will be used.  We will define "multiple versions" as
; having multiple fusion.dll files, because that's the only case where
; we would have to make a choice.
Var AlreadyWarnedUser
Function RegisterDotNetObject

  !insertmacro SEARCH_FOR_DOTNET ContinueRegister noDotNet
  
  ContinueRegister:
  
  Pop $9 ; Get the argument to this function, the dll to register

  ; else, at this point we have the path of fusion.dll stored in $8
  nsExec::ExecToStack /TIMEOUT=3000 '"$INSTDIR\install_shot" --dotnetRegister "$9"  --fusion "$8"'
  Pop $R0 ; return value from ExecToStack (we don't care about it though, since apparently firewalls can modify it - see #3380)
  Pop $R1 ; output from install_shot
  ${WordFind} $R1 "Registered" "#" $0 ; if "Registered" is not found, $0 will be set to $R1
  StrCmp $0 $R1 0 done ; if $0 == $R1 continue, otherwise, goto done
    MessageBox MB_OK "Setup could not register $9.  Reason: $R1$\r$\n\
    This will not prevent ${INSTALL_APPLICATION_NAME} from working, all this means is that $9 is not in your Global Assembly Cache (GAC)" /SD IDOK
    Goto done

  noDotNet:
    StrCmp $AlreadyWarnedUser 1 done
    StrCpy $AlreadyWarnedUser 1
    MessageBox MB_OK "No version of the  Microsoft .NET Framework was detected.  .NET objects will not be registered." /SD IDOK

  done:
FunctionEnd

; Unregister the .NET component pushed on the top of the stack.  If 
; multiple .NET versions exist on the target machine, warn users that 
; the most current version will be used.  We will define "multiple 
; versions" as having multiple fusion.dll files, because that's the 
; only case where we would have to make a choice.
Function UnregisterDotNetObject

  !insertmacro SEARCH_FOR_DOTNET ContinueUnregister done

  ContinueUnregister:

  Pop $9 ; Get the argument to this function, the component to unregister.

  ; At this point we have the path of fusion.dll stored in $8.
  nsExec::ExecToStack /TIMEOUT=3000 '"$INSTDIR\install_shot" --dotnetUnregister "$9"  --fusion "$8"'
  Pop $R0 ; Return value from ExecToStack.
  Pop $R1 ; Output from install_shot.
  ; We ignore the output.

  done:
FunctionEnd
