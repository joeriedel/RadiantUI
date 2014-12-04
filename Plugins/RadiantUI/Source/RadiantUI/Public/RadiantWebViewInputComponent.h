// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebViewInteractionMode.h"
#include "RadiantWebViewInputComponent.generated.h"

class ICefWebView;
struct CefRuntimeMouseEvent;
struct CefRuntimeKeyEvent;
class URadiantWebViewRenderComponent;
class URadiantWebViewInputMaskedActionList;

#define DECLARE_KEY_BIND(__KeyName) \
	void __KeyName##Pressed(); \
	void __KeyName##Released()

UCLASS(meta = (BlueprintSpawnableComponent))
class RADIANTUI_API URadiantWebViewInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URadiantWebViewInputComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, noclear, Instanced, Category = "Input|Actions", meta=(Tooltip="Actions listed here will be masked (not processed) by the player controller when an editable field is selected."))
	TSubclassOf<class URadiantWebViewInputMaskedActionList> MaskedActionListClass;

	UFUNCTION(BlueprintCallable, Category = WebView)
	void SetInteractionMode(TEnumAsByte<ERadiantWebViewInteractionMode::Type> InteractionMode);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FKeyEventDelegate, const FKey&, EInputEvent);

	FKeyEventDelegate KeyEventDelegate;

	// Begin UActorComponent interface.
	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// End UActorComponent interface.

	void BeginInteraction(APawn* InPawn);
	void EndInteraction();

	void GetMouseState(CefRuntimeMouseEvent& InEvent);

