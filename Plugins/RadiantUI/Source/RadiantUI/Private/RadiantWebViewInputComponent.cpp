// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "../../../CefRuntime/API/CefRuntimeAPI.h"

#define BIND_KEY(__KeyName) \
	InComponent->BindKey(EKeys::##__KeyName, IE_Pressed, this, &URadiantWebViewInputComponent::##__KeyName##Pressed); \
	InComponent->BindKey(EKeys::##__KeyName, IE_Released, this, &URadiantWebViewInputComponent::##__KeyName##Released)

#define IMPLEMENT_KEY_BIND(__KeyName) \
	void URadiantWebViewInputComponent::##__KeyName##Pressed() { KeyEventDelegate.Broadcast(EKeys::##__KeyName, IE_Pressed); } \
	void URadiantWebViewInputComponent::##__KeyName##Released() { KeyEventDelegate.Broadcast(EKeys::##__KeyName, IE_Released); }

URadiantWebViewInputComponent::URadiantWebViewInputComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	static ConstructorHelpers::FObjectFinder<UBlueprint> DefaultMaskedActionList(TEXT("/RadiantUI/DefaultMaskedActionList"));
#endif

	SetIsReplicated(true);
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bCanFocusEditableField = false;
	MaskedActionList = nullptr;
	InteractionMode = ERadiantWebViewInteractionMode::VisibleCursor;

#if WITH_EDITORONLY_DATA
	MaskedActionListClass = DefaultMaskedActionList.Object ? Cast<UClass>(DefaultMaskedActionList.Object->GeneratedClass) : nullptr;
#endif
}

void URadiantWebViewInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AActor* Owner = GetOwner();
	RenderComponent = Owner->FindComponentByClass<URadiantWebViewRenderComponent>();

	if (MaskedActionListClass)
	{
		MaskedActionList = Cast<URadiantWebViewInputMaskedActionList>(MaskedActionListClass->ClassDefaultObject);
	}
}

void URadiantWebViewInputComponent::SetInteractionMode(TEnumAsByte<ERadiantWebViewInteractionMode::Type> InInteractionMode)
{
	InteractionMode = InInteractionMode;
	PopInputComponents();

	if (InteractingPawn && (InteractionMode > ERadiantWebViewInteractionMode::VisibleCursor))
	{
		bool bShouldFocus = RenderComponent && RenderComponent->WebView->IsFocusingEditableField() && bCanFocusEditableField
			&& (InteractionMode > ERadiantWebViewInteractionMode::CursorAndButtons);

		if (bShouldFocus)
		{
			PushMouseKeyInputComponent();
		}
		else
		{
			PushMouseInputComponent();
		}
	}
}

void URadiantWebViewInputComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	bool bShouldFocus = RenderComponent && RenderComponent->WebView->IsFocusingEditableField() && bCanFocusEditableField
		&& (InteractionMode > ERadiantWebViewInteractionMode::CursorAndButtons);

	if (InteractingPawn && (bShouldFocus != bFocusEditableField))
	{
		bFocusEditableField = bShouldFocus;

		if (bShouldFocus)
		{
			PushMouseKeyInputComponent();
		}
		else if (InteractionMode > ERadiantWebViewInteractionMode::VisibleCursor)
		{
			PushMouseInputComponent();
		}
	}
}

void URadiantWebViewInputComponent::SetupInputComponents()
{
	if (MouseInputComponent == nullptr)
	{
		static const FName InputMouseComponentName(TEXT("WebViewInputMouseInputComponent0"));
		static const FName InputMouseAndKeyComponentName(TEXT("WebViewInputMouseAndKeyInputComponent0"));

		MouseInputComponent = ConstructObject<UInputComponent>(UInputComponent::StaticClass(), GetOwner(), InputMouseComponentName);
		MouseAndKeyInputComponent = ConstructObject<UInputComponent>(UInputComponent::StaticClass(), GetOwner(), InputMouseAndKeyComponentName);

		BindMouseInput(MouseInputComponent);
		BindMouseInput(MouseAndKeyInputComponent);
		BindKeyboardInput(MouseAndKeyInputComponent);
	}
}

void URadiantWebViewInputComponent::BindMouseInput(UInputComponent* InComponent)
{
	BIND_KEY(LeftMouseButton);
	BIND_KEY(ThumbMouseButton);
	BIND_KEY(ThumbMouseButton2);
	BIND_KEY(MouseScrollUp);
	BIND_KEY(MouseScrollDown);
}

