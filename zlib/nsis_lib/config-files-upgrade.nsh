; Description:
;
; Various macros and functions to use for moving and backing up
; iNTERFACEWARE product configuration files.
;
; Author: Kevin Senn

; Move file_name from dir to $INSTDIR\oldconfig
; If we do this at least once, we will warn the user that it is being
; done.
Var OLDCONFIGWARNING
!macro move_to_backup dir file_name
  
  IfFileExists ${dir}\${file_name} 0 done${dir}${file_name}
  StrCmp $OLDCONFIGWARNING "Done" skipWarning${dir}${file_name}
    MessageBox MB_OK "Old configuration files moved to $INSTDIR\oldconfig" /SD IDOK
    StrCpy $OLDCONFIGWARNING "Done"
  skipWarning${dir}${file_name}:
    CreateDirectory "$INSTDIR\oldconfig"
    Delete "$INSTDIR\oldconfig\${file_name}"
    Delete "$INSTDIR\oldconfig\${file_name}.old"
    nsExec::ExecToLog /TIMEOUT=2000 'cmd.exe /C move "${dir}\${file_name}" "$INSTDIR\oldconfig"'
  done${dir}${file_name}:
!macroend

; Replace an old config file (if there is already one).
; Takes the new one from $UNINSTDIR
; Arguments:
; file_name - the name of the file which we are replacing
!macro replace_config_file file_name
  IfFileExists "$INSTDIR\${file_name}" 0 skipBackup${file_name}
    ; if the file does exist in $INSTDIR already
    !insertmacro move_to_backup $INSTDIR ${file_name}
  skipBackup${file_name}:
  nsExec::ExecToLog /TIMEOUT=2000 'cmd.exe /C move "$UNINSTDIR\${file_name}" "$INSTDIR"'
!macroend

; Look for config files in dir.
; If none exist, we exit the macro normally.
; If some exist, but the user decides not to use them, we'll back them up
;   and exit the macro normally.
; If some exist, and the user wants to use them, we will goto jump_to_if_using_files
!macro look_for_config_files dir jump_to_if_using_files
  ; ************** logic to check port and log directory if user chooses to use old files
  ; ****** check port conflict ************************

  StrCpy $1 "$INSTDIR\IguanaConfigurationRepo\IguanaConfiguration.xml"  

  ifFileExists "$INSTDIR\IguanaConfiguration.xml" 0 +2
     StrCpy $1 "$INSTDIR\IguanaConfiguration.xml"
  
  ifFileExists "$INSTDIR\AdminConfiguration.xml" 0 +2
     StrCpy $1 "$INSTDIR\AdminConfiguration.xml"
  
  !insertmacro get_old_config_values "$1" ${jump_to_if_using_files}
  pop $4
  pop $3

  StrCmp $3 "" 0 +2
     StrCpy $3 "6543"
  StrCmp $PARAMETER_PORT "" 0 +2
     StrCpy $PARAMETER_PORT $3
     
  StrCmp $4 "" 0 +2
     StrCpy $4 "logs\"
  
  StrCmp $PARAMETER_LOG "" 0 +2
     StrCpy $PARAMETER_LOG $4
  
  !insertmacro if_config_files_exist "${dir}" configFilesIn${dir}
  Goto done${dir}

  configFilesIn${dir}:
    MessageBox MB_YESNO "Iguana configuration files already exist in ${dir}.$\r$\n\
    Use these files?" /SD IDYES IDNO moveFrom${dir}ToBackup

    Push $4 ; Log Directory
    Push $3 ; Port
    Push $INSTDIR
    Call VerifyPortAndLogConflict
    Pop $8 ; return 1 = conflict 0 = port and logdirectory good
  
    StrCmp $8 "1" reconfigureConfigurationFile${dir}
    ; *********************
    goto configurationOk${dir}
    
  reconfigureConfigurationFile${dir}:
    StrCpy $ForceEditConfigurations "1" ; Global status variable to let configuration page know
    ; TODO - Here we want to try changing the file instead.
    ; ABCD
    ; Goto moveFrom${dir}ToBackup
    
  configurationOk${dir}:
    ;StrCpy $UNINSTDIR ${dir}
    Goto ${jump_to_if_using_files}
  
  moveFrom${dir}ToBackup:
    Push ${dir}
    Call MoveAllToBackup

  done${dir}:

