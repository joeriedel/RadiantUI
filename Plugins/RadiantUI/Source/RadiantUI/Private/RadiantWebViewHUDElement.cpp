// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "JSCall.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace
{
ECefRuntimeMouseButton GetMouseButton(const FPointerEvent& InPointerEvent)
{
	FKey Key = InPointerEvent.GetEffectingButton();
	if (Key == EKeys::LeftMouseButton)
	{
		return CEFRT_MouseLeft;
	}
	if (Key == EKeys::RightMouseButton)
	{
		return CEFRT_MouseRight;
	}

	check(Key == EKeys::MiddleMouseButton);
	return CEFRT_MouseMiddle;
}

#if PLATFORM_WINDOWS

inline int ExtendedKeyBits(int VK)
{
	if ((VK == VK_LSHIFT) || (VK == VK_RSHIFT)
		|| (VK == VK_LCONTROL) || (VK == VK_RCONTROL)
		|| (VK == VK_LMENU) || (VK == VK_RMENU)
		|| (VK == VK_LWIN) || (VK == VK_RWIN))
	{
		return (1 << 24);
	}

	return 0;
}

#define MAP_VKEY(__FKEY, __VK) if (Key == EKeys::__FKEY) { return __VK; }

int GetVirtualKeyCode(const FKey& Key)
{
	MAP_VKEY(BackSpace, VK_BACK);
	MAP_VKEY(Tab, VK_TAB);
	MAP_VKEY(Enter, VK_RETURN);
	MAP_VKEY(Pause, VK_PAUSE);
	MAP_VKEY(CapsLock, VK_CAPITAL);
	MAP_VKEY(Escape, VK_ESCAPE);
	MAP_VKEY(PageUp, VK_PRIOR);
	MAP_VKEY(PageDown, VK_NEXT);
	MAP_VKEY(End, VK_END);
	MAP_VKEY(Home, VK_HOME);
	MAP_VKEY(Left, VK_LEFT);
	MAP_VKEY(Up, VK_UP);
	MAP_VKEY(Right, VK_RIGHT);
	MAP_VKEY(Down, VK_DOWN);
	MAP_VKEY(Insert, VK_INSERT);
	MAP_VKEY(Delete, VK_DELETE);
	MAP_VKEY(NumPadZero, VK_NUMPAD0);
	MAP_VKEY(NumPadOne, VK_NUMPAD1);
	MAP_VKEY(NumPadTwo, VK_NUMPAD2);
	MAP_VKEY(NumPadThree, VK_NUMPAD3);
	MAP_VKEY(NumPadFour, VK_NUMPAD4);
	MAP_VKEY(NumPadFive, VK_NUMPAD5);
	MAP_VKEY(NumPadSix, VK_NUMPAD6);
	MAP_VKEY(NumPadSeven, VK_NUMPAD7);
	MAP_VKEY(NumPadEight, VK_NUMPAD8);
	MAP_VKEY(NumPadNine, VK_NUMPAD9);
	MAP_VKEY(Multiply, VK_MULTIPLY);
	MAP_VKEY(Add, VK_ADD);
	MAP_VKEY(Subtract, VK_SUBTRACT);
	MAP_VKEY(Decimal, VK_DECIMAL);
	MAP_VKEY(Divide, VK_DIVIDE);
	MAP_VKEY(F1, VK_F1);
	MAP_VKEY(F2, VK_F2);
	MAP_VKEY(F3, VK_F3);
	MAP_VKEY(F4, VK_F4);
	MAP_VKEY(F5, VK_F5);
	MAP_VKEY(F6, VK_F6);
	MAP_VKEY(F7, VK_F7);
	MAP_VKEY(F8, VK_F8);
	MAP_VKEY(F9, VK_F9);
	MAP_VKEY(F10, VK_F10);
	MAP_VKEY(F11, VK_F11);
	MAP_VKEY(F12, VK_F12);
	MAP_VKEY(NumLock, VK_NUMLOCK);
	MAP_VKEY(LeftShift, VK_LSHIFT);
	MAP_VKEY(RightShift, VK_LSHIFT);
	MAP_VKEY(LeftControl, VK_LSHIFT);
	MAP_VKEY(RightControl, VK_LSHIFT);
	MAP_VKEY(LeftAlt, VK_LSHIFT);
	MAP_VKEY(RightAlt, VK_LSHIFT);
	MAP_VKEY(SpaceBar, VK_SPACE);
	MAP_VKEY(Zero, 0x30);
	MAP_VKEY(One, 0x31);
	MAP_VKEY(Two, 0x32);
	MAP_VKEY(Three, 0x33);
	MAP_VKEY(Four, 0x34);
	MAP_VKEY(Five, 0x35);
	MAP_VKEY(Six, 0x36);
	MAP_VKEY(Seven, 0x37);
	MAP_VKEY(Eight, 0x38);
	MAP_VKEY(Nine, 0x39);
	MAP_VKEY(A, 0x41)
	MAP_VKEY(B, 0x42)
	MAP_VKEY(C, 0x43)
	MAP_VKEY(D, 0x44)
	MAP_VKEY(E, 0x45)
	MAP_VKEY(F, 0x46)
	MAP_VKEY(G, 0x47)
	MAP_VKEY(H, 0x48)
	MAP_VKEY(I, 0x49)
	MAP_VKEY(J, 0x4A)
	MAP_VKEY(K, 0x4B)
	MAP_VKEY(L, 0x4C)
	MAP_VKEY(M, 0x4D)
	MAP_VKEY(N, 0x4E)
	MAP_VKEY(O, 0x4F)
	MAP_VKEY(P, 0x50)
	MAP_VKEY(Q, 0x51)
	MAP_VKEY(R, 0x52)
	MAP_VKEY(S, 0x53)
	MAP_VKEY(T, 0x54)
	MAP_VKEY(U, 0x55)
	MAP_VKEY(V, 0x56)
	MAP_VKEY(W, 0x57)
	MAP_VKEY(X, 0x58)
	MAP_VKEY(Y, 0x59)
	MAP_VKEY(Z, 0x5A)
	MAP_VKEY(Semicolon, VK_OEM_1);
	MAP_VKEY(Equals, VK_OEM_PLUS);
	MAP_VKEY(Comma, VK_OEM_COMMA);
	MAP_VKEY(Underscore, VK_OEM_MINUS);
	MAP_VKEY(Period, VK_OEM_PERIOD);
	MAP_VKEY(Slash, VK_OEM_2);
	MAP_VKEY(Tilde, VK_OEM_3);
	MAP_VKEY(LeftBracket, VK_OEM_4);
	MAP_VKEY(Backslash, VK_OEM_5);
	MAP_VKEY(RightBracket, VK_OEM_6);
	MAP_VKEY(Quote, VK_OEM_7);
	return 0;
}

int GetCurrentModifiers(const FInputEvent& KeyEvent, const FKey& Key)
{
	int Modifiers = 0;

	if (KeyEvent.IsAltDown())
	{
		Modifiers |= CEFRT_AltDown;
	}

	if (KeyEvent.IsControlDown())
	{
		Modifiers |= CEFRT_ControlDown;
	}

	if (KeyEvent.IsShiftDown())
	{
		Modifiers |= CEFRT_ShiftDown;
	}

	if ((Key == EKeys::LeftShift) ||
		(Key == EKeys::LeftAlt) ||
		(Key == EKeys::LeftControl))
	{
		Modifiers |= CEFRT_IsLeft;
	}

	if ((Key == EKeys::RightShift) ||
		(Key == EKeys::LeftAlt) ||
		(Key == EKeys::LeftControl))
	{
		Modifiers |= CEFRT_IsLeft;
	}

	if ((Key == EKeys::NumPadZero) ||
		(Key == EKeys::NumPadOne) ||
		(Key == EKeys::NumPadTwo) ||
		(Key == EKeys::NumPadThree) ||
		(Key == EKeys::NumPadFour) ||
		(Key == EKeys::NumPadFive) ||
		(Key == EKeys::NumPadSix) ||
		(Key == EKeys::NumPadSeven) ||
		(Key == EKeys::NumPadEight) ||
		(Key == EKeys::NumPadNine) ||
		(Key == EKeys::Multiply) ||
		(Key == EKeys::Add) ||
		(Key == EKeys::Subtract) ||
		(Key == EKeys::Decimal) ||
		(Key == EKeys::Divide))
	{
		Modifiers |= CEFRT_IsKeyPad;
	}

	if (::GetKeyState(VK_NUMLOCK) & 1)
	{
		Modifiers |= CEFRT_NumLockOn;
	}

	if (::GetKeyState(VK_CAPITAL) & 1)
	{
		Modifiers |= CEFRT_CapsLockOn;
	}

	return Modifiers;
}

void DispatchSlateKey(ICefWebView* WebView, const FKeyEvent& KeyEvent, bool Pressed)
{
	CefRuntimeKeyEvent Event;
	FMemory::MemZero(Event);
	Event.Type = Pressed ? CEFRT_RawKeyDown : CEFRT_KeyUp;

	const FKey Key = KeyEvent.GetKey();

	int VirtualKey = GetVirtualKeyCode(Key);
	int ScanCode = MapVirtualKey(VirtualKey, MAPVK_VK_TO_VSC);
	Event.WindowsKeyCode = VirtualKey;
	Event.NativeKeyCode = (ScanCode << 16) | ExtendedKeyBits(VirtualKey);
	Event.NativeKeyCode |= Pressed ? 0 : (1 << 30);
	Event.Modifiers = GetCurrentModifiers(KeyEvent, Key);
	WebView->SendKeyEvent(Event);
}

void DispatchSlateChar(ICefWebView* WebView, const FCharacterEvent& KeyEvent)
{
	CefRuntimeKeyEvent Event;
	FMemory::MemZero(Event);
	Event.Type = CEFRT_Char;

	/*
	const FKey Key = KeyEvent.GetKey();

	int VirtualKey = GetVirtualKeyCode(Key);
	int ScanCode = MapVirtualKey(VirtualKey, MAPVK_VK_TO_VSC);
	if (ScanCode == 0)
	{
		return;
	}
	Event.NativeKeyCode = (ScanCode << 16) | ExtendedKeyBits(__VK);
	*/

	Event.WindowsKeyCode = KeyEvent.GetCharacter();
	Event.Modifiers = GetCurrentModifiers(KeyEvent, EKeys::Invalid);
	WebView->SendKeyEvent(Event);
}

#endif
}