void URadiantWebViewInputComponent::BindKeyboardInput(UInputComponent* InComponent)
{
	if (MaskedActionList)
	{
		for (auto It = MaskedActionList->ActionMappings.CreateConstIterator(); It; ++It)
		{
			InComponent->BindAction(*It, IE_Pressed, this, &URadiantWebViewInputComponent::NullAction);
			InComponent->BindAction(*It, IE_Released, this, &URadiantWebViewInputComponent::NullAction);
		}

		for (auto It = MaskedActionList->AxisMappings.CreateConstIterator(); It; ++It)
		{
			InComponent->BindAxis(*It, this, &URadiantWebViewInputComponent::NullAxis);
		}
	}

	BIND_KEY(BackSpace);
	BIND_KEY(Tab);
	BIND_KEY(Enter);
	BIND_KEY(Pause);
	BIND_KEY(CapsLock);
	BIND_KEY(Escape);
	BIND_KEY(SpaceBar);
	BIND_KEY(PageUp);
	BIND_KEY(PageDown);
	BIND_KEY(End);
	BIND_KEY(Home);
	BIND_KEY(Left);
	BIND_KEY(Up);
	BIND_KEY(Right);
	BIND_KEY(Down);
	BIND_KEY(Insert);
	BIND_KEY(Delete);
	BIND_KEY(Zero);
	BIND_KEY(One);
	BIND_KEY(Two);
	BIND_KEY(Three);
	BIND_KEY(Four);
	BIND_KEY(Five);
	BIND_KEY(Six);
	BIND_KEY(Seven);
	BIND_KEY(Eight);
	BIND_KEY(Nine);
	BIND_KEY(A);
	BIND_KEY(B);
	BIND_KEY(C);
	BIND_KEY(D);
	BIND_KEY(E);
	BIND_KEY(F);
	BIND_KEY(G);
	BIND_KEY(H);
	BIND_KEY(I);
	BIND_KEY(J);
	BIND_KEY(K);
	BIND_KEY(L);
	BIND_KEY(M);
	BIND_KEY(N);
	BIND_KEY(O);
	BIND_KEY(P);
	BIND_KEY(Q);
	BIND_KEY(R);
	BIND_KEY(S);
	BIND_KEY(T);
	BIND_KEY(U);
	BIND_KEY(V);
	BIND_KEY(W);
	BIND_KEY(X);
	BIND_KEY(Y);
	BIND_KEY(Z);
	BIND_KEY(NumPadZero);
	BIND_KEY(NumPadOne);
	BIND_KEY(NumPadTwo);
	BIND_KEY(NumPadThree);
	BIND_KEY(NumPadFour);
	BIND_KEY(NumPadFive);
	BIND_KEY(NumPadSix);
	BIND_KEY(NumPadSeven);
	BIND_KEY(NumPadEight);
	BIND_KEY(NumPadNine);
	BIND_KEY(Multiply);
	BIND_KEY(Add);
	BIND_KEY(Subtract);
	BIND_KEY(Decimal);
	BIND_KEY(Divide);
	/*BIND_KEY(F1);
	BIND_KEY(F2);
	BIND_KEY(F3);
	BIND_KEY(F4);
	BIND_KEY(F5);
	BIND_KEY(F6);
	BIND_KEY(F7);
	BIND_KEY(F8);
	BIND_KEY(F9);
	BIND_KEY(F10);
	BIND_KEY(F11);
	BIND_KEY(F12);*/
	BIND_KEY(NumLock);
	BIND_KEY(ScrollLock);
	BIND_KEY(LeftShift);
	BIND_KEY(RightShift);
	BIND_KEY(LeftControl);
	BIND_KEY(RightControl);
	BIND_KEY(LeftAlt);
	BIND_KEY(RightAlt);
	BIND_KEY(LeftCommand);
	BIND_KEY(RightCommand);
	BIND_KEY(Semicolon);
	BIND_KEY(Equals);
	BIND_KEY(Comma);
	BIND_KEY(Underscore);
	BIND_KEY(Period);
	BIND_KEY(Slash);
	BIND_KEY(Tilde);
	BIND_KEY(LeftBracket);
	BIND_KEY(Backslash);
	BIND_KEY(RightBracket);
	BIND_KEY(Quote);
}

void URadiantWebViewInputComponent::NullAxis(float)
{
}

