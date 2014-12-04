// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebViewInteractionComponent.generated.h"

class ARadiantWebViewActor;

UCLASS(meta = (BlueprintSpawnableComponent))
class RADIANTUI_API URadiantWebViewInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URadiantWebViewInteractionComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	uint32 bEnabled:1;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interaction")
	void SetEnabled(bool bInIsEnabled);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnBeginInteraction(class ARadiantWebViewActor* InGUIActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnEndInteraction(class ARadiantWebViewActor* InGUIActor);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnBeginInteraction(class ARadiantWebViewActor* InGUIActor);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEndInteraction(class ARadiantWebViewActor* InGUIActor);
	
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CursorPosition, transient, BlueprintReadOnly, Category = "WebView")
	FVector2D CursorPosition;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetCursorPosition(const FVector2D& InCursorPosition);

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CursorVisible, transient, BlueprintReadOnly, Category = "Input")
	uint32 bCursorVisible:1;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void SetCursorVisible(bool bInVisible);

	UFUNCTION(Server, Reliable, WithValidation, Category = "Input")
	void Server_ForwardKeyEvent(FKey InKey, EInputEvent InEvent);

	// Begin UActorComponent interface.
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	// End UActorComponent interface.

	void CancelInteraction();

private:

	friend class ARadiantWebViewActor;

	UPROPERTY(transient, Replicated, ReplicatedUsing=OnRep_GUIActor)
	ARadiantWebViewActor* GUIActor;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCursorPosition(FVector2D InCursorPosition);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetCursorVisible(bool bInVisible);

	UFUNCTION()
	void OnRep_CursorPosition();

	UFUNCTION()
	void OnRep_CursorVisible();

	UFUNCTION()
	void OnRep_GUIActor();

	void UpdateCursorPosition();
	void UpdateCursorVisible();
	void RequestInteraction(ARadiantWebViewActor* InActor);
	void InteractionCanceled();

	APawn* PawnOwner;
};