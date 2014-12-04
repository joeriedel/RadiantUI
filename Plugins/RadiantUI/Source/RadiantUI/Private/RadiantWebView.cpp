// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "../../../CefRuntime/API/CefRuntimeAPI.h"

struct FRect
{
	uint16 X;
	uint16 Y;
	uint16 W;
	uint16 H;
};

namespace
{
	class NakedFileStream : public ICefStream
	{
	public:
		int32 Ofs;
		TArray<uint8> Data;

		NakedFileStream() : Ofs(0)
		{
		}

		virtual void Release()
		{
			delete this;
		}

		virtual int Read(void* Ptr, int Count)
		{
			if (IsEOF())
			{
				return 0;
			}

			Count = ((Ofs + Count) <= Data.Num()) ? Count : (Data.Num() - Ofs);
			FMemory::Memcpy(Ptr, &Data[Ofs], Count);
			Ofs += Count;
			return Count;
		}

		virtual bool Seek(int Offset, SeekType Whence)
		{
			switch (Whence)
			{
			case ICefStream::Stream_Current:
				Offset = Ofs + Offset;
				break;
			case ICefStream::Stream_End:
				Offset = Data.Num() + Offset;
				break;
			case ICefStream::Stream_Set:
				break;
			}

			if ((Offset >= 0) && (Offset < Data.Num()))
			{
				Ofs = Offset;
				return true;
			}

			return false;
		}

		virtual int GetPos()
		{
			return Ofs;
		}

		virtual bool IsEOF()
		{
			return Ofs >= Data.Num();
		}
	};

	ICefStream* LoadFileToStream(const FString& FilePath)
	{
		NakedFileStream* Stream = new NakedFileStream();
		if (FFileHelper::LoadFileToArray(Stream->Data, *FilePath))
		{
			return Stream;
		}
		delete Stream;
		return nullptr;
	}
}

class FRadiantWebViewCallbacks : public ICefWebViewCallbacks
{
public:
	FRadiantWebViewCallbacks(FRadiantWebView* InComponent) : Component(InComponent)
	{
	}

	// Begin ICefWebViewCallbacks Interface
	virtual void WebViewCreated(ICefWebView* InWebView) override
	{
		check(Component);
		Component->WebViewCreated(InWebView);
	}

	// Called when regions in the webview are rendered.
	virtual void Repaint(int InNumRegions, const CefRuntimeRect* InRegions, const void* InBuffer, int InWidth, int InHeight) override
	{
		check(Component);
		Component->Repaint(InNumRegions, InRegions, InBuffer, InWidth, InHeight);
	}

	// Called when the cursor changes
	virtual void OnCursorChange(void* InPlatformCursorHandle) override
	{
		check(Component);
		Component->OnCursorChange(InPlatformCursorHandle);
	}

	// Called when the focused item changes
	virtual void FocusedNodeChanged(bool InIsEditableField) override
	{
		check(Component);
		Component->FocusedNodeChanged(InIsEditableField);
	}

	// Called by JavaScript to execute a hook function in the game
	virtual void ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments) override
	{
		check(Component);
		Component->ExecuteJSHook(InHookName, InArguments);
	}

	virtual ICefStream* GetFileStream(const char* FilePath) override
	{
		check(Component);
		return Component->GetFileStream(FilePath);
	}

	// When the associated webview is being released.
	// If there are no more references to the ICefWebViewCallbacks 
	// object it should be destroyed by the owner.
	virtual void Release(ICefWebView *InWebView) override
	{
		check(Component);
		Component->Release(InWebView);
	}

	virtual bool EnterCriticalSection()
	{
		check(Component);
		Component->CriticalSection.Lock();
		return true;
	}

	virtual void LeaveCriticalSection()
	{
		check(Component);
		Component->CriticalSection.Unlock();
	}

	// End ICefWebViewCallbacks;

private:

	FRadiantWebView* Component;
};