!macroend


Function BackupRepos
  Pop $0
  IfFileExists "$0\IguanaConfigurationRepo\*.*" config_repo_exists config_repo_end
  config_repo_exists:
    ; Backup config repo
    RMDir /r "$0\oldconfig\IguanaConfigurationRepo"
    CreateDirectory "$0\oldconfig\IguanaConfigurationRepo"
    CopyFiles "$0\IguanaConfigurationRepo\*" "$0\oldconfig\IguanaConfigurationRepo"
    RMDir /r "$0\IguanaConfigurationRepo"
  config_repo_end:
  
  IfFileExists "$0\IguanaMainRepo\*.*" main_repo_exists main_repo_end
  main_repo_exists:
    ; Backup main repo
    RMDir /r "$0\oldconfig\IguanaMainRepo"
    CreateDirectory "$0\oldconfig\IguanaMainRepo"
    CopyFiles "$0\IguanaMainRepo\*" "$0\oldconfig\IguanaMainRepo"
    RMDir /r "$0\IguanaMainRepo"
  main_repo_end:


FunctionEnd

; Backs up the 'data' and 'edit' directories into the directory
; on the top of the stack.
; Beware - this function is not Iguana-independent, as other
; functions in this file are.
Function BackupDirectories
  Pop $0
  IfFileExists "$0\data\*.*" data_dir_exists data_dir_end
  data_dir_exists:
    ; Backup data directory
    RMDir /r "$0\oldconfig\data"
    CreateDirectory "$0\oldconfig\data"
    CopyFiles "$0\data\*" "$0\oldconfig\data"
    RMDir /r "$0\data"
  data_dir_end:

  IfFileExists "$0\edit\*.*" edit_dir_exists edit_dir_end
  edit_dir_exists:
    ; Backup edit directory
    RMDir /r "$0\oldconfig\edit"
    CreateDirectory "$0\oldconfig\edit"
    CopyFiles "$0\edit\*" "$0\oldconfig\edit"
    RMDir /r "$0\edit"
  edit_dir_end:

  IfFileExists "$0\run\*.*" run_dir_exists run_dir_end
  run_dir_exists:
    ; Backup data directory
    RMDir /r "$0\oldconfig\run"
    CreateDirectory "$0\oldconfig\run"
    CopyFiles "$0\run\*" "$0\oldconfig\run"
    RMDir /r "$0\run"
  run_dir_end:
FunctionEnd

; Moves the config repositories from the directory
; on top of the stack to $INSTDIR.
; Beware - this function is not Iguana-independent, as other
; functions in this file are.
Function RestoreRepos
  ; First, backup any files already in $INSTDIR
  Push $INSTDIR
  Call BackupRepos
  
  Pop $0 ; The "from" directory
  IfFileExists "$0\IguanaMainRepo\*.*" main_repo_exists main_repo_end
  main_repo_exists:
    CreateDirectory "$INSTDIR\IguanaMainRepo"
    CopyFiles "$0\IguanaMainRepo\*" "$INSTDIR\IguanaMainRepo"
    RMDir /r "$0\IguanaMainRepo"
  main_repo_end:

  IfFileExists "$0\IguanaConfigurationRepo\*.*" config_repo_exists config_repo_end
  config_repo_exists:
    CreateDirectory "$INSTDIR\IguanaConfigurationRepo"
    CopyFiles "$0\IguanaConfigurationRepo\*" "$INSTDIR\IguanaConfigurationRepo"
    RMDir /r "$0\IguanaConfigurationRepo"
  config_repo_end:
FunctionEnd

; Move all config and log files from their original the directory
; on top of the stack, to a backup subdirectory.
Function MoveAllToBackup
  Pop $0 ; the directory
  !insertmacro on_all_config_and_log_files "!insertmacro move_to_backup $0"
  
  Push $0
  Call BackupRepos
  
  Push $0
  Call BackupDirectories

FunctionEnd

; Move config files (backing up ones that are in the way, if neccessary)
; from $UNINSTDIR to $INSTDIR
!macro move_config_files
  !insertmacro on_all_config_and_log_files "!insertmacro replace_config_file"
  
  Push $UNINSTDIR
  Call RestoreRepos
!macroend

