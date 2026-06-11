; Description:
;
; Contains script for uninstalling old WISE versions of Chameleon
; and Iguana.  This is called by inserting the macro WISE_UNINSTALL.
;
; Author: Kevin Senn

; Calling this macro will uninstall Chameleon and/or Iguana from
; a directory specified by $R1.  Attempting to delete a file
; that doesn't exist does nothing, so we just delete all Iguana
; and Chameleon files.  Attempting to remove a directory that
; still has contents does nothing, so if users have added their
; own files, they will be safe.

!macro WISE_UNINSTALL
  Call wiseUninstall
!macroend

; Input:
; $R1 is the path of the main iNTERFACEWARE application directory
; $R3 is the name of the product found (either "iNTERFACEWARE Chameleon"
; or "iNTERFACEWARE Iguana")
Function wiseUninstall

  ; Stop Iguana service, if it's running
  services::SendServiceCommand 'stop' 'Iguana'
  services::SendServiceCommand 'stop' 'Iguana Monitor'

  ; delete the Iguana service as well
  nsExec::ExecToLog /TIMEOUT=5000 "$R1\iguana_monitor --uninstall"
  nsExec::ExecToLog /TIMEOUT=5000 "$R1\iguana --uninstall"

  ; Make sure Iguana is no longer running (if it was never running in the first
  ; place this will work too)
  services::IsServiceRunning 'Iguana'
  ; Value is returned on the stack
  Pop $0
  ; If it is not running, go ahead and delete files.
  StrCmp $0 "Yes" couldNotStopIguana
  
  ; Make sure iguana_monitor is no longer running
  services::IsServiceRunning 'Iguana Monitor'
  Pop $0
  StrCmp $0 "Yes" couldNotStopIguana deleteFiles

  couldNotStopIguana:
    MessageBox MB_OK "Could not remove the iNTERFACEWARE Iguana service.  Remove it manually (Control Panel > Administrative Tools > Services), or try running setup again." /SD IDOK
    Call QuitInstaller
    Goto done

  deleteFiles:

  ; Main Directory

  ; First we will TRY to delete CHM_LIB3.dll.  If this doesn't work,
  ; then the user is more than likely running an iNTERFACEWARE
  ; application as we speak!  We'll have to tell them to knock it off
  ; before we can continue.  If we were to just check which processes
  ; were running, we'd have to check a few, since there is more than
  ; one Chameleon executable.  They all, however, use the CHM_LIB3.dll
  ; library, so if any iNTERFACEWARE application is running, we won't
  ; be able to delete CHM_LIB3.dll.
  UnRegDLL $R1\CHM_LIB3.dll
  !insertmacro UnInstallLib REGDLL NOTSHARED NOREBOOT_NOTPROTECTED $R1\CHM_LIB3.dll
  IfFileExists $R1\CHM_LIB3.dll applicationRunning applicationNotRunning

  applicationRunning:
    MessageBox MB_OK "Setup cannot uninstall $R3.  Please close all iNTERFACEWARE applications and run setup again." /SD IDOK
    Call QuitInstaller

  applicationNotRunning: ; continue with uninstallation

  Delete /REBOOTOK $R1\win95detect.dll
  Delete /REBOOTOK $R1\hl7diff.css
  Delete /REBOOTOK $R1\msgdiff.exe
  Delete /REBOOTOK $R1\msgxml.exe
  Delete /REBOOTOK $R1\msgtransform.exe
  Delete /REBOOTOK $R1\x12diff.vmd
  Delete /REBOOTOK $R1\CS_GEN.DLL
  Delete /REBOOTOK $R1\GACUTIL.EXE
  Delete /REBOOTOK $R1\Listener.exe
  Delete /REBOOTOK $R1\Simulator.exe
  Delete /REBOOTOK $R1\messages.txt
  Delete /REBOOTOK $R1\manual.chm
  Delete /REBOOTOK $R1\anchor.ini
  Delete /REBOOTOK $R1\expat.dll
  Delete /REBOOTOK $R1\expat.ilk
  Delete /REBOOTOK $R1\bzip2.dll
  Delete /REBOOTOK $R1\python22.dll
  Delete /REBOOTOK $R1\msvcrt.dll
  Delete /REBOOTOK $R1\msvcp60.dll
  Delete /REBOOTOK $R1\vcl40.bpl
  Delete /REBOOTOK $R1\vclx40.bpl
  Delete /REBOOTOK $R1\CHM_Browser.dll
  Delete /REBOOTOK $R1\Listener.vmd
  Delete /REBOOTOK $R1\VMstudio.exe
  Delete /REBOOTOK $R1\NETDLL.dll
  Delete /REBOOTOK $R1\NETDLL2.dll
  Delete /REBOOTOK $R1\CHM_VBbridge.dll
  Delete /REBOOTOK $R1\chm_java.dll
  Delete /REBOOTOK $R1\conspawn.exe
  Delete /REBOOTOK $R1\qry_transport.dll
  Delete /REBOOTOK $R1\browser.ico
  Delete /REBOOTOK $R1\messages.txt
  Delete /REBOOTOK $R1\Simulator.exe
  Delete /REBOOTOK $R1\Listener.exe
  Delete /REBOOTOK $R1\GACUTIL.EXE
  Delete /REBOOTOK $R1\CS_GEN.DLL
  Delete /REBOOTOK $R1\x12diff.vmd
  Delete /REBOOTOK $R1\msgtransform.exe
  Delete /REBOOTOK $R1\msgxml.exe
  Delete /REBOOTOK $R1\msgdiff.exe
  Delete /REBOOTOK $R1\hl7diff.css
  Delete /REBOOTOK $R1\win95detect.dll
  Delete /REBOOTOK $R1\hl7-listener.ico
  Delete /REBOOTOK $R1\hl7-simulator.ico
  Delete /REBOOTOK $R1\vmd.ico
  Delete /REBOOTOK $R1\mainicon.ico
  Delete /REBOOTOK $R1\wizard_logo.jpg
  Delete /REBOOTOK $R1\config.tbn
  Delete /REBOOTOK $R1\iguana_monitor.exe
  Delete /REBOOTOK $R1\iguana.exe
  Delete /REBOOTOK $R1\iguana.pdb
  Delete /REBOOTOK $R1\autoack.vmd
  Delete /REBOOTOK $R1\autonack.vmd
  Delete /REBOOTOK $R1\ack_verify.vmd
  Delete /REBOOTOK $R1\iguana.chm
  Delete /REBOOTOK $R1\Iguana.url
  Delete /REBOOTOK $R1\IGC.dll
  Delete /REBOOTOK $R1\GACUTIL.EXE
  Delete /REBOOTOK $R1\UNWISE.EXE
  Delete /REBOOTOK $R1\INSTALL.LOG
  Delete /REBOOTOK $R1\ServiceErrorLog.txt
  Delete $R1\spencer.txt


  ; CHM

  Delete /REBOOTOK $R1\CHM\NETDLL.def
  Delete /REBOOTOK $R1\CHM\CHMexport.def
  Delete /REBOOTOK $R1\CHM\CHMuserQueueDll.def
  Delete /REBOOTOK $R1\CHM\VMlibrary.def
  Delete /REBOOTOK $R1\CHM\VMlibrarySR.def
  Delete /REBOOTOK $R1\CHM\CHM.dsp
  Delete /REBOOTOK $R1\CHM\CHM_QUEUE.dsp
  Delete /REBOOTOK $R1\CHM\CHM_TCPIP.dsp
  Delete /REBOOTOK $R1\CHM\CHM_TCPIP2.dsp
  Delete /REBOOTOK $R1\CHM\CHMnet.dsp
  Delete /REBOOTOK $R1\CHM\CHM.vcproj
  Delete /REBOOTOK $R1\CHM\CHM_QUEUE.vcproj
  Delete /REBOOTOK $R1\CHM\CHM_TCPIP.vcproj
  Delete /REBOOTOK $R1\CHM\CHM_TCPIP2.dsp
  Delete /REBOOTOK $R1\CHM\CHM_TCPIP2.vcproj
  Delete /REBOOTOK $R1\CHM\CHM_TCPIP2_VC2003.vcproj
  Delete /REBOOTOK $R1\CHM\CHM_VC2003.vcproj
  Delete /REBOOTOK $R1\CHM\CHMnet.vcproj
  Delete /REBOOTOK $R1\CHM\CHMnet_2003.vcproj
  Delete /REBOOTOK $R1\CHM\CHMhandle.h
  Delete /REBOOTOK $R1\CHM\CHMconnectionDll.h
  Delete /REBOOTOK $R1\CHM\CHMlistenerDll.h
  Delete /REBOOTOK $R1\CHM\CHMclientDll.h
  Delete /REBOOTOK $R1\CHM\CHMexceptionDll.h
  Delete /REBOOTOK $R1\CHM\CHMllpConnection.h
  Delete /REBOOTOK $R1\CHM\CHMllpServer.h
  Delete /REBOOTOK $R1\CHM\CHMllpClient.h
  Delete /REBOOTOK $R1\CHM\CHMexception.h
  Delete /REBOOTOK $R1\CHM\CHMbuffer.h
  Delete /REBOOTOK $R1\CHM\CHMcallbackFunction.h
  Delete /REBOOTOK $R1\CHM\CHMcallbackOutputLog.h
  Delete /REBOOTOK $R1\CHM\CHMcallbackTransport.h
  Delete /REBOOTOK $R1\CHM\CHMcompositeDll.h
  Delete /REBOOTOK $R1\CHM\CHMconfigDll.h
  Delete /REBOOTOK $R1\CHM\CHMconnectionInfo.h
  Delete /REBOOTOK $R1\CHM\CHMcoutOutputLog.h
  Delete /REBOOTOK $R1\CHM\CHMdataType.h
  Delete /REBOOTOK $R1\CHM\CHMdataTypeValue.h
  Delete /REBOOTOK $R1\CHM\CHMdateTime.h
  Delete /REBOOTOK $R1\CHM\CHMdateTimeClass.h
  Delete /REBOOTOK $R1\CHM\CHMdateTimeGrammarDll.h
  Delete /REBOOTOK $R1\CHM\CHMeditOutputLog.h
  Delete /REBOOTOK $R1\CHM\CHMengine.h
  Delete /REBOOTOK $R1\CHM\CHMengineClass.h
  Delete /REBOOTOK $R1\CHM\CHMengineQueueHandler.h
  Delete /REBOOTOK $R1\CHM\CHMerror.h
  Delete /REBOOTOK $R1\CHM\CHMerrorClass.h
  Delete /REBOOTOK $R1\CHM\CHMglobalDefines.h
  Delete /REBOOTOK $R1\CHM\CHMhl7ClientSocket.h
  Delete /REBOOTOK $R1\CHM\CHMhl7ServerSocket.h
  Delete /REBOOTOK $R1\CHM\CHMinstanceHeap.h
  Delete /REBOOTOK $R1\CHM\CHMiomanip.h
  Delete /REBOOTOK $R1\CHM\CHMllpWrapper.h
  Delete /REBOOTOK $R1\CHM\CHMmessage.h
  Delete /REBOOTOK $R1\CHM\CHMmessageClass.h
  Delete /REBOOTOK $R1\CHM\CHMmessageDef.h
  Delete /REBOOTOK $R1\CHM\CHMmessageDefClass.h
  Delete /REBOOTOK $R1\CHM\CHMmessageDll.h
  Delete /REBOOTOK $R1\CHM\CHMmfcSocketTransport.h
  Delete /REBOOTOK $R1\CHM\CHMminCInclude.h
  Delete /REBOOTOK $R1\CHM\CHMminimumInclude.h
  Delete /REBOOTOK $R1\CHM\CHMnullWrapper.h
  Delete /REBOOTOK $R1\CHM\CHMostreamOperator.h
  Delete /REBOOTOK $R1\CHM\CHMostreamOperatorStd.h
  Delete /REBOOTOK $R1\CHM\CHMoutputLog.h
  Delete /REBOOTOK $R1\CHM\CHMparseError.h
  Delete /REBOOTOK $R1\CHM\CHMparseErrorEnum.h
  Delete /REBOOTOK $R1\CHM\CHMpreCompHead.h
  Delete /REBOOTOK $R1\CHM\CHMqueueHandler.h
  Delete /REBOOTOK $R1\CHM\CHMqueueLog.h
  Delete /REBOOTOK $R1\CHM\CHMqueuePrecomp.h
  Delete /REBOOTOK $R1\CHM\CHMqueueTransport.h
  Delete /REBOOTOK $R1\CHM\CHMrefHashTable.h
  Delete /REBOOTOK $R1\CHM\CHMreference.h
  Delete /REBOOTOK $R1\CHM\CHMsegmentDll.h
  Delete /REBOOTOK $R1\CHM\CHMserviceParameters.h
  Delete /REBOOTOK $R1\CHM\CHMstring.h
  Delete /REBOOTOK $R1\CHM\CHMtable.h
  Delete /REBOOTOK $R1\CHM\CHMtableClass.h
  Delete /REBOOTOK $R1\CHM\CHMtableDef.h
  Delete /REBOOTOK $R1\CHM\CHMtableDefClass.h
  Delete /REBOOTOK $R1\CHM\CHMtableGrammar.h
  Delete /REBOOTOK $R1\CHM\CHMtableGrammarClass.h
  Delete /REBOOTOK $R1\CHM\CHMtableRow.h
  Delete /REBOOTOK $R1\CHM\CHMtcpipPrecond.h
  Delete /REBOOTOK $R1\CHM\CHMtransport.h
  Delete /REBOOTOK $R1\CHM\CHMunicode.h
  Delete /REBOOTOK $R1\CHM\CHMutil.h
  Delete /REBOOTOK $R1\CHM\CHMvector.h
  Delete /REBOOTOK $R1\CHM\CHMwrapper.h
  Delete /REBOOTOK $R1\CHM\DLLuntypedMessageTree.h
  Delete /REBOOTOK $R1\CHM\NETdll.h
  Delete /REBOOTOK $R1\CHM\NETDLL.def
  Delete /REBOOTOK $R1\CHM\CHM_BCB3.cpp
  Delete /REBOOTOK $R1\CHM\CHM_BCB4.cpp
  Delete /REBOOTOK $R1\CHM\CHM_BCB5.cpp
  Delete /REBOOTOK $R1\CHM\CHMllpConnection.cpp
  Delete /REBOOTOK $R1\CHM\CHMllpServer.cpp
  Delete /REBOOTOK $R1\CHM\CHMllpClient.cpp
  Delete /REBOOTOK $R1\CHM\CHMexception.cpp
  Delete /REBOOTOK $R1\CHM\CHMbuffer.cpp
  Delete /REBOOTOK $R1\CHM\CHMcallbackOutputLog.cpp
  Delete /REBOOTOK $R1\CHM\CHMcallbackTransport.cpp
  Delete /REBOOTOK $R1\CHM\CHMcoutOutputLog.cpp
  Delete /REBOOTOK $R1\CHM\CHMdateTimeClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMeditOutputLog.cpp
  Delete /REBOOTOK $R1\CHM\CHMengineClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMengineQueueHandler.cpp
  Delete /REBOOTOK $R1\CHM\CHMerrorClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMhl7ClientSocket.cpp
  Delete /REBOOTOK $R1\CHM\CHMhl7ServerSocket.cpp
  Delete /REBOOTOK $R1\CHM\CHMinstanceHeap.cpp
  Delete /REBOOTOK $R1\CHM\CHMllpWrapper.cpp
  Delete /REBOOTOK $R1\CHM\CHMmessageClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMmessageDefClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMmfcSocketTransport.cpp
  Delete /REBOOTOK $R1\CHM\CHMnullWrapper.cpp
  Delete /REBOOTOK $R1\CHM\CHMoutputLog.cpp
  Delete /REBOOTOK $R1\CHM\CHMpreCompHead.cpp
  Delete /REBOOTOK $R1\CHM\CHMqueueHandler.cpp
  Delete /REBOOTOK $R1\CHM\CHMqueueLog.cpp
  Delete /REBOOTOK $R1\CHM\CHMqueuePrecomp.cpp
  Delete /REBOOTOK $R1\CHM\CHMqueueTransport.cpp
  Delete /REBOOTOK $R1\CHM\CHMrefHashTable.cpp
  Delete /REBOOTOK $R1\CHM\CHMstring.cpp
  Delete /REBOOTOK $R1\CHM\CHMtableClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMtableDefClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMtableGrammarClass.cpp
  Delete /REBOOTOK $R1\CHM\CHMtableRow.cpp
  Delete /REBOOTOK $R1\CHM\CHMtcpipPrecond.cpp
  Delete /REBOOTOK $R1\CHM\CHMtransport.cpp
  Delete /REBOOTOK $R1\CHM\CHMutil.cpp
  Delete /REBOOTOK $R1\CHM\CHMwrapper.cpp
  Delete /REBOOTOK $R1\CHM\CHM_BCB3.bpr
  Delete /REBOOTOK $R1\CHM\CHM_BCB4.bpr
  Delete /REBOOTOK $R1\CHM\CHM_BCB5.bpr
  RMDir $R1\CHM

  
  ; Client

  Delete /REBOOTOK $R1\client\csharp\example\AssemblyInfo.cs
  Delete /REBOOTOK $R1\client\csharp\example\IGCexample.csproj
  Delete /REBOOTOK $R1\client\csharp\example\IGCexample.csproj.user
  Delete /REBOOTOK $R1\client\csharp\example\IGCexample.sln
  Delete /REBOOTOK $R1\client\csharp\example\IGCexampleMain.cs
  RMDir $R1\client\csharp\example

  Delete /REBOOTOK $R1\client\csharp\IGC_csharp.dll
  RMDir $R1\client\csharp

  RMDir $R1\client


  ; Csharp

  Delete /REBOOTOK $R1\csharp\CHM_CSHARP.DLL
  Delete /REBOOTOK $R1\csharp\ToolBoxControl.exe
  RMDir $R1\csharp


  ; DelphiPackages

  Delete /REBOOTOK $R1\DelphiPackages\CHM.dcr
  Delete /REBOOTOK $R1\DelphiPackages\CHMnet.dcr 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHM.pas 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMnet.pas 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMinterface.pas 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcoreBcbPackage6.bpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcoreBcbPackage6.res 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage3.res 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage4.res 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage5.res 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage6.res 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage7.res 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage3.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage4.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage5.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage6.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcorePackage7.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMnetPackage3.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMnetPackage4.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMnetPackage5.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMnetPackage6.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMnetPackage7.dpk 	 
  Delete /REBOOTOK $R1\DelphiPackages\CHMcoreBcbPackage6.cpp 	 
  Delete /REBOOTOK $R1\DelphiPackages\VMlibraryBCB.def 	 
  RMDir $R1\DelphiPackages


  ; Java

  Delete /REBOOTOK $R1\java\classes.jar
  RMDir $R1\java


  ; Lib

  RMDir /r $R1\lib\xml\sax\CVS

  Delete /REBOOTOK $R1\lib\xml\sax\__init__.py
  Delete /REBOOTOK $R1\lib\xml\sax\_exceptions.py
  Delete /REBOOTOK $R1\lib\xml\sax\expatreader.py
  Delete /REBOOTOK $R1\lib\xml\sax\handler.py
  Delete /REBOOTOK $R1\lib\xml\sax\saxutils.py
  Delete /REBOOTOK $R1\lib\xml\sax\xmlreader.py
  Delete /REBOOTOK $R1\lib\xml\sax\*.pyc
  RMDir $R1\lib\xml\sax

  RMDir /r $R1\lib\xml\parsers\CVS

  Delete /REBOOTOK $R1\lib\xml\parsers\__init__.py
  Delete /REBOOTOK $R1\lib\xml\parsers\expat.py
  Delete /REBOOTOK $R1\lib\xml\parsers\*.pyc
  RMDir $R1\lib\xml\parsers

  RMDir /r $R1\lib\xml\dom\CVS

  Delete /REBOOTOK $R1\lib\xml\dom\__init__.py
  Delete /REBOOTOK $R1\lib\xml\dom\domreg.py
  Delete /REBOOTOK $R1\lib\xml\dom\minidom.py
  Delete /REBOOTOK $R1\lib\xml\dom\pulldom.py
  Delete /REBOOTOK $R1\lib\xml\dom\*.pyc
  RMDir $R1\lib\xml\dom

  RMDir /r $R1\lib\xml\CVS

  Delete /REBOOTOK $R1\lib\xml\__init__.py
  Delete /REBOOTOK $R1\lib\xml\*.pyc
  RMDir $R1\lib\xml

  RMDir /r $R1\lib\CVS

  Delete /REBOOTOK $R1\lib\StringIO.py
  Delete /REBOOTOK $R1\lib\UserDict.py
  Delete /REBOOTOK $R1\lib\__future__.py
  Delete /REBOOTOK $R1\lib\copy_reg.py
  Delete /REBOOTOK $R1\lib\linecache.py
  Delete /REBOOTOK $R1\lib\ntpath.py
  Delete /REBOOTOK $R1\lib\os.py
  Delete /REBOOTOK $R1\lib\posixpath.py
  Delete /REBOOTOK $R1\lib\pre.py
  Delete /REBOOTOK $R1\lib\random.py
  Delete /REBOOTOK $R1\lib\re.py
  Delete /REBOOTOK $R1\lib\site.py
  Delete /REBOOTOK $R1\lib\sre.py
  Delete /REBOOTOK $R1\lib\sre_compile.py
  Delete /REBOOTOK $R1\lib\sre_constants.py
  Delete /REBOOTOK $R1\lib\sre_parse.py
  Delete /REBOOTOK $R1\lib\stat.py
  Delete /REBOOTOK $R1\lib\string.py
  Delete /REBOOTOK $R1\lib\traceback.py
  Delete /REBOOTOK $R1\lib\types.py
  Delete /REBOOTOK $R1\lib\warnings.py
  Delete /REBOOTOK $R1\lib\*.pyc
  RMDir $R1\lib


  ; Libraries
  ; Need to have both sets of libraries (with and without the underscore)
  ; to make sure they're all cleaned out -- as noted above trying to delete
  ; a file that doesn't exist doesn't do anything

  Delete /REBOOTOK "$R1\Libraries\HL7 2.1.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7 2.2.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7 2.3.1.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7 2.3.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7 2.4.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7 2.5.vmd" 
  Delete /REBOOTOK "$R1\Libraries\HL7_2.1.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7_2.2.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7_2.3.1.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7_2.3.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7_2.4.vmd"
  Delete /REBOOTOK "$R1\Libraries\HL7_2.5.vmd"
  RMDir $R1\Libraries

  
  ; Plugins

  Delete /REBOOTOK $R1\Plugins\CHMvmdFileTreXml.dll
  RMDir $R1\Plugins


  ; Projects

  RMDir $R1\Projects


  ; Reports

  Delete /REBOOTOK $R1\Reports\html\chmIcon.gif
  Delete /REBOOTOK $R1\Reports\html\compositeIcon.gif
  Delete /REBOOTOK $R1\Reports\html\compositescrollicon.gif
  Delete /REBOOTOK $R1\Reports\html\datetimeIcon.gif
  Delete /REBOOTOK $R1\Reports\html\datetimescrollicon.gif
  Delete /REBOOTOK $R1\Reports\html\dividerIcon.gif
  Delete /REBOOTOK $R1\Reports\html\homeicon.gif
  Delete /REBOOTOK $R1\Reports\html\messageicon.gif
  Delete /REBOOTOK $R1\Reports\html\messagescrollicon.gif
  Delete /REBOOTOK $R1\Reports\html\segmentIcon.gif
  Delete /REBOOTOK $R1\Reports\html\segmentscrollicon.gif
  Delete /REBOOTOK $R1\Reports\html\tableIcon.gif
  Delete /REBOOTOK $R1\Reports\html\tableScrollicon.gif
  Delete /REBOOTOK $R1\Reports\html\uparrowIcon.gif
  RMDir $R1\Reports\html
  RMDir $R1\Reports


  ; Server_docs

  Delete /REBOOTOK $R1\server_docs\CVS\Root
  Delete /REBOOTOK $R1\server_docs\CVS\Repository
  Delete /REBOOTOK $R1\server_docs\CVS\Entries
  RMDir /r $R1\server_docs\CVS

  Delete /REBOOTOK $R1\server_docs\404.html
  Delete /REBOOTOK $R1\server_docs\exception.html
  Delete /REBOOTOK $R1\server_docs\exception_info.html
  RMDir $R1\server_docs

  ; Web_docs

  RMDir /r $R1\web_docs


  ; Wizard

  RMDir /r $R1\wizard


  ; Wizard Templates

  RMDir /r $R1\wizard_templates


  ; Remove the whole directory (won't be done if anything is left
  ; behind)  
  RMDir $R1


  ; Start Menu and Desktop shortcuts

  ; The iNTERFACEWARE start menu folder is in All Users' start menu,
  ; so we need to specify this.
  SetShellVarContext all

  RMDir /r $STARTMENU\Programs\iNTERFACEWARE
  ; Iguana shortcut is stored in AllUsers desktop (NSIS default) while
  ; Chameleon shortcut is stored on the current user's desktop
  Delete /REBOOTOK "$DESKTOP\Iguana.lnk"
  Delete /REBOOTOK "$DESKTOP\iNTERFACEWARE Iguana.lnk"
  SetShellVarContext current
  Delete /REBOOTOK "$DESKTOP\Chameleon.lnk"
  Delete /REBOOTOK "$DESKTOP\iNTERFACEWARE Chameleon.lnk"
  SetShellVarContext all ; change back to default
  Delete /REBOOTOK "$DESKTOP\Chameleon.lnk"
  Delete /REBOOTOK "$DESKTOP\iNTERFACEWARE Chameleon.lnk"
  
  ; PATH environment variable - remove all iNTERFACEWARE stuff
  ; This functionality is built into install_shot.exe.
  nsExec::ExecToLog /TIMEOUT=3000 '"$TEMP\install_shot" --cleanPATH "$R1"'


  ; Registry

  DeleteRegKey HKCU 'iNTERFACEWARE' ""
  ; #4136 commented out below line since I think it causes the recent file list to be removed - Eliot
  ;DeleteRegKey HKCU 'Software\iNTERFACEWARE' ""
  
  ; This is the "add/remove programs" section
  DeleteRegKey HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\$R3'

  ; This is the filetype associations section
  DeleteRegKey HKLM 'Software\Classes\.vmd'
  DeleteRegKey HKLM 'Software\Classes\iNTERFACEWARE Chameleon.Document'

done:
FunctionEnd

