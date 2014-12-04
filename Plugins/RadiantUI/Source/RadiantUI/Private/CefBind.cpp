// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include <windows.h>

namespace
{
#if defined(WIN32)
#pragma warning (disable:4191)
#if RADIANTUI_DEBUG // from RadiantUI.Build.cs
#define DLLNAME "CefFramework-Debug.dll"
#define MODULENAME "UE4Editor-RadiantUI-Win64-DebugGame.dll"
#else
#define DLLNAME "CefFramework.dll"
#define MODULENAME "UE4Editor-RadiantUI.dll"
#endif

#if !WITH_EDITOR
#undef MODULENAME
#define MODULENAME NULL
#endif

#define FRAMEWORKPATH "..\\..\\CefRuntime\\Binaries\\Win64\\"DLLNAME
#define FRAMEWORKPATH_NOTFOUND_ERROR TEXT("Unable to get path for ") TEXT("..\\..\\CefRuntime\\Binaries\\Win64\\") TEXT(DLLNAME)
#define FRAMEWORKPATH_LOAD_ERROR TEXT("LoadLibrary failed on ") TEXT("..\\..\\CefRuntime\\Binaries\\Win64\\") TEXT(DLLNAME)

	static ICefRuntimeAPI* LoadCefFrameworkDLL(ICefRuntimeCallbacks *InCallbacks)
	{
		UE_LOG(RadiantUILog, Log, TEXT("Loading CEF Framework"));

		static bool bLoaded = false;
		if (bLoaded)
		{
			return nullptr;
		}

		bLoaded = true;

		HMODULE Module = GetModuleHandleA(MODULENAME);
		if (Module == NULL)
		{
			UE_LOG(RadiantUILog, Error, TEXT("Unable to get module handle"));
			return nullptr;
		}

		char szModuleName[256];

		if (0 == GetModuleFileNameA(Module, szModuleName, 255))
		{
			UE_LOG(RadiantUILog, Error, TEXT("Unable to get module name"));
			return nullptr;
		}

		szModuleName[255] = 0;

		char szModulePath[256];
		char *szFilePart;

		if (0 == GetFullPathNameA(szModuleName, 255, szModulePath, &szFilePart))
		{
			UE_LOG(RadiantUILog, Error, FRAMEWORKPATH_NOTFOUND_ERROR);
			return nullptr;
		}

		if (szFilePart)
		{
			*szFilePart = 0;
		}

		strcat_s(szModulePath, 256, FRAMEWORKPATH);

		HMODULE Library = LoadLibraryExA(szModulePath, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS|LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);

		if (Library == NULL)
		{
			UE_LOG(RadiantUILog, Error, FRAMEWORKPATH_LOAD_ERROR);
			return nullptr;
		}

		CreateCefRuntimeAPI f = (CreateCefRuntimeAPI)GetProcAddress(Library, CEFCREATERUNTIMEAPI_SIG);
		return f ? f(InCallbacks) : NULL;
	}
#endif

}

ICefRuntimeAPI* CefStartup(ICefRuntimeCallbacks* InCallbacks)
{
	return LoadCefFrameworkDLL(InCallbacks);
}