enum
{
	MaxPendingRectUpdates = 64
};


FRadiantWebViewCursor::FRadiantWebViewCursor()
{
	HotSpot = FIntPoint(0, 0);
	Scale = 1.0f;
	Image = nullptr;
}

FRadiantWebViewCursorSet::FRadiantWebViewCursorSet()
{
#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultArrow(TEXT("/RadiantUI/DefaultArrow"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultHover(TEXT("/RadiantUI/DefaultHover"));

	Arrow.Image = DefaultArrow.Object;
	Hover.Image = DefaultHover.Object;
#endif
}

FRadiantWebView::FRadiantWebView(const FRadiantWebViewDefaultSettings& Settings)
: Cursors(Settings.Cursors)
, Size(Settings.Size)
, RefreshRate(Settings.RefreshRate)
, URL(Settings.URL)
, bCursorEnabled(Settings.bProjectedCursor)
, InitialCanvasColor(Settings.InitialCanvasColor)
{
	bCursorVisible = false;
	bFocusingEditableField = false;
	bTransparentRendering = Settings.bTransparentRendering;

	CursorPosition = FVector2D(0.5, 0.5);

	NumPendingRenderCommands = 0;
	WebView = nullptr;
	WebViewTexture = nullptr;
	WebViewCanvas = nullptr;
	SurfacePtr = nullptr;
	CallbacksInterface = new FRadiantWebViewCallbacks(this);
	bRunning = false;
	bCreated = false;
	bTextureDirty = false;
	bDedicatedServer = false;
	bCursorMoved = false;
	bFocusingEditableField = false;
	bHasInitialFrame = false;
	bRunning = false;
	TextureUpdateTime = 0.0f;
	MouseCursor = &Cursors.Arrow;
}

FRadiantWebView::~FRadiantWebView()
{
	DestroyWebView_Concurrent(false);
	delete CallbacksInterface;
}

void FRadiantWebView::SetNetMode(ENetMode InNetMode)
{
	bDedicatedServer = InNetMode == NM_DedicatedServer;
}

void FRadiantWebView::Start()
{
	if (!bRunning)
	{
		bRunning = true;
		StartRefresh();
	}
}

void FRadiantWebView::PauseRefresh()
{
	if (bRunning)
	{
		bRunning = false;
		StopRefresh();
	}
}

void FRadiantWebView::Stop(bool bPreserveRenderTarget)
{
	DestroyWebView_Concurrent(bPreserveRenderTarget);
}

bool FRadiantWebView::SetCursorPosition(const FVector2D& InCursorPosition)
{
	FVector2D NewPosition;

	NewPosition.X = FMath::Clamp<float>(InCursorPosition.X, 0, 1);
	NewPosition.Y = FMath::Clamp<float>(InCursorPosition.Y, 0, 1);

	if (NewPosition != CursorPosition)
	{
		CursorPosition = NewPosition;
		bCursorMoved = true;
		return true;
	}

	return false;
}

void FRadiantWebView::SetCursorVisible(bool bInVisible)
{
	if (bInVisible != bCursorVisible)
	{
		bCursorVisible = bInVisible;
		bCursorMoved = true;
	}
}

void FRadiantWebView::CreateWebView()
{
	if (bCreated)
	{
		return;
	}

	CreateTextureIfNeeded();
	CreateBrowser();
	bCreated = true;
}

