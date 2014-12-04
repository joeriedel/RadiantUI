// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.

#pragma once

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "../API/CefRuntimeAPI.h"
#include <set>
#include <string>

class CefFrameworkClient : public CefClient,
	public CefContextMenuHandler,
	public CefDisplayHandler,
	public CefDownloadHandler,
	public CefDragHandler,
	public CefGeolocationHandler,
	public CefKeyboardHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRenderHandler,
	public CefRequestHandler
{
	IMPLEMENT_REFCOUNTING(CefFrameworkClient);
	IMPLEMENT_LOCKING(CefFrameworkClient);
public:
	
	CefFrameworkClient(int InSizeX, int InSizeY, ICefWebView* InWebView, ICefWebViewCallbacks *InCallbacks);
	virtual ~CefFrameworkClient();

	void Resize(int InSizeX, int InSizeY);

	void CloseExistingBrowser();
	void LoadURL(const CefString& InURL);
	
	CefRefPtr<CefBrowser> GetBrowser() { return Browser; }
	CefRefPtr<CefBrowserHost> GetHost() { return Browser->GetHost(); }

	virtual CefRefPtr<CefRenderHandler> GetRenderHandler() 
	{
		return this;
	}

	// CefClient methods
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE
	{
		return this;
	}

	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE
	{
		return this;
	}

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE;
	
	// CefContextMenuHandler methods
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) OVERRIDE;
	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) OVERRIDE;

	// CefDisplayHandler methods
	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url) OVERRIDE;
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) OVERRIDE;
	virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line) OVERRIDE;

	// CefDownloadHandler methods
	virtual void OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback) OVERRIDE;
	virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback) OVERRIDE;

	// CefDragHandler methods
	virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask) OVERRIDE;

	// CefGeolocationHandler methods
	virtual void OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

	// CefKeyboardHandler methods
	virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut) OVERRIDE;

	// CefLifeSpanHandler methods
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access
	) OVERRIDE;

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods
	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward
	) OVERRIDE;

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl
	) OVERRIDE;

	// CefRequestHandler methods
	virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request
	) OVERRIDE;

	virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
		const CefString& origin_url,
		int64 new_size,
		CefRefPtr<CefQuotaCallback> callback
	) OVERRIDE;

	virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
		const CefString& url,
		bool& allow_os_execution
	) OVERRIDE;

	virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
		TerminationStatus status
	) OVERRIDE;

	// CefRenderHandler methods
	virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,CefRect& rect) OVERRIDE;
	virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
	virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY) OVERRIDE;
	virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) OVERRIDE;
	virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
	virtual void OnPopupSize(CefRefPtr<CefBrowser> browser,	const CefRect& rect) OVERRIDE;

	virtual void OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor) OVERRIDE;
	
	virtual void OnPaint(
		CefRefPtr<CefBrowser> browser,
		PaintElementType type,
		const RectList& dirtyRects,
		const void* buffer,
		int width, int height
	) OVERRIDE;

private:

	bool ParseURL(const std::string& URL, std::string& OutPath, std::string& OutMimeType);
	
	int SizeX;
	int SizeY;
	bool InEditableField;
	ICefWebView* WebView;
	ICefWebViewCallbacks *Callbacks;
	CefRefPtr<CefBrowser> Browser;
};

class WebView : public ICefWebView
{
public:

	void Bind(CefFrameworkClient* InClient);

	virtual ICefRuntimeVariantFactory* GetVariantFactory() OVERRIDE;

	virtual void Resize(int InSizeX, int InSizeY);
	virtual void LoadURL(const char *InURL);