#define REPLY_IF_NOT_READY(...) if (!HUDElement.IsValid() || !HUDOwner.IsValid() || !HUDElement->WebView.IsValid() || !HUDElement->WebView->GetBrowser()) return __VA_ARGS__
#define HITTEST(...)
//#define HITTEST(...) if (!SWidget::UseLegacyHittest() && !FSlateApplication::Get().GetMouseCaptor().IsValid() && !const_cast<SRadiantWebViewHUDElement*>(this)->OnHitTest(MyGeometry, MouseEvent.GetScreenSpacePosition())) return __VA_ARGS__

void SRadiantWebViewHUDElement::Construct(const FArguments& InArgs)
{
	HUDOwner = InArgs._HUDOwner;
	HUDElement = InArgs._HUDElement;
	MouseCaptured = false;
}

void SRadiantWebViewHUDElement::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SRadiantWebViewHUDElement::GetMouseState(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent, CefRuntimeMouseEvent &OutMouseEvent)
{
	FVector2D MousePosition = AbsoluteToLocal(InGeometry, InPointerEvent.GetScreenSpacePosition());
	FIntPoint ViewSize = HUDElement->WebView->GetSize();
	FVector2D Scale = FVector2D(ViewSize.X / ScreenSize.X, ViewSize.Y / ScreenSize.Y);
	MousePosition *= Scale;

	OutMouseEvent.X = FMath::FloorToInt(MousePosition.X + 0.5f);
	OutMouseEvent.Y = FMath::FloorToInt(MousePosition.Y + 0.5f);
	OutMouseEvent.Modifiers = 0;

	if (InPointerEvent.IsAltDown())
	{
		OutMouseEvent.Modifiers |= CEFRT_AltDown;
	}

	if (InPointerEvent.IsControlDown())
	{
		OutMouseEvent.Modifiers |= CEFRT_ControlDown;
	}

	if (InPointerEvent.IsShiftDown())
	{
		OutMouseEvent.Modifiers |= CEFRT_ShiftDown;
	}

	if (InPointerEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		OutMouseEvent.Modifiers |= CEFRT_LeftMouseButton;
	}

	if (InPointerEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		OutMouseEvent.Modifiers |= CEFRT_RightMouseButton;
	}

	if (InPointerEvent.IsMouseButtonDown(EKeys::MiddleMouseButton))
	{
		OutMouseEvent.Modifiers |= CEFRT_MiddleMouseButton;
	}
}

