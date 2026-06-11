
!define IFW_RunCommand "!insertmacro runCommand ''"
Var InstallStep

!macro runCommand un Command Timeout Error
   Push "${Error}"
   Push "${Timeout}"
   Push "${Command}"
   Call RunTheCommand
!macroend

Function RunTheCommand
   Pop $0   ; Command
   Pop $1   ; Timeout
   Pop $2   ; ExecToStack
   nsExec::ExecToStack /TIMEOUT=$1 '$0'
   Pop $R0
   Pop $R1
   ${NSD_CreateLabel} 0 42u 100% 80u "$R1"
   ${If} $R0 == 0
      GetDlgItem $0 $HWNDPARENT 1
      EnableWindow $0 1
      StrCpy $InstallStep "2"
      nsDialogs::Show
   ${EndIf}
FunctionEnd


Function InstallPage
   !insertmacro MUI_HEADER_TEXT "Installing Iguana Windows" "Last step in the installation process"
   nsDialogs::Create 1018
   ${IF} $InstallType == "System"
      ; Reset any entered username and password.
      StrCpy $WindowsUserName ""
      StrCpy $WindowsPassword ""
   ${EndIf}

   ${IFW_RunCommand} "${IFW_BINARY} --service install --username $\"$WindowsUserName$\" --password $\"$WindowsPassword$\"" 10000 "install Iguana service"
   DetailPrint "Installing Iguana service..."
   GetDlgItem $0 $HWNDPARENT 1
   EnableWindow $0 0
   nsDialogs::Show
FunctionEnd

!macro IFW_INSTALL_PAGE
   Page Custom InstallPage
!macroend