void URadiantWebViewInputComponent::NullAction()
{
}

void URadiantWebViewInputComponent::PushMouseInputComponent()
{
	SetupInputComponents();
	PopInputComponents();
	InteractingPC->PushInputComponent(MouseInputComponent);
}

void URadiantWebViewInputComponent::PushMouseKeyInputComponent()
{
	SetupInputComponents();
	PopInputComponents();
	InteractingPC->PushInputComponent(MouseAndKeyInputComponent);
}

void URadiantWebViewInputComponent::PopInputComponents()
{
	if (MouseInputComponent)
	{
		InteractingPC->PopInputComponent(MouseInputComponent);
		InteractingPC->PopInputComponent(MouseAndKeyInputComponent);
	}
}

void URadiantWebViewInputComponent::BeginInteraction(APawn* InPawn)
{
	EndInteraction();
			
	if (RenderComponent && RenderComponent->WebView->GetBrowser() && InPawn)
	{
		APlayerController *PC = Cast<APlayerController>(InPawn->GetController());
		if (PC && PC->PlayerInput)
		{
			bFocusEditableField = false;
			bCanFocusEditableField = false;
			InteractingPC = PC;
			InteractingPawn = InPawn;

			if (InteractionMode > ERadiantWebViewInteractionMode::VisibleCursor)
			{
				PushMouseInputComponent();
			}
		}
		
	}
}

void URadiantWebViewInputComponent::EndInteraction()
{
	if (InteractingPawn)
	{
		PopInputComponents();
		InteractingPawn = nullptr;
		InteractingPC = nullptr;
	}
}

void URadiantWebViewInputComponent::LeftMouseButtonPressed()
{
	bCanFocusEditableField = true;
	KeyEventDelegate.Broadcast(EKeys::LeftMouseButton, IE_Pressed);
}

void URadiantWebViewInputComponent::LeftMouseButtonReleased()
{
	KeyEventDelegate.Broadcast(EKeys::LeftMouseButton, IE_Released);
}