void FRadiantWebView::DestroyWebView_Concurrent(bool bPreserveRenderTarget)
{
	if (!bCreated)
	{
		if (!bPreserveRenderTarget && WebViewTexture)
		{
			WebViewTexture->RemoveFromRoot();
			WebViewTexture->MarkPendingKill();
			WebViewTexture = nullptr;
		}
		return;
	}

	{
		AcquireBrowser();
		FlushRenderUpdates();

		FScopeLock L(&CriticalSection);
				
		if (WebViewCanvas)
		{
			WebViewCanvas->RemoveFromRoot();
			WebViewCanvas->Destroy();
			WebViewCanvas = nullptr;
		}
		if (!bPreserveRenderTarget && WebViewTexture)
		{
			WebViewTexture->RemoveFromRoot();
			WebViewTexture->MarkPendingKill();
			WebViewTexture = nullptr;
		}

		if (SurfacePtr)
		{
			FMemory::Free(SurfacePtr);
			SurfacePtr = nullptr;
		}

		if (WebView)
		{
			WebView->Release();
		}
	}

	while (WebView)
	{
		FPlatformProcess::Sleep(0);
	}
}

bool FRadiantWebView::CanNavigateForward()
{
	return WebView && WebView->CanGoForward();
}

bool FRadiantWebView::CanNavigateBackward()
{
	return WebView && WebView->CanGoBack();
}

void FRadiantWebView::NavigateForward()
{
	if (CanNavigateForward())
	{
		WebView->GoForward();
	}
}

void FRadiantWebView::NavigateBackward()
{
	if (CanNavigateForward())
	{
		WebView->GoBack();
	}
}

void FRadiantWebView::LoadURL(const FString& InURL)
{
	if (!bCreated)
	{
		return;
	}

	AcquireBrowser();
	if (WebView)
	{
		URL = InURL;

		FTCHARToUTF8 Convert(*URL);
		WebView->LoadURL(Convert.Get());
	}
}

void FRadiantWebView::StartRefresh()
{
	if (!bCreated)
	{
		CreateWebView();
		bRunning = true;
		return;
	}

	if (bRunning)
	{
		return;
	}

	AcquireBrowser();
	if (WebView)
	{
		WebView->WasHidden(false);
	}

	bRunning = true;
}

void FRadiantWebView::StopRefresh()
{
	if (!bCreated || !bRunning)
	{
		return;
	}

	AcquireBrowser();

	if (WebView)
	{
		WebView->WasHidden(true);
	}

	bRunning = false;
}

void FRadiantWebView::SetRefreshRate(float InFramesPerSecond)
{
	RefreshRate = InFramesPerSecond;
}

ICefWebView* FRadiantWebView::GetBrowser()
{
	if (!bCreated)
	{
		return nullptr;
	}

	AcquireBrowser();
	return WebView;
}

FIntPoint FRadiantWebView::GetBrowserCursorPosition()
{
	FIntPoint WebViewCursorPosition;

	WebViewCursorPosition.X = (int)(CursorPosition.X * Size.X);
	WebViewCursorPosition.Y = (int)(CursorPosition.Y * Size.Y);

	return WebViewCursorPosition;
}

void FRadiantWebView::CreateBrowser()
{
	ICefRuntimeAPI *API = GetCefRuntime();
	if (API)
	{
		FTCHARToUTF8 Convert(*URL);
		API->CreateWebView(
			Convert.Get(), 
			Size.X, 
			Size.Y, 
			bTransparentRendering, 
			CallbacksInterface
			);
	}
}

void FRadiantWebView::AcquireBrowser()
{
	ICefRuntimeAPI *API = GetCefRuntime();
	if (API)
	{
		while (WebView == nullptr)
		{
			FPlatformProcess::Sleep(0);
		}
	}
}

void FRadiantWebView::Resize(const FIntPoint& InSize)
{
	if (Size == InSize)
	{
		return;
	}

	AcquireBrowser();

	// Wait for pending texture uploads.
	FlushRenderUpdates();

	FScopeLock L(&CriticalSection);

	Size = InSize;

	bHasInitialFrame = false;

	// Create new texture
	CreateTexture();

	// Resize existing webview.
	if (WebView)
	{
		WebView->Resize(Size.X, Size.Y);
	}
}

