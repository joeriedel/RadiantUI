// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include "Assert.h"
#include "CefClient.h"
#include "CefApplication.h"
#include "include/cef_url.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "Variants.h"
#include <sstream>
#include <algorithm>

class CefStreamWrapper : public CefReadHandler
{
	IMPLEMENT_REFCOUNTING(CefStreamWrapper);
public:

	CefStreamWrapper(ICefStream* InStream) : Stream(InStream) {}
	
	virtual ~CefStreamWrapper()
	{
		if (Stream)
		{
			Stream->Release();
		}
	}

	///
	// Read raw binary data.
	///
	/*--cef()--*/
	virtual size_t Read(void* ptr, size_t size, size_t n) OVERRIDE
	{
		return Stream->Read(ptr, (int)(size*n));
	}

	///
	// Seek to the specified offset position. |whence| may be any one of
	// SEEK_CUR, SEEK_END or SEEK_SET. Return zero on success and non-zero on
	// failure.
	///
	/*--cef()--*/
	virtual int Seek(int64 offset, int whence)
	{
		return Stream->Seek((int)offset, (ICefStream::SeekType)whence) ? 1 : 0;
	}

	///
	// Return the current offset position.
	///
	/*--cef()--*/
	virtual int64 Tell()
	{
		return (int64)Stream->GetPos();
	}

	///
	// Return non-zero if at end of file.
	///
	/*--cef()--*/
	virtual int Eof()
	{
		return Stream->IsEOF() ? 1 : 0;
	}

private:

	ICefStream* Stream;
};

CefFrameworkClient::CefFrameworkClient(int InSizeX, int InSizeY, ICefWebView* InWebView, ICefWebViewCallbacks *InCallbacks) : SizeX(InSizeX), SizeY(InSizeY), WebView(InWebView), Callbacks(InCallbacks), InEditableField(false)
{
}

CefFrameworkClient::~CefFrameworkClient()
{
}

void CefFrameworkClient::Resize(int InSizeX, int InSizeY)
{
	if ((SizeX == InSizeX) && (SizeY == InSizeY))
	{
		return;
	}

	AutoLock L(this);

	SizeX = InSizeX;
	SizeY = InSizeY;

	if (Browser.get())
	{
		Browser->GetHost()->WasResized();
	}
}

void CefFrameworkClient::CloseExistingBrowser()
{
	if (Browser.get())
	{
		Browser->GetHost()->CloseBrowser(true);
	}
}

void CefFrameworkClient::LoadURL(const CefString& InURL)
{
	if (Browser.get())
	{
		Browser->GetMainFrame()->LoadURL(InURL);
	}
}

bool CefFrameworkClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
	ASSERT(source_process == PID_RENDERER);

	if (message->GetName() == RADUIIPCMSG_FOCUSNODECHANGED)
	{
		InEditableField = message->GetArgumentList()->GetBool(0);
		Callbacks->FocusedNodeChanged(InEditableField);
	}
	else
	{
		ICefRuntimeVariantList* Arguments = CefListToVariant(message->GetArgumentList());
		Callbacks->ExecuteJSHook(message->GetName().ToString().c_str(), Arguments);
		Arguments->Release();
	}
	
	return true;
}

// CefContextMenuHandler methods
void CefFrameworkClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
	model->Clear(); // suppress context menus
}

bool CefFrameworkClient::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
	return false; // call default implementation
}

// CefDisplayHandler methods
void CefFrameworkClient::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
}

void CefFrameworkClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
}

bool CefFrameworkClient::OnConsoleMessage(CefRefPtr<CefBrowser> browser, const CefString& message, const CefString& source, int line)
{
	return false;
}

// CefDownloadHandler methods
void CefFrameworkClient::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{
}

void CefFrameworkClient::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback)
{
}

// CefDragHandler methods
bool CefFrameworkClient::OnDragEnter(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDragData> dragData, DragOperationsMask mask)
{
	return false;
}

