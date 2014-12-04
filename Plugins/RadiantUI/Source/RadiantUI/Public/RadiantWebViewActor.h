// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "StaticMeshResources.h"
#include "RadiantWebViewInteractionMode.h"
#include "RadiantJavaScriptFunctionCallTargetInterface.h"
#include "RadiantWebViewRenderComponent.h"
#include "RadiantWebViewInputComponent.h"
#include "RadiantWebViewActor.generated.h"

class URadiantWebViewInteractionComponent;

UCLASS(abstract, NotBlueprintable, BlueprintType)
class RADIANTUI_API ARadiantWebViewActor : public AActor, public IRadiantJavaScriptFunctionCallTargetInterface
{
	GENERATED_BODY()

public:

	ARadiantWebViewActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, Category = "WebView")
	class URadiantWebViewRenderComponent* WebViewRenderComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WebView")
	class URadiantWebViewInputComponent* WebViewInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WebView", meta = (Tooltip = "The material at the specified index on the static mesh must have a Texture2D input named WebViewTexture"))
	int32 MaterialIndex;
		
	UPROPERTY(transient, BlueprintReadOnly, Category = "WebView")
	UMaterialInstanceDynamic* WebViewMID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WebView", meta = (Tooltip = "(Optional) A material that will replace the material at the specified MaterialIndex. If bSwapWhenOverllaped is set the material will be swapped dynamically."))
	UMaterialInterface* ReplaceMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WebView")
	uint32 bEnabledByDefault:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WebView")
	uint32 bSwapMaterialWhenOverlapped:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float TraceOversize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (Tooltip = "Player interactions with the WebView actor will be replicated. Only one player can use the WebView actor at a time in this mode."))
	uint32 bReplicatesInteraction:1;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Interactive, EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (Tooltip = "If enabled players can interact with the webview."))
	uint32 bInteractive:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ERadiantWebViewInteractionMode::Type> InteractionMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	uint32 bMouseThumbNavigate:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class APawn* InteractingPawn;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Interaction")
	void SetInteractive(bool bIsInteractive);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void Received_BeginInteraction(class APawn* InstigatorPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void Received_EndInteraction(class APawn* InstigatorPawn);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void BeginInteractionOverlap(class AActor* OverlappingActor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EndInteractionOverlap(class AActor* OverlappingActor);

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void StartRefresh();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void PauseRefresh();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void StopRefreshAndRelease(bool bPreserveRenderTarget);

	UFUNCTION(BlueprintCallable, Category = "WebView")
	bool CanNavigateForward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	bool CanNavigateBackward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void NavigateForward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void NavigateBackward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void LoadURL(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "WebView")
	FString GetURL();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void SetRefreshRate(float FramesPerSecond);

	UFUNCTION(BlueprintCallable, Category = "WebView")
	float GetRefreshRate();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = JavaScript)
	TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface> GetJavaScriptCallContext();

	UFUNCTION(BlueprintCallable, Category=WebView)
	void SetInteractionMode(TEnumAsByte<ERadiantWebViewInteractionMode::Type> Mode);

	void CancelInteraction(APawn* InPawn);

	void SetCursorPosition(const FVector2D& InPosition);
	void SetCursorVisible(bool bInVisible);

	// Default implementations will call the BP events.
	virtual void BeginInteraction(class APawn* InCurrentController);
	virtual void EndInteraction(class APawn* InCurrentController);
	
	virtual bool CanInteract() { return false; }
	virtual bool GetUVForPoint(int InTriIndex, const FVector& InPoint, FVector2D& OutUV) { return false;  }
	
	// Begin UObject interface
	virtual void PostInitProperties() override;
	virtual void Serialize(FArchive& Ar) override;
	// End UObject interface

	// Begin AActor interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End AActor interface

	// This must be called before PostInitializeComponents() by
	// your child class to register the mesh to use.
	virtual void RegisterMeshComponent(UMeshComponent* InMeshComponent);

	virtual void CallJavaScriptFunction(const FString& HookName, UObject* Parameters) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	friend class URadiantWebViewInteractionComponent;

	UPROPERTY(transient)
	UMaterialInterface *OldMeshMaterial;
	
	UPROPERTY(transient)
	UMeshComponent* MeshComponent;

	UPROPERTY(transient)
	URadiantWebViewInteractionComponent* InteractingPawnComponent;

	UPROPERTY(transient)
	TArray<APawn*> OverlappingPawns;

	UFUNCTION(Reliable, NetMulticast)
	void Multicast_SetInteractingPawn(APawn* InPawn);

	UFUNCTION(Reliable, NetMulticast)
	void Multicast_KeyEvent(FKey InKey, EInputEvent InEvent);

	UFUNCTION(Reliable, NetMulticast)
	void Multicast_BindWebMaterial();

	UFUNCTION(Reliable, NetMulticast)
	void Multicast_ResetMaterial();

	UFUNCTION()
	void OnRep_Interactive();

	UFUNCTION()
	void OnKeyEvent(const FKey& InKey, EInputEvent InEvent);

	void DispatchKeyEvent(const FKey& InKey, EInputEvent InEvent);
	void DispatchPlatformKeyEvent(const FKey& InKey, EInputEvent InEvent);

	bool ShouldSimulate(APawn* InPawn = nullptr);

	void SetInteractingPawn(APawn* InPawn);
	void Local_SetInteractingPawn(APawn* InPawn);

	void CheckOverlappedInteractions();
	void CheckInteraction(APawn* InPawn);
	void WillInteract(APawn* InPawn);
	void UpdateInteraction(APawn* InPawn);
	void SyncMouseState(bool InClearButtons, bool InFocus);
	bool TraceScreenPoint(APawn* InPawn, FVector2D& OutUV);
	void OnExecuteJSHook(const FString& HookName, ICefRuntimeVariantList* Arguments);

	int32 ModifierKeyState;
	int32 ModifierKeyExState;
	int32 OldMaterialIndex;
		
	void InitDynamicMaterial();
	void BindDynamicMaterial();
	void BindWebMaterial();
	void ResetMaterial();
	void Local_BindWebMaterial();
	void Local_ResetMaterial();
};