void FRadiantWebView::FlushRenderUpdates()
{
	// Could use a FRenderCommandFence here but 
	// I don't want to wait to flush the entire render thread
	// I just want to wait until any outstanding paints are done.

	while (NumPendingRenderCommands > 0)
	{
		FPlatformProcess::Sleep(0);
	}
}

void FRadiantWebView::Tick(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel)
{
	ProcessPendingCallbacks();

	if (!bDedicatedServer)
	{
		TickTextureUpdate(InRealTime, InWorldTime, InWorldDeltaTime, FeatureLevel);
	}
}

void FRadiantWebView::PreCreateTexture()
{
	CreateTextureIfNeeded();
}

void FRadiantWebView::CreateTextureIfNeeded()
{
	if (!WebViewTexture)
	{
		CreateTexture();
	}
}

void FRadiantWebView::CreateTexture()
{
	if (bDedicatedServer)
	{
		return;
	}
	if (WebViewCanvas)
	{
		WebViewCanvas->RemoveFromRoot();
		WebViewCanvas->Destroy();
	}
	
	WebViewCanvas = URadiantCanvasRenderTarget::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8, InitialCanvasColor);

	if (WebViewCanvas)
	{
		WebViewCanvas->AddToRoot();
	}

	if (WebViewTexture)
	{
		WebViewTexture->RemoveFromRoot();
		WebViewTexture->MarkPendingKill();
	}

	WebViewTexture = UTexture2D::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8);

	if (WebViewTexture)
	{
		WebViewTexture->AddToRoot();
		WebViewTexture->LODGroup = TEXTUREGROUP_UI;
		WebViewTexture->CompressionSettings = TC_EditorIcon;
		WebViewTexture->Filter = TF_Default;
		WebViewTexture->UpdateResource();
	}

	if (SurfacePtr)
	{
		FMemory::Free(SurfacePtr);
	}

	SurfacePtr = FMemory::Malloc(Size.X*Size.Y * 4, 16);

	bCursorMoved = true;
}

uint8 FRadiantWebView::GetPixelAlpha(int X, int Y)
{
	if (!bHasInitialFrame || !SurfacePtr || (X < 0) || (X >= Size.X) || (Y < 0) || (Y >= Size.Y))
	{
		return 0;
	}

	const int Stride = Size.X * 4;
	uint8 Alpha = reinterpret_cast<uint8*>(SurfacePtr)[(Stride*Y)+(X*4)+3];
	return Alpha;
}

ERadiantWebViewCursor::Type FRadiantWebView::GetMouseCursor()
{
	if (MouseCursor == &Cursors.Hover)
	{
		return ERadiantWebViewCursor::Hover;
	}

	return ERadiantWebViewCursor::Arrow;
}

ICefRuntimeVariantFactory* FRadiantWebView::GetVariantFactory()
{
	AcquireBrowser();
	check(WebView);
	return WebView->GetVariantFactory();
}

void FRadiantWebView::CallJavaScriptFunction(const char* InHookName, ICefRuntimeVariantList* InArguments)
{
	AcquireBrowser();
	check(WebView);
	WebView->ExecuteJSHook(InHookName, InArguments);
}

void FRadiantWebView::ProcessPendingCallbacks()
{
	const int MaxCallbacksPerFrame = 8;
	FQueuedCallback Stack[MaxCallbacksPerFrame];
	int NumCallbacks = 0;
	{
		FScopeLock L(&CriticalSection);
		for (auto It = PendingCallbacks.CreateConstIterator(); It && (NumCallbacks < MaxCallbacksPerFrame); ++It)
		{
			const FQueuedCallback& Callback = *It;
			Stack[NumCallbacks++] = *It;
		}

		PendingCallbacks.Empty(PendingCallbacks.Max());
	}

	for (int i = 0; i < NumCallbacks; ++i)
	{
		const FQueuedCallback& Callback = Stack[i];
		OnExecuteJSHook.Broadcast(Callback.HookName, Callback.Arguments);
	}
}