FReply SRadiantWebViewHUDElement::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	REPLY_IF_NOT_READY(FReply::Handled());
	HITTEST(FReply::Unhandled());

	FKey MouseKey = MouseEvent.GetEffectingButton();

	if (MouseKey == EKeys::ThumbMouseButton)
	{
		if (HUDElement->bMouseThumbNavigate)
		{
			HUDElement->WebView->NavigateBackward();
		}

		return FReply::Handled();
	}

	if (MouseKey == EKeys::ThumbMouseButton2)
	{
		if (HUDElement->bMouseThumbNavigate)
		{
			HUDElement->WebView->NavigateForward();
		}

		return FReply::Handled();
	}

	ECefRuntimeMouseButton CefMouseButton = GetMouseButton(MouseEvent);
	CefRuntimeMouseEvent Event;

	GetMouseState(MyGeometry, MouseEvent, Event);

	HUDElement->WebView->GetBrowser()->SendMouseClickEvent(Event, CefMouseButton, false, 1);

	//UE_LOG(RadiantUILog, Log, TEXT("MouseDown"));

	MouseCaptured = true;
	return FReply::Handled().PreventThrottling().CaptureMouse(SharedThis(this));
}

FReply SRadiantWebViewHUDElement::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	REPLY_IF_NOT_READY(FReply::Handled());
	HITTEST(FReply::Unhandled());

	FKey MouseKey = MouseEvent.GetEffectingButton();

	if ((MouseKey == EKeys::ThumbMouseButton) || (MouseKey == EKeys::ThumbMouseButton2))
	{
		return FReply::Handled();
	}

	ECefRuntimeMouseButton CefMouseButton = GetMouseButton(MouseEvent);
	CefRuntimeMouseEvent Event;

	GetMouseState(MyGeometry, MouseEvent, Event);

	HUDElement->WebView->GetBrowser()->SendMouseClickEvent(Event, CefMouseButton, true, 1);

	if (MouseCaptured &&
		!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) &&
		!MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) &&
		!MouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton))
	{
		MouseCaptured = false;
		return FReply::Handled().ReleaseMouseCapture();
	}

	//UE_LOG(RadiantUILog, Log, TEXT("MouseUp"));

	return FReply::Handled();
}

