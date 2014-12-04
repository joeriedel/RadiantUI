// Copyright 2014 Joseph Riedel. All Rights Reserved.

#pragma once

#include "include/cef_app.h"
#include <map>

#define RADUIIPCMSG_FOCUSNODECHANGED "RADUIIPC.EditModeChanged"

class CefFrameworkApp : public CefApp, public CefBrowserProcessHandler, public CefRenderProcessHandler
{
	IMPLEMENT_REFCOUNTING(CefFrameworkApp);

public:

	void SetJSHook(
		const CefString& InHookName,
		int InBrowserID,
		CefRefPtr<CefV8Context> InContext,
		CefRefPtr<CefV8Value> InFunction
	);

	void RemoveJSHook(const CefString& InHookName, int InBrowserId);

	// CefApp
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE{ return this; }

	// CefRenderProcessHandler methods.
	virtual void OnWebKitInitialized() OVERRIDE;

	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;

	virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) OVERRIDE;

	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

private:

	struct JSHook
	{
		CefRefPtr<CefV8Context> Context;
		CefRefPtr<CefV8Value> Function;
	};

	typedef std::map<std::pair<std::string, int>, JSHook> JSHookMap;

	JSHookMap Hooks;
	
};