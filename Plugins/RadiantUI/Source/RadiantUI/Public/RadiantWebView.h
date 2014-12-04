// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantCanvasRenderTarget.h"
#include "../../../CefRuntime/API/CefJavaScriptAPI.h"
#include "RadiantWebView.generated.h"

class ICefWebView;
class ICefStream;
struct CefRuntimeRect;

UENUM()
namespace ERadiantWebViewCursor
{
	enum Type
	{
		Arrow,
		Hover
	};
}

USTRUCT()
struct RADIANTUI_API FRadiantWebViewCursor
{
	GENERATED_USTRUCT_BODY()

	FRadiantWebViewCursor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cursor)
	FIntPoint HotSpot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cursor)
	float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cursor)
	UTexture2D* Image;
};

USTRUCT()
struct RADIANTUI_API FRadiantWebViewCursorSet
{
	GENERATED_USTRUCT_BODY()

	FRadiantWebViewCursorSet();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cursor)
	FRadiantWebViewCursor Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Cursor)
	FRadiantWebViewCursor Hover;
};

USTRUCT()
struct RADIANTUI_API FRadiantWebViewDefaultSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings, meta = (UIMin = "64", UIMax = "32768"))
	FIntPoint Size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings)
	uint32 bTransparentRendering:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	float RefreshRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings)
	FString URL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings)
	FColor InitialCanvasColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings, meta=(Tooltop="If true a mouse cursor will be shown. Typically disabled for Menu UI's"))
	bool bProjectedCursor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Settings)
	FRadiantWebViewCursorSet Cursors;

	FRadiantWebViewDefaultSettings()
	{
		Size = FIntPoint(1024, 1024);
		bTransparentRendering = false;
		RefreshRate = 30.0f;
		InitialCanvasColor = FColor(0, 0, 0, 0);
		bProjectedCursor = true;
		URL = TEXT("http://www.unrealengine.com");
	}
};

class FRadiantWebViewCallbacks;

class FRadiantWebView
{
public:

	FRadiantWebView(const FRadiantWebViewDefaultSettings& Settings);
	~FRadiantWebView();

	DECLARE_EVENT_TwoParams(FRadiantWebView, FOnExecuteJSHook, const FString&, ICefRuntimeVariantList*);
	// Invoked from JavaScript to Run Game Function
	FOnExecuteJSHook OnExecuteJSHook;

	// Only valid if Start() or PreCreateTexture() have been called.
	UTexture2D* WebViewTexture;
	URadiantCanvasRenderTarget* WebViewCanvas;
	
	bool CanNavigateForward();
	bool CanNavigateBackward();
	void NavigateForward();
	void NavigateBackward();
	void LoadURL(const FString& URL);
	FString GetURL() { return URL; }
	void SetRefreshRate(float FramesPerSecond);
	float GetRefreshRate() { return RefreshRate; }
	void Resize(const FIntPoint& Size);
	FIntPoint GetSize() { return Size; }

	bool IsTransparentRendering() { return bTransparentRendering; }

	bool SetCursorPosition(const FVector2D& CursorPosition);
	FVector2D GetCursorPosition() { return CursorPosition; }

	void SetCursorVisible(bool bVisible);
	bool GetCursorVisible() { return bCursorVisible; }

	bool HasInitialFrame() { return bHasInitialFrame; }
	bool IsFocusingEditableField() { return bFocusingEditableField; }
	bool IsRunning() { return bRunning; }

	void SetNetMode(ENetMode InNetMode);
	void Start();
	void PauseRefresh();
	void Stop(bool bPreserveRenderTarget = false);
	void PreCreateTexture();

	uint8 GetPixelAlpha(int X, int Y);
	ERadiantWebViewCursor::Type GetMouseCursor();

	ICefRuntimeVariantFactory* GetVariantFactory();
	void CallJavaScriptFunction(const char* InHookName, ICefRuntimeVariantList* InArguments);

	void Tick(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel);
	