void FRadiantWebView::ExecuteJSHook(const char* InHookName, ICefRuntimeVariantList* InArguments)
{
	FScopeLock L(&CriticalSection);
	PendingCallbacks.Add(FQueuedCallback(FString(InHookName), InArguments));
}

void FRadiantWebView::WebViewCreated(ICefWebView* InWebView)
{
	WebView = InWebView;
	WebView->SendFocusEvent(false);
}

void FRadiantWebView::TickTextureUpdate(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel)
{
	TextureUpdateTime += InWorldDeltaTime;
	if (bTextureDirty || bCursorMoved)
	{
		if (bCursorMoved || (RefreshRate <= 0.0f) || (TextureUpdateTime >= (1.0f / RefreshRate)))
		{
			if ((SurfacePtr == nullptr) || (WebViewTexture == nullptr) || (WebViewTexture->Resource == nullptr))
			{
				return;
			}

			UpdateTextureAndRedrawCanvas(InRealTime, InWorldTime, InWorldDeltaTime, FeatureLevel);
			TextureUpdateTime = 0.0f;
		}
	}
}

// Called when regions in the webview are rendered.
void FRadiantWebView::Repaint(int InNumRegions, const CefRuntimeRect* InRegions, const void* InBuffer, int InWidth, int InHeight)
{
	// NOTE: CEF calls EnterCriticalSection for us
	// When we get here FRadiantWebView::CriticalSection
	// has been entered.

	if ((WebViewCanvas == nullptr) || (WebViewTexture == nullptr) || (WebViewTexture->Resource == nullptr))
	{
		return;
	}

	if ((InWidth < 1) || (InHeight < 1) || (InNumRegions < 1))
	{
		return;
	}

	if ((InWidth != (int)WebViewTexture->GetSizeX()) || (InHeight != (int)WebViewTexture->GetSizeY()))
	{
		return;
	}

	// update the offscreen surface
	const int BPP = 4;
	const int SurfaceStride = InWidth * BPP;
	uint8* const BaseDstSurfacePtr = (uint8*)SurfacePtr;
	uint8* const BaseSrcSurfacePtr = (uint8*)InBuffer;

	for (int i = 0; i < InNumRegions; ++i)
	{
		const CefRuntimeRect &Rect = InRegions[i];
		const int RectStride = Rect.Width * 4;

		uint8* DstScanLine = BaseDstSurfacePtr + SurfaceStride*Rect.Y + Rect.X*BPP;
		uint8* SrcScanLine = BaseSrcSurfacePtr + SurfaceStride*Rect.Y + Rect.X*BPP;
		for (int h = 0; h < Rect.Height; ++h)
		{
			FMemory::Memcpy(DstScanLine, SrcScanLine, RectStride);
			DstScanLine += SurfaceStride;
			SrcScanLine += SurfaceStride;
		}
	}

	//ClearSurfaceToColor(FColor::Red);

	bHasInitialFrame = true;
	bTextureDirty = true;
}

void FRadiantWebView::ClearSurfaceToColor(const FColor& InColor)
{
	int NumPixels = Size.X*Size.Y;

	uint32 IntColor = ((int)InColor.A << 24)
		| ((int)InColor.R << 16) | ((int)InColor.G << 8)
		| InColor.B;

	uint32 *PixelPtr = (uint32*)SurfacePtr;
	while (NumPixels-- > 0)
	{
		*PixelPtr = IntColor;
		++PixelPtr;
	}
}

void FRadiantWebView::UpdateTextureAndRedrawCanvas(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel)
{
	{
		FScopeLock L(&CriticalSection);
		++NumPendingRenderCommands;
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER_CREATE(FQueueUpdateTextureCmd, FRadiantWebView*, this);
		bTextureDirty = false;
	}

	RedrawCanvas(InRealTime, InWorldTime, InWorldDeltaTime, FeatureLevel);
}

