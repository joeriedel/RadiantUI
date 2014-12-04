// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

#if PLATFORM_WINDOWS
#include <windows.h>
#endif

enum
{
	LeftShiftFlag = 0x1,
	RightShiftFlag = 0x2,
	LeftControlFlag = 0x4,
	RightControlFlag = 0x8,
	LeftAltFlag = 0x10,
	RightAltFlag = 0x20
};

inline bool IsMouseKey(const FKey& InKey)
{
	return (InKey == EKeys::LeftMouseButton) || (InKey == EKeys::RightMouseButton)
		|| (InKey == EKeys::MiddleMouseButton) || (InKey == EKeys::ThumbMouseButton)
		|| (InKey == EKeys::ThumbMouseButton2) || (InKey == EKeys::MouseScrollDown)
		|| (InKey == EKeys::MouseScrollUp);
}

#if PLATFORM_WINDOWS

inline int ExtendedKeyBits(int VK)
{
	if ((VK == VK_LSHIFT) || (VK == VK_RSHIFT)
		|| (VK == VK_LCONTROL) || (VK == VK_RCONTROL)
		|| (VK == VK_LMENU) || (VK == VK_RMENU)
		|| (VK == VK_LWIN) || (VK == VK_RWIN))
	{
		return (1<<24);
	}

	return 0;
}