	ICefWebView* GetBrowser();
	FIntPoint GetBrowserCursorPosition();

private:

	friend class FRadiantWebViewCallbacks;

	struct FQueuedCallback
	{
		FQueuedCallback() : Arguments(nullptr) {}
		
		FQueuedCallback(const FQueuedCallback& Other) : Arguments(nullptr)
		{
			*this = Other;
		}

		FQueuedCallback(const FString& InHookName, ICefRuntimeVariantList* InArguments)
		: HookName(InHookName), Arguments(InArguments)
		{
			if (Arguments)
			{
				Arguments->AddRef();
			}
		}

		~FQueuedCallback()
		{
			if (Arguments)
			{
				Arguments->Release();
			}
		}

		FQueuedCallback& operator = (const FQueuedCallback& Other)
		{
			HookName = Other.HookName;

			if (Arguments)
			{
				Arguments->Release();
			}
			
			Arguments = Other.Arguments;

			if (Arguments)
			{
				Arguments->AddRef();
			}

			return *this;
		}

		FString HookName;
		ICefRuntimeVariantList* Arguments;
	};

	TArray<FQueuedCallback> PendingCallbacks;
	FRadiantWebViewCursor* MouseCursor;
	ICefWebView* volatile WebView;

	FString URL;
	FIntPoint Size;
	FVector2D CursorPosition;
	FColor InitialCanvasColor;
	FRadiantWebViewCursorSet Cursors;
	void* SurfacePtr;
	FRadiantWebViewCallbacks* CallbacksInterface;
	bool bFocusingEditableField;
	bool bHasInitialFrame;
	bool bRunning;
	bool bCreated;
	bool bDedicatedServer;
	bool bTextureDirty;
	bool bCursorMoved;
	bool bTransparentRendering;
	bool bCursorVisible;
	bool bCursorEnabled;
	volatile int NumPendingRenderCommands;
	float TextureUpdateTime;
	float RefreshRate;

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER_DECLARE(
		FQueueUpdateTextureCmd,
		FRadiantWebView*, WebView, nullptr,
		{
		WebView->RenderThread_UpdateTexture();
		--WebView->NumPendingRenderCommands;
	})

	friend class EURCMacro_FQueueUpdateTextureCmd;

	void InternalInit();
	void DestroyWebView_Concurrent(bool bPreserveRenderTarget);
	void StartRefresh();
	void StopRefresh();
	void CreateTexture();
	void CreateTextureIfNeeded();
	void CreateBrowser();
	void CreateWebView();
	void AcquireBrowser();
	void FlushRenderUpdates();
	void TickTextureUpdate(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel);
	void UpdateTextureAndRedrawCanvas(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel);
	void ClearSurfaceToColor(const FColor& InColor);
	void RedrawCanvas(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel);
	void RenderThread_UpdateTexture();
	void BlitWebViewToRenderTarget();
	void BlitCursor();
	void ProcessPendingCallbacks();
	void ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments);

	// Begin ICefWebViewCallbacks Interface
	void WebViewCreated(ICefWebView* InWebView);

	// Called when regions in the webview are rendered.
	void Repaint(int InNumRegions, const CefRuntimeRect* InRegions, const void* InBuffer, int InWidth, int InHeight);

	// Called when the cursor changes
	void OnCursorChange(void* InPlatformCursorHandle);

	// Called when the focused item changes
	void FocusedNodeChanged(bool InIsEditableField);

	// Called by JavaScript to execute a hook function in the game
	void ExecuteJSHook_Callback(const char* InHookName, ICefRuntimeVariantList* InArguments);

	// Open a file (if it exists).
	ICefStream* GetFileStream(const char* FilePath);

	// When the associated webview is being released.
	// If there are no more references to the ICefWebViewCallbacks 
	// object it should be destroyed by the owner.
	virtual void Release(ICefWebView *InWebView);

	// End ICefWebViewCallbacks;

	FCriticalSection CriticalSection;
};