IMPLEMENT_KEY_BIND(RightMouseButton)
IMPLEMENT_KEY_BIND(ThumbMouseButton)
IMPLEMENT_KEY_BIND(ThumbMouseButton2)
IMPLEMENT_KEY_BIND(MouseScrollUp);
IMPLEMENT_KEY_BIND(MouseScrollDown);
IMPLEMENT_KEY_BIND(BackSpace);
IMPLEMENT_KEY_BIND(Tab);
IMPLEMENT_KEY_BIND(Enter);
IMPLEMENT_KEY_BIND(Pause);
IMPLEMENT_KEY_BIND(CapsLock);
IMPLEMENT_KEY_BIND(Escape);
IMPLEMENT_KEY_BIND(SpaceBar);
IMPLEMENT_KEY_BIND(PageUp);
IMPLEMENT_KEY_BIND(PageDown);
IMPLEMENT_KEY_BIND(End);
IMPLEMENT_KEY_BIND(Home);
IMPLEMENT_KEY_BIND(Left);
IMPLEMENT_KEY_BIND(Up);
IMPLEMENT_KEY_BIND(Right);
IMPLEMENT_KEY_BIND(Down);
IMPLEMENT_KEY_BIND(Insert);
IMPLEMENT_KEY_BIND(Delete);
IMPLEMENT_KEY_BIND(Zero);
IMPLEMENT_KEY_BIND(One);
IMPLEMENT_KEY_BIND(Two);
IMPLEMENT_KEY_BIND(Three);
IMPLEMENT_KEY_BIND(Four);
IMPLEMENT_KEY_BIND(Five);
IMPLEMENT_KEY_BIND(Six);
IMPLEMENT_KEY_BIND(Seven);
IMPLEMENT_KEY_BIND(Eight);
IMPLEMENT_KEY_BIND(Nine);
IMPLEMENT_KEY_BIND(A);
IMPLEMENT_KEY_BIND(B);
IMPLEMENT_KEY_BIND(C);
IMPLEMENT_KEY_BIND(D);
IMPLEMENT_KEY_BIND(E);
IMPLEMENT_KEY_BIND(F);
IMPLEMENT_KEY_BIND(G);
IMPLEMENT_KEY_BIND(H);
IMPLEMENT_KEY_BIND(I);
IMPLEMENT_KEY_BIND(J);
IMPLEMENT_KEY_BIND(K);
IMPLEMENT_KEY_BIND(L);
IMPLEMENT_KEY_BIND(M);
IMPLEMENT_KEY_BIND(N);
IMPLEMENT_KEY_BIND(O);
IMPLEMENT_KEY_BIND(P);
IMPLEMENT_KEY_BIND(Q);
IMPLEMENT_KEY_BIND(R);
IMPLEMENT_KEY_BIND(S);
IMPLEMENT_KEY_BIND(T);
IMPLEMENT_KEY_BIND(U);
IMPLEMENT_KEY_BIND(V);
IMPLEMENT_KEY_BIND(W);
IMPLEMENT_KEY_BIND(X);
IMPLEMENT_KEY_BIND(Y);
IMPLEMENT_KEY_BIND(Z);
IMPLEMENT_KEY_BIND(NumPadZero);
IMPLEMENT_KEY_BIND(NumPadOne);
IMPLEMENT_KEY_BIND(NumPadTwo);
IMPLEMENT_KEY_BIND(NumPadThree);
IMPLEMENT_KEY_BIND(NumPadFour);
IMPLEMENT_KEY_BIND(NumPadFive);
IMPLEMENT_KEY_BIND(NumPadSix);
IMPLEMENT_KEY_BIND(NumPadSeven);
IMPLEMENT_KEY_BIND(NumPadEight);
IMPLEMENT_KEY_BIND(NumPadNine);
IMPLEMENT_KEY_BIND(Multiply);
IMPLEMENT_KEY_BIND(Add);
IMPLEMENT_KEY_BIND(Subtract);
IMPLEMENT_KEY_BIND(Decimal);
IMPLEMENT_KEY_BIND(Divide);
IMPLEMENT_KEY_BIND(F1);
IMPLEMENT_KEY_BIND(F2);
IMPLEMENT_KEY_BIND(F3);
IMPLEMENT_KEY_BIND(F4);
IMPLEMENT_KEY_BIND(F5);
IMPLEMENT_KEY_BIND(F6);
IMPLEMENT_KEY_BIND(F7);
IMPLEMENT_KEY_BIND(F8);
IMPLEMENT_KEY_BIND(F9);
IMPLEMENT_KEY_BIND(F10);
IMPLEMENT_KEY_BIND(F11);
IMPLEMENT_KEY_BIND(F12);
IMPLEMENT_KEY_BIND(NumLock);
IMPLEMENT_KEY_BIND(ScrollLock);
IMPLEMENT_KEY_BIND(LeftShift);
IMPLEMENT_KEY_BIND(RightShift);
IMPLEMENT_KEY_BIND(LeftControl);
IMPLEMENT_KEY_BIND(RightControl);
IMPLEMENT_KEY_BIND(LeftAlt);
IMPLEMENT_KEY_BIND(RightAlt);
IMPLEMENT_KEY_BIND(LeftCommand);
IMPLEMENT_KEY_BIND(RightCommand);
IMPLEMENT_KEY_BIND(Semicolon);
IMPLEMENT_KEY_BIND(Equals);
IMPLEMENT_KEY_BIND(Comma);
IMPLEMENT_KEY_BIND(Underscore);
IMPLEMENT_KEY_BIND(Period);
IMPLEMENT_KEY_BIND(Slash);
IMPLEMENT_KEY_BIND(Tilde);
IMPLEMENT_KEY_BIND(LeftBracket);
IMPLEMENT_KEY_BIND(Backslash);
IMPLEMENT_KEY_BIND(RightBracket);
IMPLEMENT_KEY_BIND(Quote);

void URadiantWebViewInputComponent::GetMouseState(CefRuntimeMouseEvent& InEvent)
{
	FIntPoint CursorPosition = RenderComponent->WebView->GetBrowserCursorPosition();
	
	InEvent.X = CursorPosition.X;
	InEvent.Y = CursorPosition.Y;
	InEvent.Modifiers = 0;

	if (InteractingPC)
	{
		if (InteractingPC->PlayerInput->IsPressed(EKeys::LeftMouseButton))
		{
			InEvent.Modifiers |= CEFRT_LeftMouseButton;
		}

		if (InteractingPC->PlayerInput->IsPressed(EKeys::RightMouseButton))
		{
			InEvent.Modifiers |= CEFRT_RightMouseButton;
		}

		if (InteractingPC->PlayerInput->IsPressed(EKeys::MiddleMouseButton))
		{
			InEvent.Modifiers |= CEFRT_MiddleMouseButton;
		}
	}
}

