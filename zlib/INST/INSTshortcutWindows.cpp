// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTshortcut
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday 11 July 2023 - 08:31AM
// ---------------------------------------------------------------------------

#include <INST/INSTshortcut.h>

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h> // Include the necessary header for IShellLink

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// The COM library only needs to be initialized once. We are Using a class to automatically
// un-initialize it when program exits. Can we do it without using class?
class INSTcomLibrary {
public:
   INSTcomLibrary() {
      if (S_OK != CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)) {
         COL_ERR("Failed to initialize the COM library.");
      }
   }
   ~INSTcomLibrary() { CoUninitialize(); }
};
static COLauto<INSTcomLibrary> g_pLib;
static void INSTinitComLibrary() {
   COL_FUNCTION(INSTinitComLibrary);
   if (g_pLib) { return; }    // already initialized
   g_pLib = new INSTcomLibrary();
}

static bool INSTsaveLink(IShellLink* pLink, const COLstring& SavePath) {
   COL_FUNCTION(INSTsaveLink);
   // Query IShellLink for the IPersistFile interface, used for saving the shortcut in persistent storage.
   IPersistFile* pLinkFile = NULL;
   HRESULT Result = pLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pLinkFile);
   if (FAILED(Result)) {
      COL_ERR("Failed to query the 'IShellLink' interface to save the shortcut.");
      return false;
   }
   // Save the link by calling IPersistFile::Save.
   COLstring WideCharSavePath;
   wchar_t* pWideCharSavePath = FILmangleFileNameForWindows(SavePath, &WideCharSavePath);
   Result = pLinkFile->Save(pWideCharSavePath, TRUE);
   pLinkFile->Release();
   return true;
}

bool INSTcreateLink(const COLstring& ExePath, const COLstring& SavePath, const COLstring& Arguments = "",
   const COLstring& WorkingDir = "", const COLstring& Description = "", int DisplayOption = SW_SHOWMINNOACTIVE) {
   COL_FUNCTION(INSTcreateLink);
   INSTinitComLibrary();
   IShellLink* pLink = NULL;
   // Get a pointer to the IShellLink interface
   HRESULT Result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pLink);
   if (FAILED(Result)) {
      COL_ERR("Failed to instantiate an 'IShellLink' object.");
      return false;
   }

   // Set the path to the shortcut target
   COLstring ExeAbsolutePath = FILpathResolve(ExePath);
   COL_VAR(ExeAbsolutePath);
   pLink->SetPath(ExeAbsolutePath.c_str());
   // Set the arguments for the target
   if (!Arguments.is_null()) {
      COL_VAR(Arguments);
      pLink->SetArguments(Arguments.c_str());
   }
   // Set the working directory
   COLstring WorkDirPath = FILparentDir(ExeAbsolutePath);
   if (!WorkingDir.is_null()) { WorkDirPath = FILpathResolve(WorkDirPath); }
   COL_VAR(WorkDirPath);
   pLink->SetWorkingDirectory(WorkDirPath.c_str());
   // Set the description
   if (!Description.is_null()) {
      COL_VAR(Description);
      pLink->SetDescription(Description.c_str());
   }
   // Set the display option
   pLink->SetShowCmd(DisplayOption);
   // Set the icon
   pLink->SetIconLocation(ExeAbsolutePath.c_str(), 0);

   bool Success = INSTsaveLink(pLink, SavePath);
   pLink->Release();
   return Success;
}


static bool INSTgetUserDesktopPath(COLstring* pPath) {
   COL_FUNCTION(INSTappendPathW);
   wchar_t* DesktopPath = nullptr;
   HRESULT result = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &DesktopPath);
   if (FAILED(result)) {
      COL_ERR("Failed to get current user's desktop directory path.");
      return false;
   }
   COLstring Path(DesktopPath);
   CoTaskMemFree(DesktopPath);
   Path.swap(pPath);
   return true;
}

static void INSTremoveOldDesktopShortcut(const COLstring& UserDesktopPath) {
   COL_FUNCTION(INSTremoveOldDesktopShortcut);
   COLstring OldPath(FILpathAppend(UserDesktopPath, "Iguana X.lnk"));
   if (FILfileExists(OldPath)) { FILremove(OldPath); }
}

static bool INSTgetShortcutSavePath(COLstring* pPath) {
   COL_FUNCTION(INSTgetIguanaShortcutPath);
   if (!INSTgetUserDesktopPath(pPath)) { return false; }
   COLstring& Path = *pPath;
   INSTremoveOldDesktopShortcut(Path);
   Path = FILpathAppend(Path, "IguanaX.lnk");
   COL_VAR(Path);
   return true;
}

bool INSTcreateDesktopShortcut(const COLstring& ExePath) {
   COL_FUNCTION(INSTcreateDesktopShortcut);
   COLstring ShortcutPath;
   if (!INSTgetShortcutSavePath(&ShortcutPath)) {
      COL_ERR("Failed to get shortcut save path");
      return false;
   }
   INSTcreateLink(ExePath, ShortcutPath, "--open_gui", "", "Open IguanaX dashboard");
   return true;
}