private:
	UPROPERTY(transient)
	APawn* InteractingPawn;

	UPROPERTY(transient)
	APlayerController* InteractingPC;
	
	UPROPERTY(transient)
	URadiantWebViewRenderComponent* RenderComponent;

	UPROPERTY(transient)
	UInputComponent *MouseInputComponent;

	UPROPERTY(transient)
	UInputComponent *MouseAndKeyInputComponent;

	UPROPERTY(transient)
	URadiantWebViewInputMaskedActionList* MaskedActionList;

	ERadiantWebViewInteractionMode::Type InteractionMode;

	bool bFocusEditableField;
	bool bCanFocusEditableField;

	void SetupInputComponents();
	void PushMouseInputComponent();
	void PushMouseKeyInputComponent();
	void PopInputComponents();

	void NullAxis(float);
	void NullAction();

	void BindMouseInput(UInputComponent* InComponent);
	void BindKeyboardInput(UInputComponent* InComponent);
	
	DECLARE_KEY_BIND(LeftMouseButton);
	DECLARE_KEY_BIND(RightMouseButton);
	DECLARE_KEY_BIND(ThumbMouseButton);
	DECLARE_KEY_BIND(ThumbMouseButton2);
	DECLARE_KEY_BIND(MouseScrollUp);
	DECLARE_KEY_BIND(MouseScrollDown);
	DECLARE_KEY_BIND(BackSpace);
	DECLARE_KEY_BIND(Tab);
	DECLARE_KEY_BIND(Enter);
	DECLARE_KEY_BIND(Pause);
	DECLARE_KEY_BIND(CapsLock);
	DECLARE_KEY_BIND(Escape);
	DECLARE_KEY_BIND(SpaceBar);
	DECLARE_KEY_BIND(PageUp);
	DECLARE_KEY_BIND(PageDown);
	DECLARE_KEY_BIND(End);
	DECLARE_KEY_BIND(Home);
	DECLARE_KEY_BIND(Left);
	DECLARE_KEY_BIND(Up);
	DECLARE_KEY_BIND(Right);
	DECLARE_KEY_BIND(Down);
	DECLARE_KEY_BIND(Insert);
	DECLARE_KEY_BIND(Delete);
	DECLARE_KEY_BIND(Zero);
	DECLARE_KEY_BIND(One);
	DECLARE_KEY_BIND(Two);
	DECLARE_KEY_BIND(Three);
	DECLARE_KEY_BIND(Four);
	DECLARE_KEY_BIND(Five);
	DECLARE_KEY_BIND(Six);
	DECLARE_KEY_BIND(Seven);
	DECLARE_KEY_BIND(Eight);
	DECLARE_KEY_BIND(Nine);
	DECLARE_KEY_BIND(A);
	DECLARE_KEY_BIND(B);
	DECLARE_KEY_BIND(C);
	DECLARE_KEY_BIND(D);
	DECLARE_KEY_BIND(E);
	DECLARE_KEY_BIND(F);
	DECLARE_KEY_BIND(G);
	DECLARE_KEY_BIND(H);
	DECLARE_KEY_BIND(I);
	DECLARE_KEY_BIND(J);
	DECLARE_KEY_BIND(K);
	DECLARE_KEY_BIND(L);
	DECLARE_KEY_BIND(M);
	DECLARE_KEY_BIND(N);
	DECLARE_KEY_BIND(O);
	DECLARE_KEY_BIND(P);
	DECLARE_KEY_BIND(Q);
	DECLARE_KEY_BIND(R);
	DECLARE_KEY_BIND(S);
	DECLARE_KEY_BIND(T);
	DECLARE_KEY_BIND(U);
	DECLARE_KEY_BIND(V);
	DECLARE_KEY_BIND(W);
	DECLARE_KEY_BIND(X);
	DECLARE_KEY_BIND(Y);
	DECLARE_KEY_BIND(Z);
	DECLARE_KEY_BIND(NumPadZero);
	DECLARE_KEY_BIND(NumPadOne);
	DECLARE_KEY_BIND(NumPadTwo);
	DECLARE_KEY_BIND(NumPadThree);
	DECLARE_KEY_BIND(NumPadFour);
	DECLARE_KEY_BIND(NumPadFive);
	DECLARE_KEY_BIND(NumPadSix);
	DECLARE_KEY_BIND(NumPadSeven);
	DECLARE_KEY_BIND(NumPadEight);
	DECLARE_KEY_BIND(NumPadNine);
	DECLARE_KEY_BIND(Multiply);
	DECLARE_KEY_BIND(Add);
	DECLARE_KEY_BIND(Subtract);
	DECLARE_KEY_BIND(Decimal);
	DECLARE_KEY_BIND(Divide);
	DECLARE_KEY_BIND(F1);
	DECLARE_KEY_BIND(F2);
	DECLARE_KEY_BIND(F3);
	DECLARE_KEY_BIND(F4);
	DECLARE_KEY_BIND(F5);
	DECLARE_KEY_BIND(F6);
	DECLARE_KEY_BIND(F7);
	DECLARE_KEY_BIND(F8);
	DECLARE_KEY_BIND(F9);
	DECLARE_KEY_BIND(F10);
	DECLARE_KEY_BIND(F11);
	DECLARE_KEY_BIND(F12);
	DECLARE_KEY_BIND(NumLock);
	DECLARE_KEY_BIND(ScrollLock);
	DECLARE_KEY_BIND(LeftShift);
	DECLARE_KEY_BIND(RightShift);
	DECLARE_KEY_BIND(LeftControl);
	DECLARE_KEY_BIND(RightControl);
	DECLARE_KEY_BIND(LeftAlt);
	DECLARE_KEY_BIND(RightAlt);
	DECLARE_KEY_BIND(LeftCommand);
	DECLARE_KEY_BIND(RightCommand);
	DECLARE_KEY_BIND(Semicolon);
	DECLARE_KEY_BIND(Equals);
	DECLARE_KEY_BIND(Comma);
	DECLARE_KEY_BIND(Underscore);
	DECLARE_KEY_BIND(Period);
	DECLARE_KEY_BIND(Slash);
	DECLARE_KEY_BIND(Tilde);
	DECLARE_KEY_BIND(LeftBracket);
	DECLARE_KEY_BIND(Backslash);
	DECLARE_KEY_BIND(RightBracket);
	DECLARE_KEY_BIND(Quote);
};

#undef DECLARE_KEY_BIND