// CefGeolocationHandler methods
void CefFrameworkClient::OnRequestGeolocationPermission(CefRefPtr<CefBrowser> browser, const CefString& requesting_url, int request_id, CefRefPtr<CefGeolocationCallback> callback)
{
}

// CefKeyboardHandler methods
bool CefFrameworkClient::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut)
{
	return false;
}

// CefLifeSpanHandler methods
bool CefFrameworkClient::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access
	)
{
	return true;// popups not supported (true == cancel)
}

void CefFrameworkClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	Browser = browser;
	Callbacks->WebViewCreated(WebView);
}

bool CefFrameworkClient::DoClose(CefRefPtr<CefBrowser> browser)
{
	if ((Browser.get() != NULL) && (Browser->GetIdentifier() == browser->GetIdentifier()))
	{
		browser->GetHost()->ParentWindowWillClose();
	}

	return false;
}

void CefFrameworkClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	if ((Browser.get() != NULL) && (Browser->GetIdentifier() == browser->GetIdentifier()))
	{
		if (Callbacks)
		{
			Callbacks->Release(WebView);
			WebView = nullptr;
		}

		Browser = NULL;
	}
}

// CefLoadHandler methods
void CefFrameworkClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
	bool isLoading,
	bool canGoBack,
	bool canGoForward
	)
{
}

void CefFrameworkClient::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl
	)
{
	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Don't display an error for external protocols that we allow the OS to
	// handle. See OnProtocolExecution().
	/*if (errorCode == ERR_UNKNOWN_URL_SCHEME) {
		std::string urlStr = frame->GetURL();
		if (urlStr.find("spotify:") == 0)
			return;
	}*/

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

bool CefFrameworkClient::ParseURL(const std::string& URL, std::string& OutPath, std::string& OutMimeType)
{
	/*if ((URL.find(".com") == std::string::npos) 
		&& (URL.find(".net") == std::string::npos) 
		&& (URL.find(".org") == std::string::npos) 
		&& (URL.find("://") == std::string::npos)
		&& (URL.find("www.") == std::string::npos))
	{

	}*/

	CefURLParts Parts;
	CefParseURL(URL, Parts);

	if ((Parts.host.length > 0) && (CefString(&Parts.host) == "content") && (Parts.path.length > 1))
	{
		// local file
		OutPath = CefString(&Parts.path);
		if (OutPath[0] == '/')
		{ // trim leading /
			OutPath = OutPath.substr(1);
		}
		
		// Figure out mime type.
		size_t pos = OutPath.rfind(".");

		OutMimeType = "text/plain";

		if (pos != std::string::npos)
		{
			std::string Ext = OutPath.substr(pos + 1);

			std::transform(Ext.begin(), Ext.end(), Ext.begin(), ::tolower);

			if (Ext == "html")
			{
				OutMimeType = "text/html";
			}
			else if (Ext == "png")
			{
				OutMimeType = "image/png";
			}
			else if (Ext == "jpg")
			{
				OutMimeType = "image/jpg";
			}
			else if (Ext == "bmp")
			{
				OutMimeType = "image/bmp";
			}
			else if (Ext == "css")
			{
				OutMimeType = "text/css";
			}
			else if (Ext == "js")
			{
				OutMimeType = "text/javascript";
			}
		}
		
		return true;
	}

	return false;
}

// CefRequestHandler methods
CefRefPtr<CefResourceHandler> CefFrameworkClient::GetResourceHandler(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request
	)
{
	std::string Path, MimeType;

	if (ParseURL(request->GetURL(), Path, MimeType))
	{
		ICefStream* CefStream = Callbacks->GetFileStream(Path.c_str());
		if (CefStream)
		{
			CefRefPtr<CefReadHandler> ReadHandler(new CefStreamWrapper(CefStream));
			CefRefPtr<CefStreamReader> StreamReader(CefStreamReader::CreateForHandler(ReadHandler));
			return new CefStreamResourceHandler(MimeType, StreamReader);
		}
	}
	return NULL;
}

bool CefFrameworkClient::OnQuotaRequest(CefRefPtr<CefBrowser> browser, const CefString& origin_url, int64 new_size, CefRefPtr<CefQuotaCallback> callback)
{
	// don't allow any disk storage access.
	callback->Continue(false);
	return true;
}

void CefFrameworkClient::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution)
{
	allow_os_execution = true;
}

void CefFrameworkClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus status)
{
}

// CefRenderHandler methods
bool CefFrameworkClient::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	AutoLock L(this);
	rect = CefRect(0, 0, SizeX, SizeY);
	return true;
}

bool CefFrameworkClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
	return GetRootScreenRect(browser, rect);
}

bool CefFrameworkClient::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
	screenX = viewX;
	screenY = viewY;
	return true;
}

bool CefFrameworkClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
	return false;
}

void CefFrameworkClient::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
}

void CefFrameworkClient::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
}

void CefFrameworkClient::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor)
{
	Callbacks->OnCursorChange(cursor);
}

void CefFrameworkClient::OnPaint(
	CefRefPtr<CefBrowser> browser,
	PaintElementType type,
	const RectList& dirtyRects,
	const void* buffer,
	int width, int height
	)
{
	if (!Callbacks->EnterCriticalSection())
	{
		return;
	}

	AutoLock L(this);

	if ((type != PET_VIEW) || (SizeX != width) || (SizeY != height))
	{
		Callbacks->LeaveCriticalSection();
		return;
	}

	const int MaxRegions = 64;
	CefRuntimeRect Regions[MaxRegions];

	if ((int)dirtyRects.size() > MaxRegions)
	{
		CefRuntimeRect &Region = Regions[0];
		Region.X = 0;
		Region.Y = 0;
		Region.Width = width;
		Region.Height = height;
		Callbacks->Repaint(1, &Region, buffer, width, height);
	}
	else
	{
		int NumRegions = 0;

		for (RectList::const_iterator it = dirtyRects.begin(); it != dirtyRects.end(); ++it)
		{
			CefRuntimeRect &Region = Regions[NumRegions++];
			const CefRect &Rect = *it;

			Region.X = Rect.x;
			Region.Y = Rect.y;
			Region.Width = Rect.width;
			Region.Height = Rect.height;
		}

		Callbacks->Repaint(NumRegions, Regions, buffer, width, height);
	}

	Callbacks->LeaveCriticalSection();
}

///////////////////////////////////////////////////////////////////////////////

ICefRuntimeVariantFactory* WebView::GetVariantFactory()
{
	return GetStaticVariantFactory();
}

void WebView::Bind(CefFrameworkClient *InClient)
{
	Client = InClient;
}

void WebView::Resize(int InSizeX, int InSizeY)
{
	if ((InSizeX > 0) && (InSizeY > 0))
	{
		Client->Resize(InSizeX, InSizeY);
	}
}

void WebView::LoadURL(const char *InURL)
{
	Client->LoadURL(InURL);
}

void WebView::SetMouseCursorChangeDisabled(bool InDisabled)
{
	Client->GetHost()->SetMouseCursorChangeDisabled(InDisabled);
}

void WebView::ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments)
{
	CefRefPtr<CefProcessMessage> Message = CefProcessMessage::Create(InHookName);
	if (InArguments)
	{
		SetCefListFromVariantList(Message->GetArgumentList(), InArguments);
	}
	Client->GetBrowser()->SendProcessMessage(PID_RENDERER, Message);
}

bool WebView::IsMouseCursorChangeDisabled()
{
	return Client->GetHost()->IsMouseCursorChangeDisabled();
}

void WebView::WasHidden(bool InHidden)
{
	Client->GetHost()->WasHidden(InHidden);
}