FReply SRadiantWebViewHUDElement::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	REPLY_IF_NOT_READY(FReply::Handled());
	HITTEST(FReply::Unhandled());

	if (MouseEvent.GetLastScreenSpacePosition() == MouseEvent.GetScreenSpacePosition())
	{
		return FReply::Handled();
	}

	CefRuntimeMouseEvent Event;
	GetMouseState(MyGeometry, MouseEvent, Event);
	HUDElement->WebView->GetBrowser()->SendMouseMoveEvent(Event, false);

	//UE_LOG(RadiantUILog, Log, TEXT("MouseMove %d x %d"), Event.X, Event.Y);

	return FReply::Handled();
}

FReply SRadiantWebViewHUDElement::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	REPLY_IF_NOT_READY(FReply::Handled());
	HITTEST(FReply::Unhandled());

	if (HUDElement->WebView.IsValid())
	{
		CefRuntimeMouseEvent Event;
		GetMouseState(MyGeometry, MouseEvent, Event);
		HUDElement->WebView->GetBrowser()->SendMouseWheelEvent(Event, 0, FMath::FloorToInt(MouseEvent.GetWheelDelta()*80.f + 05.f));
	}
	return FReply::Handled();
}

FReply SRadiantWebViewHUDElement::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	REPLY_IF_NOT_READY(FReply::Handled());
	HITTEST(FReply::Unhandled());

	FKey MouseKey = MouseEvent.GetEffectingButton();

	if ((MouseKey == EKeys::ThumbMouseButton) || (MouseKey == EKeys::ThumbMouseButton2))
	{
		return FReply::Handled();
	}

	ECefRuntimeMouseButton CefMouseButton = GetMouseButton(MouseEvent);
	CefRuntimeMouseEvent Event;

	GetMouseState(MyGeometry, MouseEvent, Event);

	HUDElement->WebView->GetBrowser()->SendMouseClickEvent(Event, CefMouseButton, false, 2);
	return FReply::Handled();
}

