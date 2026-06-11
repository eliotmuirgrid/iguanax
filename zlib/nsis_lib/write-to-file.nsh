!macro WriteToFile String File
   FileOpen $0 ${File} a ;open file
   FileSeek $0 0 END ;go to end
   FileWrite $0 "${String}" ;write to file
   FileClose $0
!macroend
!define WriteToFile "!insertmacro WriteToFile"
