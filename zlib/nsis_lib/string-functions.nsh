Var STR_HAYSTACK
Var STR_NEEDLE
Var STR_CONTAINS_VAR_1
Var STR_CONTAINS_VAR_2
Var STR_CONTAINS_VAR_3
Var STR_CONTAINS_VAR_4
Var STR_RETURN_VAR

!macro define_str_contains PREFIX
Function ${PREFIX}StrContains
  Exch $STR_NEEDLE
  Exch 1
  Exch $STR_HAYSTACK
  ; Uncomment to debug
  ;MessageBox MB_OK 'STR_NEEDLE = $STR_NEEDLE STR_HAYSTACK = $STR_HAYSTACK '
    StrCpy $STR_RETURN_VAR ""
    StrCpy $STR_CONTAINS_VAR_1 -1
    StrLen $STR_CONTAINS_VAR_2 $STR_NEEDLE
    StrLen $STR_CONTAINS_VAR_4 $STR_HAYSTACK
    loop:
      IntOp $STR_CONTAINS_VAR_1 $STR_CONTAINS_VAR_1 + 1
      StrCpy $STR_CONTAINS_VAR_3 $STR_HAYSTACK $STR_CONTAINS_VAR_2 $STR_CONTAINS_VAR_1
      StrCmp $STR_CONTAINS_VAR_3 $STR_NEEDLE found
      StrCmp $STR_CONTAINS_VAR_1 $STR_CONTAINS_VAR_4 done
      Goto loop
    found:
      StrCpy $STR_RETURN_VAR $STR_NEEDLE
      Goto done
    done:
   Pop $STR_NEEDLE ;Prevent "invalid opcode" errors and keep the
   Exch $STR_RETURN_VAR
FunctionEnd
!macroend

!insertmacro define_str_contains ""
!insertmacro define_str_contains "un."

!macro _StrContainsConstructor OUT NEEDLE HAYSTACK
  Push "${HAYSTACK}"
  Push "${NEEDLE}"
  Call StrContains
  Pop "${OUT}"
!macroend

!macro _UnStrContainsConstructor OUT NEEDLE HAYSTACK
  Push "${HAYSTACK}"
  Push "${NEEDLE}"
  Call un.StrContains
  Pop "${OUT}"
!macroend

!define StrContains '!insertmacro "_StrContainsConstructor"'
!define un.StrContains '!insertmacro "_UnStrContainsConstructor"'


; StrReplace
; Replaces all ocurrences of a given needle within a haystack with another string
; Written by dandaman32
;
; Example:
; ${StrReplace} $0 "just " "" "This is just an example"
; will result in $0 = "This is an example"

Var STR_REPLACE_VAR_0
Var STR_REPLACE_VAR_1
Var STR_REPLACE_VAR_2
Var STR_REPLACE_VAR_3
Var STR_REPLACE_VAR_4
Var STR_REPLACE_VAR_5
Var STR_REPLACE_VAR_6
Var STR_REPLACE_VAR_7
Var STR_REPLACE_VAR_8

Function StrReplace
  Exch $STR_REPLACE_VAR_2
  Exch 1
  Exch $STR_REPLACE_VAR_1
  Exch 2
  Exch $STR_REPLACE_VAR_0
    StrCpy $STR_REPLACE_VAR_3 -1
    StrLen $STR_REPLACE_VAR_4 $STR_REPLACE_VAR_1
    StrLen $STR_REPLACE_VAR_6 $STR_REPLACE_VAR_0
    loop:
      IntOp $STR_REPLACE_VAR_3 $STR_REPLACE_VAR_3 + 1
      StrCpy $STR_REPLACE_VAR_5 $STR_REPLACE_VAR_0 $STR_REPLACE_VAR_4 $STR_REPLACE_VAR_3
      StrCmp $STR_REPLACE_VAR_5 $STR_REPLACE_VAR_1 found
      StrCmp $STR_REPLACE_VAR_3 $STR_REPLACE_VAR_6 done
      Goto loop
    found:
      StrCpy $STR_REPLACE_VAR_5 $STR_REPLACE_VAR_0 $STR_REPLACE_VAR_3
      IntOp $STR_REPLACE_VAR_8 $STR_REPLACE_VAR_3 + $STR_REPLACE_VAR_4
      StrCpy $STR_REPLACE_VAR_7 $STR_REPLACE_VAR_0 "" $STR_REPLACE_VAR_8
      StrCpy $STR_REPLACE_VAR_0 $STR_REPLACE_VAR_5$STR_REPLACE_VAR_2$STR_REPLACE_VAR_7
      StrLen $STR_REPLACE_VAR_6 $STR_REPLACE_VAR_0
      Goto loop
    done:
  Pop $STR_REPLACE_VAR_1 ; Prevent "invalid opcode" errors and keep the
  Pop $STR_REPLACE_VAR_1 ; stack as it was before the function was called
  Exch $STR_REPLACE_VAR_0
FunctionEnd

!macro _strReplaceConstructor OUT NEEDLE NEEDLE2 HAYSTACK
  Push "${HAYSTACK}"
  Push "${NEEDLE}"
  Push "${NEEDLE2}"
  Call StrReplace
  Pop "${OUT}"
!macroend

!define StrReplace '!insertmacro "_strReplaceConstructor"'

Function IndexOf
Exch $R0
Exch
Exch $R1
Push $R2
Push $R3

 StrCpy $R3 $R0
 StrCpy $R0 -1
 IntOp $R0 $R0 + 1
  StrCpy $R2 $R3 1 $R0
  StrCmp $R2 "" +2
  StrCmp $R2 $R1 +2 -3

 StrCpy $R0 -1

Pop $R3
Pop $R2
Pop $R1
Exch $R0
FunctionEnd

Function WriteToFileFunc
 Exch $0 ;file to write to
 Exch
 Exch $1 ;text to write

  FileOpen $0 $0 a #open file
   FileSeek $0 0 END #go to end
   FileWrite $0 $1 #write to file
  FileClose $0

 Pop $1
 Pop $0
FunctionEnd

!macro IndexOf Var Str Char
Push "${Char}"
Push "${Str}"
 Call IndexOf
Pop "${Var}"
!macroend
!define IndexOf "!insertmacro IndexOf"