void FRadiantWebView::RenderThread_UpdateTexture()
{
	check(IsInRenderingThread());

	FUpdateTextureRegion2D UpdateRegion(
		(int32)0,
		(int32)0,
		(int32)0,
		(int32)0,
		(int32)WebViewTexture->GetSizeX(),
		(int32)WebViewTexture->GetSizeY()
		);

	RHIUpdateTexture2D(static_cast<FTexture2DResource*>(WebViewTexture->Resource)->GetTexture2DRHI(), 0, UpdateRegion, WebViewTexture->GetSizeX() * 4, (uint8*)SurfacePtr);
}

void FRadiantWebView::RedrawCanvas(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel)
{
	WebViewCanvas->BeginPaint(InRealTime, InWorldTime, InWorldDeltaTime, FeatureLevel);
	BlitWebViewToRenderTarget();
	BlitCursor();
	WebViewCanvas->EndPaint();
}

void FRadiantWebView::BlitWebViewToRenderTarget()
{
	FCanvasTileItem CanvasTile(FVector2D(0, 0), WebViewTexture->Resource, FVector2D(WebViewTexture->GetSizeX(), WebViewTexture->GetSizeY()), FColor::White);
	CanvasTile.BlendMode = SE_BLEND_Opaque;
	CanvasTile.Draw(WebViewCanvas->Canvas);
}

void FRadiantWebView::BlitCursor()
{
	if (bCursorVisible && bCursorEnabled && (MouseCursor != nullptr) && (MouseCursor->Image != nullptr) && (MouseCursor->Image->Resource != nullptr))
	{
		FVector2D DrawPos(-MouseCursor->HotSpot.X, -MouseCursor->HotSpot.Y);
		DrawPos *= MouseCursor->Scale;
		DrawPos += FVector2D(CursorPosition.X * Size.X, CursorPosition.Y * Size.Y);

		FVector2D DrawSize(MouseCursor->Image->GetSizeX() * MouseCursor->Scale, MouseCursor->Image->GetSizeY() * MouseCursor->Scale);

		FCanvasTileItem CanvasTile(DrawPos, MouseCursor->Image->Resource, DrawSize, FColor::White);
		CanvasTile.BlendMode = SE_BLEND_Translucent;
		CanvasTile.Draw(WebViewCanvas->Canvas);
	}

	bCursorMoved = false;
}

void FRadiantWebView::OnCursorChange(void* InPlatformCursorHandle)
{
	if (LoadCursor(NULL, IDC_HAND) == InPlatformCursorHandle)
	{
		MouseCursor = &Cursors.Hover;
	}
	else
	{
		MouseCursor = &Cursors.Arrow;
	}
}

// Called when the focused item changes
void FRadiantWebView::FocusedNodeChanged(bool InIsEditableField)
{
	bFocusingEditableField = InIsEditableField;
}

// Called by JavaScript to execute a hook function in the game
void FRadiantWebView::ExecuteJSHook_Callback(const char* InHookName, ICefRuntimeVariantList* InArguments)
{
	check(InHookName);
	check(InArguments);

	FString HookName(InHookName);
	OnExecuteJSHook.Broadcast(HookName, InArguments);
}

ICefStream* FRadiantWebView::GetFileStream(const char* FilePath)
{
	FString FullPath = FString::Printf(TEXT("%s%s"), *FPaths::GameContentDir(), *FString(FilePath));
	return LoadFileToStream(FullPath);
}

// Called when the associated webview is being released.
// If there are no more references to the ICefWebViewCallbacks 
// object it should be destroyed by the owner.
void FRadiantWebView::Release(ICefWebView *InWebView)
{
	WebView = nullptr;
	bCreated = false;
	bRunning = false;
}

/*
#if WITH_EDITOR
void FRadiantWebView::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(FRadiantWebView, URL))
	{
		LoadURL(URL);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
*/