FCursorReply SRadiantWebViewHUDElement::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	REPLY_IF_NOT_READY(FCursorReply::Unhandled());
	HITTEST(FCursorReply::Unhandled());

	if (HUDElement->WebView->GetMouseCursor() == ERadiantWebViewCursor::Hover)
	{
		return FCursorReply::Cursor(EMouseCursor::Hand);
	}

	return FCursorReply::Cursor(EMouseCursor::Default);
}

FReply SRadiantWebViewHUDElement::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent)
{
	REPLY_IF_NOT_READY(FReply::Unhandled());
	if (HUDElement->InputMode != ERadiantHUDElementInputMode::MouseAndKeyboard)
	{
		return FReply::Unhandled();
	}
	DispatchSlateKey(HUDElement->WebView->GetBrowser(), InKeyboardEvent, true);
	return FReply::Handled();
}

FReply SRadiantWebViewHUDElement::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent)
{
	REPLY_IF_NOT_READY(FReply::Unhandled());
	if (HUDElement->InputMode != ERadiantHUDElementInputMode::MouseAndKeyboard)
	{
		return FReply::Unhandled();
	}
	DispatchSlateKey(HUDElement->WebView->GetBrowser(), InKeyboardEvent, false);
	return SCompoundWidget::OnKeyUp(MyGeometry, InKeyboardEvent);
}

FReply SRadiantWebViewHUDElement::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	REPLY_IF_NOT_READY(FReply::Unhandled());
	if (HUDElement->InputMode != ERadiantHUDElementInputMode::MouseAndKeyboard)
	{
		return FReply::Unhandled();
	}
	DispatchSlateChar(HUDElement->WebView->GetBrowser(), InCharacterEvent);
	return SCompoundWidget::OnKeyChar(MyGeometry, InCharacterEvent);
}

bool SRadiantWebViewHUDElement::SupportsKeyboardFocus() const
{
	return true;
}

FReply SRadiantWebViewHUDElement::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	REPLY_IF_NOT_READY(FReply::Handled());
	HUDElement->WebView->GetBrowser()->SendFocusEvent(true);
	return FReply::Handled();
}

void SRadiantWebViewHUDElement::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	REPLY_IF_NOT_READY();
	HUDElement->WebView->GetBrowser()->SendFocusEvent(false);
}

/*
bool SRadiantWebViewHUDElement::OnHitTest(const FGeometry& MyGeometry, FVector2D InAbsoluteCursorPosition)
{
	REPLY_IF_NOT_READY(false);

	if (!HUDElement->bVisible)
	{
		return false;
	}

	if (HUDElement->HitTest == ERadiantHUDElementHitTest::None)
	{
		return false;
	}

	FVector2D LocalCursorPosition = AbsoluteToLocal(MyGeometry, InAbsoluteCursorPosition);

	if ((LocalCursorPosition.X < 0) || (LocalCursorPosition.X >= ScreenSize.X) ||
		(LocalCursorPosition.Y < 0) || (LocalCursorPosition.Y >= ScreenSize.Y))
	{
		return false;
	}

	if (HUDElement->HitTest == ERadiantHUDElementHitTest::Alpha)
	{
		int X = FMath::FloorToInt(LocalCursorPosition.X + 0.5f);
		int Y = FMath::FloorToInt(LocalCursorPosition.Y + 0.5f);

		return (HUDElement->WebView.IsValid()) ? (HUDElement->WebView->GetPixelAlpha(X, Y) > 0) : false;
	}

	return true;
}
*/

FVector2D SRadiantWebViewHUDElement::AbsoluteToLocal(const FGeometry& MyGeometry, const FVector2D AbsolutePosition)
{
	FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(AbsolutePosition) * MyGeometry.Scale;
	return LocalPosition - ScreenPosition;
}