#define DISPATCH_KEYEX(__FKEY, __VK, __Modifiers, __ModifierFlag, __ModifierToggle, __ModifierEx) \
	if (EKeys::##__FKEY == InKey) \
	{ \
		if (InEvent == IE_Pressed) \
		{\
			ModifierKeyState |= __ModifierFlag; \
			ModifierKeyExState |= __ModifierEx; \
		} \
		else \
		{ \
			ModifierKeyState &= ~__ModifierFlag; \
			ModifierKeyExState &= ~__ModifierEx; \
		} \
		if (InEvent == IE_Pressed) \
		{\
			if (ModifierKeyState & __ModifierToggle) \
			{ \
				ModifierKeyState &= ~__ModifierToggle; \
			} \
						else \
			{ \
				ModifierKeyState |= __ModifierToggle; \
			} \
		} \
		CefRuntimeKeyEvent Event; \
		FMemory::MemZero(Event); \
		Event.Type = (InEvent == IE_Pressed) ? CEFRT_RawKeyDown : CEFRT_KeyUp; \
		Event.WindowsKeyCode = __VK; \
		int ScanCode = MapVirtualKey(__VK, MAPVK_VK_TO_VSC); \
		Event.NativeKeyCode = (ScanCode << 16) | ExtendedKeyBits(__VK); \
		Event.NativeKeyCode |= (InEvent == IE_Pressed) ? 0 : (1 << 30); \
		Event.Modifiers = __Modifiers | ModifierKeyState; \
		Browser->SendKeyEvent(Event); \
		return; \
	}

#define DISPATCH_CHAREX(__FKEY, __VK, __Modifiers) \
	if (EKeys::##__FKEY == InKey) \
	{ \
		if (IE_Pressed == InEvent) \
		{\
			CefRuntimeKeyEvent Event; \
			FMemory::MemZero(Event); \
			BYTE KeyState[256]; \
			FMemory::Memzero(KeyState, 256); \
			KeyState[VK_CAPITAL] = (ModifierKeyState&CEFRT_CapsLockOn) ? 0xff : 0; \
			KeyState[VK_SHIFT] = (ModifierKeyState&CEFRT_ShiftDown) ? 0xff : 0; \
			KeyState[VK_LSHIFT] = (ModifierKeyExState&LeftShiftFlag) ? 0xff : 0; \
			KeyState[VK_RSHIFT] = (ModifierKeyExState&RightShiftFlag) ? 0xff : 0; \
			KeyState[VK_CONTROL] = (ModifierKeyState&CEFRT_ControlDown) ? 0xff : 0; \
			KeyState[VK_LCONTROL] = (ModifierKeyExState&LeftControlFlag) ? 0xff : 0; \
			KeyState[VK_RCONTROL] = (ModifierKeyExState&RightControlFlag) ? 0xff : 0; \
			KeyState[VK_MENU] = (ModifierKeyState&CEFRT_AltDown) ? 0xff : 0; \
			KeyState[VK_LMENU] = (ModifierKeyExState&LeftAltFlag) ? 0xff : 0; \
			KeyState[VK_RMENU] = (ModifierKeyExState&RightAltFlag) ? 0xff : 0; \
			Event.Type = CEFRT_Char; \
			int ScanCode = MapVirtualKey(__VK, MAPVK_VK_TO_VSC); \
			if (ScanCode == 0) \
			{ \
				return; \
			} \
			if (ToUnicode(__VK, ScanCode, KeyState, (LPWSTR)&Event.WindowsKeyCode, 1, 0) != 1)\
			{\
				return;\
			}\
			Event.NativeKeyCode = (ScanCode << 16) | ExtendedKeyBits(__VK); \
			Event.Modifiers = __Modifiers | ModifierKeyState; \
			Browser->SendKeyEvent(Event); \
		} \
		return; \
	}

#define DISPATCH_KEY(__FKEY, __VK) DISPATCH_KEYEX(__FKEY, __VK, 0, 0, 0, 0)
#define DISPATCH_NUMPAD(__FKEY, __VK) DISPATCH_KEYEX(__FKEY, __VK, CEFRT_IsKeyPad, 0, 0, 0)
#define DISPATCH_CHAR(__FKEY, __VK) DISPATCH_CHAREX(__FKEY, __VK, 0)

void ARadiantWebViewActor::DispatchPlatformKeyEvent(const FKey& InKey, EInputEvent InEvent)
{
	ICefWebView* Browser = WebViewRenderComponent->WebView->GetBrowser();

	DISPATCH_KEY(BackSpace, VK_BACK);
	DISPATCH_KEY(Tab, VK_TAB);
	DISPATCH_KEY(Enter, VK_RETURN);
	DISPATCH_KEY(Pause, VK_PAUSE);
	DISPATCH_KEYEX(CapsLock, VK_CAPITAL, 0, 0, CEFRT_CapsLockOn, 0);
	DISPATCH_KEY(Escape, VK_ESCAPE);
	DISPATCH_KEY(PageUp, VK_PRIOR);
	DISPATCH_KEY(PageDown, VK_NEXT);
	DISPATCH_KEY(End, VK_END);
	DISPATCH_KEY(Home, VK_HOME);
	DISPATCH_KEY(Left, VK_LEFT);
	DISPATCH_KEY(Up, VK_UP);
	DISPATCH_KEY(Right, VK_RIGHT);
	DISPATCH_KEY(Down, VK_DOWN);
	DISPATCH_KEY(Insert, VK_INSERT);
	DISPATCH_KEY(Delete, VK_DELETE);
	DISPATCH_NUMPAD(NumPadZero, VK_NUMPAD0);
	DISPATCH_NUMPAD(NumPadOne, VK_NUMPAD1);
	DISPATCH_NUMPAD(NumPadTwo, VK_NUMPAD2);
	DISPATCH_NUMPAD(NumPadThree, VK_NUMPAD3);
	DISPATCH_NUMPAD(NumPadFour, VK_NUMPAD4);
	DISPATCH_NUMPAD(NumPadFive, VK_NUMPAD5);
	DISPATCH_NUMPAD(NumPadSix, VK_NUMPAD6);
	DISPATCH_NUMPAD(NumPadSeven, VK_NUMPAD7);
	DISPATCH_NUMPAD(NumPadEight, VK_NUMPAD8);
	DISPATCH_NUMPAD(NumPadNine, VK_NUMPAD9);
	DISPATCH_NUMPAD(Multiply, VK_MULTIPLY);
	DISPATCH_NUMPAD(Add, VK_ADD);
	DISPATCH_NUMPAD(Subtract, VK_SUBTRACT);
	DISPATCH_NUMPAD(Decimal, VK_DECIMAL);
	DISPATCH_NUMPAD(Divide, VK_DIVIDE);

	DISPATCH_KEY(F1, VK_F1);
	DISPATCH_KEY(F2, VK_F2);
	DISPATCH_KEY(F3, VK_F3);
	DISPATCH_KEY(F4, VK_F4);
	DISPATCH_KEY(F5, VK_F5);
	DISPATCH_KEY(F6, VK_F6);
	DISPATCH_KEY(F7, VK_F7);
	DISPATCH_KEY(F8, VK_F8);
	DISPATCH_KEY(F9, VK_F9);
	DISPATCH_KEY(F10, VK_F10);
	DISPATCH_KEY(F11, VK_F11);
	DISPATCH_KEY(F12, VK_F12);

	DISPATCH_KEYEX(NumLock, VK_NUMLOCK, 0, 0, CEFRT_NumLockOn, 0);
	DISPATCH_KEYEX(LeftShift, VK_LSHIFT, CEFRT_IsLeft, CEFRT_ShiftDown, 0, LeftShiftFlag);
	DISPATCH_KEYEX(RightShift, VK_RSHIFT, CEFRT_IsRight, CEFRT_ShiftDown, 0, RightShiftFlag);
	DISPATCH_KEYEX(LeftControl, VK_LCONTROL, CEFRT_IsLeft, CEFRT_ControlDown, 0, LeftControlFlag);
	DISPATCH_KEYEX(RightControl, VK_RCONTROL, CEFRT_IsRight, CEFRT_ControlDown, 0, RightControlFlag);
	DISPATCH_KEYEX(LeftAlt, VK_LMENU, CEFRT_IsLeft, CEFRT_AltDown, 0, LeftAltFlag);
	DISPATCH_KEYEX(RightAlt, VK_RMENU, CEFRT_IsRight, CEFRT_AltDown, 0, RightAltFlag);

	DISPATCH_CHAR(SpaceBar, VK_SPACE);
	DISPATCH_CHAR(Zero, 0x30);
	DISPATCH_CHAR(One, 0x31);
	DISPATCH_CHAR(Two, 0x32);
	DISPATCH_CHAR(Three, 0x33);
	DISPATCH_CHAR(Four, 0x34);
	DISPATCH_CHAR(Five, 0x35);
	DISPATCH_CHAR(Six, 0x36);
	DISPATCH_CHAR(Seven, 0x37);
	DISPATCH_CHAR(Eight, 0x38);
	DISPATCH_CHAR(Nine, 0x39);
	DISPATCH_CHAR(A, 0x41)
	DISPATCH_CHAR(B, 0x42)
	DISPATCH_CHAR(C, 0x43)
	DISPATCH_CHAR(D, 0x44)
	DISPATCH_CHAR(E, 0x45)
	DISPATCH_CHAR(F, 0x46)
	DISPATCH_CHAR(G, 0x47)
	DISPATCH_CHAR(H, 0x48)
	DISPATCH_CHAR(I, 0x49)
	DISPATCH_CHAR(J, 0x4A)
	DISPATCH_CHAR(K, 0x4B)
	DISPATCH_CHAR(L, 0x4C)
	DISPATCH_CHAR(M, 0x4D)
	DISPATCH_CHAR(N, 0x4E)
	DISPATCH_CHAR(O, 0x4F)
	DISPATCH_CHAR(P, 0x50)
	DISPATCH_CHAR(Q, 0x51)
	DISPATCH_CHAR(R, 0x52)
	DISPATCH_CHAR(S, 0x53)
	DISPATCH_CHAR(T, 0x54)
	DISPATCH_CHAR(U, 0x55)
	DISPATCH_CHAR(V, 0x56)
	DISPATCH_CHAR(W, 0x57)
	DISPATCH_CHAR(X, 0x58)
	DISPATCH_CHAR(Y, 0x59)
	DISPATCH_CHAR(Z, 0x5A)

	DISPATCH_CHAR(Semicolon, VK_OEM_1);
	DISPATCH_CHAR(Equals, VK_OEM_PLUS);
	DISPATCH_CHAR(Comma, VK_OEM_COMMA);
	DISPATCH_CHAR(Underscore, VK_OEM_MINUS);
	DISPATCH_CHAR(Period, VK_OEM_PERIOD);
	DISPATCH_CHAR(Slash, VK_OEM_2);
	DISPATCH_CHAR(Tilde, VK_OEM_3);
	DISPATCH_CHAR(LeftBracket, VK_OEM_4);
	DISPATCH_CHAR(Backslash, VK_OEM_5);
	DISPATCH_CHAR(RightBracket, VK_OEM_6);
	DISPATCH_CHAR(Quote, VK_OEM_7);

}
#endif

void ARadiantWebViewActor::DispatchKeyEvent(const FKey& InKey, EInputEvent InEvent)
{
	ICefWebView* Browser = WebViewRenderComponent->WebView->GetBrowser();

	if (!Browser)
	{
		return;
	}

	if (IsMouseKey(InKey))
	{
		if (InKey == EKeys::ThumbMouseButton)
		{
			if ((InEvent == IE_Pressed) && bMouseThumbNavigate)
			{
				if (Browser->CanGoBack())
				{
					Browser->GoBack();
				}
			}
			return;
		}
		else if (InKey == EKeys::ThumbMouseButton2)
		{
			if ((InEvent == IE_Pressed) && bMouseThumbNavigate)
			{
				if (Browser->CanGoForward())
				{
					Browser->GoForward();
				}
			}
			return;
		}

		CefRuntimeMouseEvent Event;
		WebViewInputComponent->GetMouseState(Event);

		if (InEvent == IE_Pressed)
		{
			if (InKey == EKeys::MouseScrollUp)
			{
				Browser->SendMouseWheelEvent(Event, 0, 80);
				return;
			}
			else if (InKey == EKeys::MouseScrollDown)
			{
				Browser->SendMouseWheelEvent(Event, 0, -80);
				return;
			}
		}

		ECefRuntimeMouseButton MouseButton;
		ECefRuntimeEventFlags MouseFlags;

		if (InKey == EKeys::LeftMouseButton)
		{
			MouseButton = CEFRT_MouseLeft;
			MouseFlags = CEFRT_LeftMouseButton;
		}
		else if (InKey == EKeys::MiddleMouseButton)
		{
			MouseButton = CEFRT_MouseMiddle;
			MouseFlags = CEFRT_MiddleMouseButton;
		}
		else if (InKey == EKeys::RightMouseButton)
		{
			MouseButton = CEFRT_MouseRight;
			MouseFlags = CEFRT_RightMouseButton;
		}
		else
		{
			return; // unhandled mouse event...
		}

		if (InEvent == IE_Pressed)
		{
			Event.Modifiers |= MouseFlags;
		}

		Browser->SendMouseClickEvent(Event, MouseButton, InEvent == IE_Released, 1);
		
		return;
	}

	DispatchPlatformKeyEvent(InKey, InEvent);
}