	virtual void ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments);

	///
	// Set whether mouse cursor change is disabled.
	///
	/*--cef()--*/
	virtual void SetMouseCursorChangeDisabled(bool InDisabled);

	///
	// Returns true if mouse cursor change is disabled.
	///
	/*--cef()--*/
	virtual bool IsMouseCursorChangeDisabled();

	///
	// Notify the browser that it has been hidden or shown. Layouting and
	// CefRenderHandler::OnPaint notification will stop when the browser is
	// hidden. This method is only used when window rendering is disabled.
	///
	/*--cef()--*/
	virtual void WasHidden(bool InHidden);

	///
	// Send a key event to the browser.
	///
	/*--cef()--*/
	virtual void SendKeyEvent(const CefRuntimeKeyEvent& InEvent);

	///
	// Send a mouse click event to the browser. The |x| and |y| coordinates are
	// relative to the upper-left corner of the view.
	///
	/*--cef()--*/
	virtual void SendMouseClickEvent(const CefRuntimeMouseEvent& InEvent,
		ECefRuntimeMouseButton InMouseButton,
		bool bInMouseUp, int InClickCount);

	///
	// Send a mouse move event to the browser. The |x| and |y| coordinates are
	// relative to the upper-left corner of the view.
	///
	/*--cef()--*/
	virtual void SendMouseMoveEvent(const CefRuntimeMouseEvent& InEvent,
		bool bInMouseLeave);

	///
	// Send a mouse wheel event to the browser. The |x| and |y| coordinates are
	// relative to the upper-left corner of the view. The |deltaX| and |deltaY|
	// values represent the movement delta in the X and Y directions respectively.
	// In order to scroll inside select popups with window rendering disabled
	// CefRenderHandler::GetScreenPoint should be implemented properly.
	///
	/*--cef()--*/
	virtual void SendMouseWheelEvent(const CefRuntimeMouseEvent& InEvent,
		int InDeltaX, int InDeltaY);

	///
	// Send a focus event to the browser.
	///
	/*--cef()--*/
	virtual void SendFocusEvent(bool InSetFocus);

	///
	// Send a capture lost event to the browser.
	///
	/*--cef()--*/
	virtual void SendCaptureLostEvent();

	///
	// Get the NSTextInputContext implementation for enabling IME on Mac when
	// window rendering is disabled.
	///
	/*--cef(default_retval=NULL)--*/
	virtual void* GetNSTextInputContext();

	///
	// Handles a keyDown event prior to passing it through the NSTextInputClient
	// machinery.
	///
	/*--cef()--*/
	virtual void HandleKeyEventBeforeTextInputClient(void* InKeyEvent);

	///
	// Performs any additional actions after NSTextInputClient handles the event.
	///
	/*--cef()--*/
	virtual void HandleKeyEventAfterTextInputClient(void* InKeyEvent);

	///
	// Returns true if the browser can navigate backwards.
	///
	/*--cef()--*/
	virtual bool CanGoBack();

	///
	// Navigate backwards.
	///
	/*--cef()--*/
	virtual void GoBack();

	///
	// Returns true if the browser can navigate forwards.
	///
	/*--cef()--*/
	virtual bool CanGoForward();

	///
	// Navigate forwards.
	///
	/*--cef()--*/
	virtual void GoForward();

	///
	// Returns true if the browser is currently loading.
	///
	/*--cef()--*/
	virtual bool IsLoading();

	///
	// Reload the current page.
	///
	/*--cef()--*/
	virtual void Reload();

	///
	// Reload the current page ignoring any cached data.
	///
	/*--cef()--*/
	virtual void ReloadIgnoreCache();

	///
	// Stop loading the page.
	///
	/*--cef()--*/
	virtual void StopLoad();

	virtual void Release();

private:

	CefKeyEvent Convert(const CefRuntimeKeyEvent& InEvent);
	CefMouseEvent Convert(const CefRuntimeMouseEvent& InEvent);
	cef_mouse_button_type_t Convert(ECefRuntimeMouseButton InButtons);
	cef_key_event_type_t Convert(ECefRuntimeKeyEvent InKeyState);

	CefRefPtr<CefFrameworkClient> Client;
};