URadiantWebViewHUDElement::URadiantWebViewHUDElement(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bVisible = true;

	Position = FVector2D::ZeroVector;
	Size = FVector2D(1, 1);
	bAutoMatchViewportResolution = true;
	ViewportResolutionFactor = FVector2D(1, 1);
	bMouseThumbNavigate = false;

	InputMode = ERadiantHUDElementInputMode::MouseOnly;
	HitTest = ERadiantHUDElementHitTest::Alpha;
	DefaultSettings.RefreshRate = 0.0f;
	DefaultSettings.bTransparentRendering = true;
	DefaultSettings.bProjectedCursor = false;
	World = nullptr;

}

void URadiantWebViewHUDElement::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		WebView = MakeShareable(new FRadiantWebView(DefaultSettings));
		WebView->OnExecuteJSHook.AddUObject(this, &URadiantWebViewHUDElement::OnExecuteJSHook);
	}
}

UWorld* URadiantWebViewHUDElement::GetWorld() const
{
	return World;
}

void URadiantWebViewHUDElement::SetVisible(bool IsVisible)
{
	bVisible = IsVisible;
	SetSlateVisibility();
}

void URadiantWebViewHUDElement::SetHitTest(TEnumAsByte<ERadiantHUDElementHitTest::Type> InHitTest)
{
	HitTest = InHitTest;
	SetSlateVisibility();
}

void URadiantWebViewHUDElement::SetInputMode(TEnumAsByte<ERadiantHUDElementInputMode::Type> InInputMode)
{
	InputMode = InInputMode;
	SetSlateVisibility();
}

bool URadiantWebViewHUDElement::CanNavigateForward()
{
	check(WebView.IsValid());
	return WebView->CanNavigateForward();
}

bool URadiantWebViewHUDElement::CanNavigateBackward()
{
	check(WebView.IsValid()); 
	return WebView->CanNavigateBackward();
}

void URadiantWebViewHUDElement::NavigateForward()
{
	check(WebView.IsValid());
	WebView->NavigateForward();
}

void URadiantWebViewHUDElement::NavigateBackward()
{
	check(WebView.IsValid());
	WebView->NavigateBackward();
}

void URadiantWebViewHUDElement::LoadURL(const FString& InURL)
{
	check(WebView.IsValid());
	WebView->LoadURL(InURL);
}

FString URadiantWebViewHUDElement::GetURL()
{
	check(WebView.IsValid());
	return WebView->GetURL();
}

void URadiantWebViewHUDElement::SetRefreshRate(float InFramesPerSecond)
{
	check(WebView.IsValid());
	WebView->SetRefreshRate(InFramesPerSecond);
}

float URadiantWebViewHUDElement::GetRefreshRate()
{
	check(WebView.IsValid());
	return WebView->GetRefreshRate();
}

void URadiantWebViewHUDElement::SetSlateVisibility()
{
	if (!bVisible)
	{
		SWidget->SetVisibility(EVisibility::Hidden);
	}
	else
	{
		SWidget->SetVisibility((HitTest == ERadiantHUDElementHitTest::None) ? EVisibility::HitTestInvisible : EVisibility::Visible);
	}
}

void URadiantWebViewHUDElement::CallJavaScriptFunction(const FString& HookName, UObject* Parameters)
{
	if (!HookName.IsEmpty() && Parameters && WebView.IsValid())
	{
		ICefRuntimeVariantList* Arguments = FJavaScriptHelper::CreateVariantList(Parameters->GetClass(), Parameters, WebView->GetVariantFactory());
		FTCHARToUTF8 Convert(*HookName);
		WebView->CallJavaScriptFunction(Convert.Get(), Arguments);
		if (Arguments)
		{
			Arguments->Release();
		}
	}
}

void URadiantWebViewHUDElement::OnExecuteJSHook(const FString& HookName, ICefRuntimeVariantList* Arguments)
{
	FJavaScriptHelper::ExecuteHook(this, HookName, Arguments);
}

TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface> URadiantWebViewHUDElement::GetJavaScriptCallContext()
{
	return TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface>(this);
}

void URadiantWebViewHUDElement::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	static const int ArchiveVersion = 1;

	int Version = ArchiveVersion;

	Ar << Version;
}

