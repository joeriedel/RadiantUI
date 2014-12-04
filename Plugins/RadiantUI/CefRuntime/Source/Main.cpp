// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include <windows.h>
#include "include/cef_app.h"
#include "CefApplication.h"
#include "CefClient.h"
#include "Variants.h"

#if defined(_BUILD_PROCESS)
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CefMainArgs Args(hInstance);
	CefRefPtr<CefFrameworkApp> App(new CefFrameworkApp());
	return CefExecuteProcess(Args, App.get());
}
#else

inline void DebugString(const char *Text)
{
#if defined(_WIN32)
	OutputDebugStringA(Text);
#endif
}

#if !defined(_BUILD_FRAMEWORK)
#error "This file shouldn't be included here."
#endif

#if defined(_WIN32)
#pragma warning (disable : 4996)
#if defined(_DEBUG)
#define FRAMEWORKNAME  "CefFramework-Debug.dll"
#define CEFPROCESSNAME "CefProcess-Debug.exe"
#else
#define FRAMEWORKNAME "CefFramework.dll"
#define CEFPROCESSNAME "CefProcess.exe"
#endif
#else
#error "Port Me"
#endif

class CefRuntimeAPI : public ICefRuntimeAPI
{
public:

	CefRefPtr<CefFrameworkApp> App;

	virtual ICefRuntimeVariantFactory* GetVariantFactory() OVERRIDE
	{
		return GetStaticVariantFactory();
	}

	bool Initialize(ICefRuntimeCallbacks* InCallbacks)
	{
		Callbacks = InCallbacks;
		App = new CefFrameworkApp();
#if defined(_WIN32)
		DebugString("Initializing " FRAMEWORKNAME "...\n");
		HMODULE Module = GetModuleHandleA(FRAMEWORKNAME);
		if (Module == NULL)
		{
			DebugString("GetModuleHandle failed!");
			return false;
		}

		char szModulePath[256];
		if (0 == GetModuleFileNameA(Module, szModulePath, 255))
		{
			DebugString("GetModuleFileName failed!");
			return false;
		}

		szModulePath[255] = 0;

		DebugString("ModulePath: ");
		DebugString(szModulePath);
		DebugString("\n");

		char szSubprocessPath[256];
		char* szFilePart;

		if (0 == GetFullPathNameA(szModulePath, 255, szSubprocessPath, &szFilePart))
		{
			DebugString("GetFullPathNameA failed!");
			return false;
		}

		if (szFilePart)
		{
			*szFilePart = 0;
		}

		szSubprocessPath[255] = 0;

		strcat(szSubprocessPath, CEFPROCESSNAME);
		
		CefSettings Settings;
		Settings.persist_session_cookies = false;
		Settings.command_line_args_disabled = true;
		Settings.multi_threaded_message_loop = true;
		Settings.single_process = false;
		CefString(&Settings.browser_subprocess_path).FromASCII(szSubprocessPath);

		CefMainArgs Args((HINSTANCE)GetModuleHandle(NULL));
		if (!CefInitialize(Args, Settings, App.get())) {
			App = NULL;
			return false;
		}

		return true;
#endif
	}

	virtual void CreateWebView(const char* InStartupURL, int InSizeX, int InSizeY, bool InTransparentPainting, ICefWebViewCallbacks *InCallbacks)
	{
		if ((InSizeX < 1) || (InSizeY < 1) || (InStartupURL == nullptr) || (InCallbacks == nullptr))
		{
			return;
		}

		WebView* View = new WebView();

		CefRefPtr<CefFrameworkClient> Client(new CefFrameworkClient(InSizeX, InSizeY, View, InCallbacks));
		View->Bind(Client.get());

		CefWindowInfo WindowInfo;
		CefBrowserSettings BrowserSettings;

		WindowInfo.SetAsOffScreen(nullptr);
		WindowInfo.SetTransparentPainting(InTransparentPainting);
		WindowInfo.x = 0;
		WindowInfo.y = 0;
		WindowInfo.width = InSizeX;
		WindowInfo.height = InSizeY;

		BrowserSettings.javascript_access_clipboard = STATE_DISABLED;
		BrowserSettings.javascript_close_windows = STATE_DISABLED;
		BrowserSettings.javascript_open_windows = STATE_DISABLED;

		CefBrowserHost::CreateBrowser(WindowInfo, Client.get(), CefString(InStartupURL), BrowserSettings, nullptr);
	}

	//! Call this when you are done with the runtime API.
	virtual void Release()
	{
		Callbacks->Release();
		App = NULL;
		//CefShutdown();
		delete this;
	}

	ICefRuntimeCallbacks *Callbacks;
};

#if defined(WIN32)
#define DLL_API  __declspec(dllexport)
#else
#define DLL_API
#endif

extern "C" DLL_API ICefRuntimeAPI* CreateCefRuntimeAPI(ICefRuntimeCallbacks* InCallbacks)
{
	CefRuntimeAPI *API = new CefRuntimeAPI();
	if (!API->Initialize(InCallbacks))
	{
		delete API;
		API = nullptr;
	}
	return API;
}

#endif
