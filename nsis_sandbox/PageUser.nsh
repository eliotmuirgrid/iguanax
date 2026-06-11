!include "StrRep.nsh"

Var UserInput
Var PasswordInput
Var Step2
Var WindowsUserName
Var WindowsPassword

Function RenderUserName
   !insertmacro MUI_HEADER_TEXT "Windows User Credentials" "Iguana needs windows user to credentials to run it as a service under."
   nsDialogs::Create 1018
   ${NSD_CreateGroupBox} 0 13u 100% 100u "Windows User Credentials:"
   Pop $R0  ; Don't care
   ${NSD_CreateLabel} 13u 32u 100u 12u "User:"
   Pop $R0
   System::Call "advapi32::GetUserName(t .r0, *i ${NSIS_MAX_STRLEN} r1) i.r2"
   ${NSD_CreateText} 13u 47u 250u 12u "$0"
   Pop $UserInput
	${NSD_CreateLabel} 13u 71u 100u 12u "Password:"
   Pop $R0
	${NSD_CreatePassword} 13u 86u 250u 12u ""
   Pop $PasswordInput
   nsDialogs::Show
FunctionEnd

Function RememberInputs
   ${NSD_GetText} $UserInput $WindowsUserName
   ${NSD_GetText} $PasswordInput $WindowsPassword
   ${StrRep} $WindowsUserName $WindowsUserName '"' '\"'
   ${StrRep} $WindowsPassword $WindowsPassword '"' '\"'
   StrCpy $Step2 "Step 2"
FunctionEnd

!macro IFW_PAGE_USER
   Page Custom RenderUserName RememberInputs
!macroend