void WebView::SendKeyEvent(const CefRuntimeKeyEvent& InEvent)
{
	Client->GetHost()->SendKeyEvent(Convert(InEvent));
}

void WebView::SendMouseClickEvent(const CefRuntimeMouseEvent& InEvent,
	ECefRuntimeMouseButton InMouseButton,
	bool bInMouseUp, int InClickCount)
{
	Client->GetHost()->SendMouseClickEvent(Convert(InEvent), Convert(InMouseButton), bInMouseUp, InClickCount);
}

void WebView::SendMouseMoveEvent(const CefRuntimeMouseEvent& InEvent,
	bool bInMouseLeave)
{
	Client->GetHost()->SendMouseMoveEvent(Convert(InEvent), bInMouseLeave);
}

void WebView::SendMouseWheelEvent(const CefRuntimeMouseEvent& InEvent,
	int InDeltaX, int InDeltaY)
{
	Client->GetHost()->SendMouseWheelEvent(Convert(InEvent), InDeltaX, InDeltaY);
}

void WebView::SendFocusEvent(bool InSetFocus)
{
	Client->GetHost()->SendFocusEvent(InSetFocus);
}

void WebView::SendCaptureLostEvent()
{
	Client->GetHost()->SendCaptureLostEvent();
}

void* WebView::GetNSTextInputContext()
{
	return Client->GetHost()->GetNSTextInputContext();
}

void WebView::HandleKeyEventBeforeTextInputClient(void* InKeyEvent)
{
	Client->GetHost()->HandleKeyEventBeforeTextInputClient((CefEventHandle)InKeyEvent);
}

void WebView::HandleKeyEventAfterTextInputClient(void* InKeyEvent)
{
	Client->GetHost()->HandleKeyEventAfterTextInputClient((CefEventHandle)InKeyEvent);
}

bool WebView::CanGoBack()
{
	return Client->GetBrowser()->CanGoBack();
}

void WebView::GoBack()
{
	Client->GetBrowser()->GoBack();
}

bool WebView::CanGoForward()
{
	return Client->GetBrowser()->CanGoForward();
}

void WebView::GoForward()
{
	Client->GetBrowser()->GoForward();
}

bool WebView::IsLoading()
{
	return Client->GetBrowser()->IsLoading();
}

void WebView::Reload()
{
	Client->GetBrowser()->Reload();
}

void WebView::ReloadIgnoreCache()
{
	Client->GetBrowser()->ReloadIgnoreCache();
}

void WebView::StopLoad()
{
	Client->GetBrowser()->StopLoad();
}

void WebView::Release()
{
	Client->CloseExistingBrowser();
	delete this;
}

CefKeyEvent WebView::Convert(const CefRuntimeKeyEvent& InEvent)
{
	CefKeyEvent Event;
	Event.character = InEvent.Character;
	Event.focus_on_editable_field = InEvent.FocusOnEditableField;
	Event.is_system_key = InEvent.IsSystemKey;
	Event.modifiers = InEvent.Modifiers;
	Event.native_key_code = InEvent.NativeKeyCode;
	Event.type = Convert(InEvent.Type);
	Event.unmodified_character = InEvent.UnmodifiedCharacter;
	Event.windows_key_code = InEvent.WindowsKeyCode;
	return Event;
}

CefMouseEvent WebView::Convert(const CefRuntimeMouseEvent& InEvent)
{
	CefMouseEvent Event;
	Event.modifiers = InEvent.Modifiers;
	Event.x = InEvent.X;
	Event.y = InEvent.Y;
	return Event;
}

cef_mouse_button_type_t WebView::Convert(ECefRuntimeMouseButton InButtons)
{
	return (cef_mouse_button_type_t)InButtons;
}

cef_key_event_type_t WebView::Convert(ECefRuntimeKeyEvent InKeyState)
{
	return (cef_key_event_type_t)InKeyState;
}
