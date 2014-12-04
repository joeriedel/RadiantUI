// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

URadiantWebViewInteractionComponent::URadiantWebViewInteractionComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bTickInEditor = true;
	bAutoActivate = true;
	bEnabled = true;
	bCursorVisible = false;
	CursorPosition = FVector2D(0.5f, 0.5f);
	SetIsReplicated(true);
}

void URadiantWebViewInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnabled && PawnOwner && GUIActor)
	{
		GUIActor->UpdateInteraction(PawnOwner);
	}
}

void URadiantWebViewInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	PawnOwner = CastChecked<APawn>(GetOwner());
}

void URadiantWebViewInteractionComponent::SetEnabled_Implementation(bool bInIsEnabled)
{
	if (bInIsEnabled != bEnabled)
	{
		bEnabled = bInIsEnabled;

		if (!bEnabled)
		{
			CancelInteraction();
		}
	}
}

bool URadiantWebViewInteractionComponent::SetEnabled_Validate(bool bInIsEnabled)
{
	return true;
}

void URadiantWebViewInteractionComponent::CancelInteraction()
{
	if (GUIActor)
	{
		GUIActor->CancelInteraction(PawnOwner);
		InteractionCanceled();
	}
}

void URadiantWebViewInteractionComponent::RequestInteraction(ARadiantWebViewActor* InActor)
{
	if (PawnOwner && bEnabled && !GUIActor)
	{
		GUIActor = InActor;
		GUIActor->WillInteract(PawnOwner);
	}
}

void URadiantWebViewInteractionComponent::InteractionCanceled()
{
	if (GUIActor)
	{
		Multicast_OnEndInteraction(GUIActor);
		SetCursorVisible(false);
		GUIActor = nullptr;
	}
}

void URadiantWebViewInteractionComponent::SetCursorPosition(const FVector2D& InCursorPosition)
{
	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{
		if (CursorPosition != InCursorPosition)
		{
			CursorPosition = InCursorPosition;
			if (GUIActor && GUIActor->bReplicatesInteraction)
			{
				Server_SetCursorPosition(CursorPosition);
			}
			UpdateCursorPosition();
		}
	}
}

void URadiantWebViewInteractionComponent::SetCursorVisible(bool bInVisible)
{
	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{
		if (bCursorVisible != bInVisible)
		{
			bCursorVisible = bInVisible;
			if (GUIActor && GUIActor->bReplicatesInteraction)
			{
				Server_SetCursorVisible(bCursorVisible);
			}
			UpdateCursorVisible();
		}
	}
}

void URadiantWebViewInteractionComponent::Server_SetCursorPosition_Implementation(FVector2D InCursorPosition)
{
	if (CursorPosition != InCursorPosition)
	{
		CursorPosition = InCursorPosition;
		OnRep_CursorPosition();
	}
}

bool URadiantWebViewInteractionComponent::Server_SetCursorPosition_Validate(FVector2D InCursorPosition)
{
	return true;
}

void URadiantWebViewInteractionComponent::Server_SetCursorVisible_Implementation(bool bInVisible)
{
	if (bCursorVisible != bInVisible)
	{
		bCursorVisible = bInVisible;
		OnRep_CursorVisible();
	}
}

bool URadiantWebViewInteractionComponent::Server_SetCursorVisible_Validate(bool bInVisible)
{
	return true;
}

void URadiantWebViewInteractionComponent::Server_ForwardKeyEvent_Implementation(FKey InKey, EInputEvent InEvent)
{
	if (GUIActor)
	{
		GUIActor->Multicast_KeyEvent(InKey, InEvent);
	}
}

bool URadiantWebViewInteractionComponent::Server_ForwardKeyEvent_Validate(FKey InKey, EInputEvent InEvent)
{
	return true;
}

void URadiantWebViewInteractionComponent::OnRep_CursorPosition()
{
	if (GUIActor && GUIActor->bReplicatesInteraction)
	{
		UpdateCursorPosition();
	}
}

void URadiantWebViewInteractionComponent::OnRep_CursorVisible()
{
	if (GUIActor && GUIActor->bReplicatesInteraction)
	{
		UpdateCursorVisible();
	}
}

void URadiantWebViewInteractionComponent::UpdateCursorPosition()
{
	if (GUIActor)
	{
		GUIActor->SetCursorPosition(CursorPosition);
	}
}

void URadiantWebViewInteractionComponent::UpdateCursorVisible()
{
	if (GUIActor)
	{
		GUIActor->SetCursorVisible(bCursorVisible);
	}
}

void URadiantWebViewInteractionComponent::OnRep_GUIActor()
{
	if (GUIActor && GUIActor->bReplicatesInteraction)
	{
		GUIActor->SetCursorPosition(CursorPosition);
		GUIActor->SetCursorVisible(bCursorVisible);
	}
}

void URadiantWebViewInteractionComponent::Multicast_OnBeginInteraction_Implementation(class ARadiantWebViewActor* InGUIActor)
{
	OnBeginInteraction(InGUIActor);
}

void URadiantWebViewInteractionComponent::Multicast_OnEndInteraction_Implementation(class ARadiantWebViewActor* InGUIActor)
{
	OnEndInteraction(InGUIActor);
}

void URadiantWebViewInteractionComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URadiantWebViewInteractionComponent, GUIActor);
	DOREPLIFETIME_CONDITION(URadiantWebViewInteractionComponent, CursorPosition, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(URadiantWebViewInteractionComponent, bCursorVisible, COND_SkipOwner);
}