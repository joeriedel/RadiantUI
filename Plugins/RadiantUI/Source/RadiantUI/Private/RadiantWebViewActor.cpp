// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "JSCall.h"

ARadiantWebViewActor::ARadiantWebViewActor(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bInteractive = true;
	InteractionMode = ERadiantWebViewInteractionMode::CursorAndKeyboard;
	bEnabledByDefault = true;
	bReplicatesInteraction = false;
	bMouseThumbNavigate = true;
	bSwapMaterialWhenOverlapped = false;
	
	MaterialIndex = 0;
	ModifierKeyState = 0;
	ModifierKeyExState = 0;
	TraceChannel = ECC_Visibility;
	TraceOversize = 1024.0f;

	WebViewRenderComponent = ObjectInitializer.CreateDefaultSubobject<URadiantWebViewRenderComponent>(this, TEXT("WebViewRenderComponent0"));
	WebViewInputComponent = ObjectInitializer.CreateDefaultSubobject<URadiantWebViewInputComponent>(this, TEXT("WebViewInputComponent0"));

	WebViewInputComponent->KeyEventDelegate.AddUObject(this, &ARadiantWebViewActor::OnKeyEvent);
}

void ARadiantWebViewActor::PostInitProperties()
{
	Super::PostInitProperties();
	MeshComponent = nullptr;
	WebViewInputComponent->SetInteractionMode(InteractionMode);
}

void ARadiantWebViewActor::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	static const int ArchiveVersion = 1;

	int Version = ArchiveVersion;

	Ar << Version;
}

void ARadiantWebViewActor::InitDynamicMaterial()
{
	WebViewRenderComponent->WebView->PreCreateTexture();

	OldMaterialIndex = 0;
	OldMeshMaterial = nullptr;

	BindDynamicMaterial();

	if (!bSwapMaterialWhenOverlapped)
	{
		Local_BindWebMaterial();
	}
}

void ARadiantWebViewActor::BindDynamicMaterial()
{
	
	if ((GetNetMode() != NM_DedicatedServer) && MeshComponent && (WebViewRenderComponent->WebView->WebViewCanvas))
	{
		if (OldMeshMaterial)
		{
			MeshComponent->SetMaterial(OldMaterialIndex, OldMeshMaterial);
		}

		OldMeshMaterial = MeshComponent->GetMaterial(MaterialIndex);

		if (WebViewMID)
		{
			WebViewMID->MarkPendingKill();
		}

		if (ReplaceMaterial)
		{
			WebViewMID = UMaterialInstanceDynamic::Create(ReplaceMaterial, GetTransientPackage());
		}
		else if (OldMeshMaterial)
		{
			WebViewMID = UMaterialInstanceDynamic::Create(OldMeshMaterial, GetTransientPackage());
		}

		if (WebViewMID)
		{
			WebViewMID->SetTextureParameterValue(TEXT("WebViewTexture"), WebViewRenderComponent->WebView->WebViewCanvas->RenderTargetTexture);
		}
	}
}

void ARadiantWebViewActor::BindWebMaterial()
{
	if (bReplicatesInteraction)
	{
		check(HasAuthority());
		Multicast_BindWebMaterial();
	}
	else
	{
		Local_BindWebMaterial();
	}
}

void ARadiantWebViewActor::ResetMaterial()
{
	if (bReplicatesInteraction)
	{
		check(HasAuthority());
		Multicast_ResetMaterial();
	}
	else
	{
		Local_ResetMaterial();
	}
}

void ARadiantWebViewActor::Local_BindWebMaterial()
{
	if (WebViewMID && MeshComponent)
	{
		MeshComponent->SetMaterial(MaterialIndex, WebViewMID);
	}
}

void ARadiantWebViewActor::Local_ResetMaterial()
{
	if (OldMeshMaterial && MeshComponent)
	{
		MeshComponent->SetMaterial(MaterialIndex, OldMeshMaterial);
	}
}

void ARadiantWebViewActor::RegisterMeshComponent(UMeshComponent* InMeshComponent)
{
	MeshComponent = InMeshComponent;
}

void ARadiantWebViewActor::CallJavaScriptFunction(const FString& HookName, UObject* Parameters)
{
	if (!HookName.IsEmpty() && Parameters && WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid())
	{
		ICefRuntimeVariantList* Arguments = FJavaScriptHelper::CreateVariantList(Parameters->GetClass(), Parameters, WebViewRenderComponent->WebView->GetVariantFactory());
		FTCHARToUTF8 Convert(*HookName);
		WebViewRenderComponent->WebView->CallJavaScriptFunction(Convert.Get(), Arguments);
		if (Arguments)
		{
			Arguments->Release();
		}
	}
}

void ARadiantWebViewActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ARadiantWebViewActor::PostInitializeComponents()
{
	check(MeshComponent != nullptr);
	Super::PostInitializeComponents();
}

void ARadiantWebViewActor::BeginPlay()
{
	Super::BeginPlay();
	WebViewInputComponent->SetInteractionMode(InteractionMode);

	if (bEnabledByDefault)
	{
		StartRefresh();
	}
}

void ARadiantWebViewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ARadiantWebViewActor::StartRefresh()
{
	WebViewRenderComponent->StartRefresh();

	if (!WebViewMID)
	{
		InitDynamicMaterial();

		WebViewRenderComponent->WebView->OnExecuteJSHook.AddUObject(this, &ARadiantWebViewActor::OnExecuteJSHook);
	}
}

void ARadiantWebViewActor::PauseRefresh()
{
	WebViewRenderComponent->PauseRefresh();
}

void ARadiantWebViewActor::StopRefreshAndRelease(bool bPreserveRenderTarget)
{
	WebViewRenderComponent->StopRefreshAndRelease(bPreserveRenderTarget);
}

bool ARadiantWebViewActor::CanNavigateForward()
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	return WebViewRenderComponent->WebView->CanNavigateForward();
}

bool ARadiantWebViewActor::CanNavigateBackward()
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	return WebViewRenderComponent->WebView->CanNavigateBackward();
}

void ARadiantWebViewActor::NavigateForward()
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	WebViewRenderComponent->WebView->NavigateForward();
}

void ARadiantWebViewActor::NavigateBackward()
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	WebViewRenderComponent->WebView->NavigateBackward();
}

void ARadiantWebViewActor::LoadURL(const FString& InURL)
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	WebViewRenderComponent->WebView->LoadURL(InURL);
}

FString ARadiantWebViewActor::GetURL()
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	return WebViewRenderComponent->WebView->GetURL();
}

void ARadiantWebViewActor::SetRefreshRate(float InFramesPerSecond)
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	WebViewRenderComponent->WebView->SetRefreshRate(InFramesPerSecond);
}

float ARadiantWebViewActor::GetRefreshRate()
{
	check(WebViewRenderComponent && WebViewRenderComponent->WebView.IsValid());
	return WebViewRenderComponent->WebView->GetRefreshRate();
}

TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface> ARadiantWebViewActor::GetJavaScriptCallContext()
{ 
	return TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface>(this); 
}

void ARadiantWebViewActor::SetInteractionMode(TEnumAsByte<ERadiantWebViewInteractionMode::Type> InInteractionMode)
{
	InteractionMode = InInteractionMode;
	WebViewInputComponent->SetInteractionMode(InInteractionMode);
}

void ARadiantWebViewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ShouldSimulate())
	{
		if (InteractingPawn && InteractingPawn->IsPendingKillPending())
		{
			SetInteractingPawn(nullptr);
		}

		CheckOverlappedInteractions();
	}
}

void ARadiantWebViewActor::SetInteractive_Implementation(bool bInIsInteractive)
{
	if (bInteractive != bInIsInteractive)
	{
		bInteractive = bInIsInteractive;

		if (!bInteractive)
		{
			if (InteractingPawn)
			{
				CancelInteraction(InteractingPawn);
			}
		}
	}
}

bool ARadiantWebViewActor::SetInteractive_Validate(bool bInIsInteractive)
{
	return true;
}

void ARadiantWebViewActor::OnRep_Interactive()
{
	if (!bReplicatesInteraction)
	{
		if (InteractingPawn && !bInteractive)
		{
			CancelInteraction(InteractingPawn);
		}
	}
}

void ARadiantWebViewActor::BeginInteraction(class APawn* InInteractingPawn)
{
	Received_BeginInteraction(InInteractingPawn);
}

void ARadiantWebViewActor::EndInteraction(class APawn* InInteractingPawn)
{
	Received_EndInteraction(InInteractingPawn);
}

void ARadiantWebViewActor::BeginInteractionOverlap(class AActor* InOverlappingActor)
{
	APawn* OverlappingPawn = Cast<APawn>(InOverlappingActor);

	if (CanInteract() && OverlappingPawn && ShouldSimulate(OverlappingPawn))
	{
		URadiantWebViewInteractionComponent* InteractionComponent = OverlappingPawn->FindComponentByClass<URadiantWebViewInteractionComponent>();
		if (InteractionComponent)
		{
			if ((OverlappingPawns.Num() == 0) && bSwapMaterialWhenOverlapped)
			{
				BindWebMaterial();
			}

			OverlappingPawns.Add(OverlappingPawn);

			if (!InteractingPawn && bInteractive)
			{
				CheckInteraction(OverlappingPawn);
			}
		}
	}
}

void ARadiantWebViewActor::EndInteractionOverlap(class AActor* InOverlappingActor)
{
	APawn* Pawn = Cast<APawn>(InOverlappingActor);

	if (CanInteract() && Pawn && ShouldSimulate(Pawn))
	{
		CancelInteraction(Pawn);
		OverlappingPawns.Remove(Pawn);

		if ((OverlappingPawns.Num() == 0) && bSwapMaterialWhenOverlapped)
		{
			ResetMaterial();
		}
	}
}

void ARadiantWebViewActor::CheckOverlappedInteractions()
{
	for (int i = 0; i < OverlappingPawns.Num();)
	{
		APawn* Pawn = OverlappingPawns[i];

		if (Pawn->IsPendingKillPending())
		{
			OverlappingPawns.RemoveAt(i, 1);
		}
		else
		{
			++i;
		}
	}

	if (!InteractingPawn && bInteractive)
	{
		for (auto It = OverlappingPawns.CreateConstIterator(); It && (InteractingPawn == nullptr); ++It)
		{
			CheckInteraction(*It);
		}
	}
}

void ARadiantWebViewActor::CheckInteraction(APawn* InPawn)
{
	FVector2D Unused;
	if (TraceScreenPoint(InPawn, Unused))
	{
		URadiantWebViewInteractionComponent* InteractionComponent = InPawn->FindComponentByClass<URadiantWebViewInteractionComponent>();
		check(InteractionComponent);
		InteractionComponent->RequestInteraction(this);
	}
}

void ARadiantWebViewActor::CancelInteraction(APawn* InPawn)
{
	if (InteractingPawn && (InteractingPawn == InPawn) && ShouldSimulate(InPawn))
	{
		SetInteractingPawn(nullptr);
	}
}

void ARadiantWebViewActor::SetCursorPosition(const FVector2D& InPosition)
{
	if (WebViewRenderComponent)
	{
		WebViewRenderComponent->WebView->SetCursorPosition(InPosition);
		SyncMouseState(false, true);
	}
}

void ARadiantWebViewActor::SetCursorVisible(bool bInVisible)
{
	if (WebViewRenderComponent)
	{
		WebViewRenderComponent->WebView->SetCursorVisible(bInVisible);
	}
}

void ARadiantWebViewActor::WillInteract(APawn* InPawn)
{
	check(InteractingPawn == nullptr);	
	check(ShouldSimulate(InPawn));

	SetInteractingPawn(InPawn);
}

void ARadiantWebViewActor::SetInteractingPawn(APawn* InPawn)
{
	if (bReplicatesInteraction)
	{
		if (HasAuthority())
		{
			Multicast_SetInteractingPawn(InPawn);
		}
	}
	else
	{
		Local_SetInteractingPawn(InPawn);
	}
}

void ARadiantWebViewActor::Multicast_SetInteractingPawn_Implementation(APawn* InPawn)
{
	Local_SetInteractingPawn(InPawn);
}

void ARadiantWebViewActor::Local_SetInteractingPawn(APawn* InPawn)
{
	if (InteractingPawn && (InteractingPawn != InPawn))
	{
		EndInteraction(InteractingPawn);

		if (InteractingPawnComponent)
		{
			InteractingPawnComponent->InteractionCanceled();
			WebViewInputComponent->EndInteraction();
		}

		SyncMouseState(true, false);
		if (WebViewRenderComponent->WebView->GetBrowser())
		{
			WebViewRenderComponent->WebView->GetBrowser()->SendFocusEvent(false);
			WebViewRenderComponent->WebView->SetCursorVisible(false);
		}
		
		InteractingPawn = nullptr;
		InteractingPawnComponent = nullptr;
	}

	ModifierKeyState = 0; // clear modifier keys
	ModifierKeyExState = 0;

	if (InPawn)
	{
		InteractingPawn = InPawn;

		if (WebViewRenderComponent->WebView->GetBrowser())
		{
			WebViewRenderComponent->WebView->GetBrowser()->SendFocusEvent(true);
		}

		if (ShouldSimulate(InPawn) || InPawn->IsLocallyControlled())
		{
			InteractingPawnComponent = InPawn->FindComponentByClass<URadiantWebViewInteractionComponent>();
			check(InteractingPawnComponent);

			WebViewInputComponent->BeginInteraction(InPawn);

			if (InPawn->IsLocallyControlled())
			{
				FVector2D CursorPosition;
				if (TraceScreenPoint(InPawn, CursorPosition))
				{
					InteractingPawnComponent->SetCursorPosition(CursorPosition);
					InteractingPawnComponent->SetCursorVisible(true);

					if (InteractingPawnComponent->GUIActor != this)
					{
						// gui actor not replicated yet... call server thunks
						InteractingPawnComponent->Server_SetCursorPosition(CursorPosition);
						InteractingPawnComponent->Server_SetCursorVisible(true);
					}
				}
			}
		}
		
		SyncMouseState(true, true);
		BeginInteraction(InPawn);
	}
}

void ARadiantWebViewActor::Multicast_KeyEvent_Implementation(FKey InKey, EInputEvent InEvent)
{
	check(bReplicatesInteraction);

	if (InteractingPawn && !InteractingPawn->IsLocallyControlled())
	{
		DispatchKeyEvent(InKey, InEvent);
	}
}

void ARadiantWebViewActor::Multicast_BindWebMaterial_Implementation()
{
	Local_BindWebMaterial();
}

void ARadiantWebViewActor::Multicast_ResetMaterial_Implementation()
{
	Local_ResetMaterial();
}

void ARadiantWebViewActor::SyncMouseState(bool InClearButtons, bool InFocus)
{
	if (WebViewRenderComponent->WebView->GetBrowser())
	{
		CefRuntimeMouseEvent Event;
		WebViewInputComponent->GetMouseState(Event);

		if (InClearButtons)
		{
			Event.Modifiers = 0;
		}

		WebViewRenderComponent->WebView->GetBrowser()->SendMouseMoveEvent(Event, !InFocus);
	}
}

void ARadiantWebViewActor::UpdateInteraction(APawn* InPawn)
{
	// guard this: if we are replicating interactions then the authoritative version may have
	// changed the InteractingPawn but that has not been replicated to both actors yet.
	if (InPawn != InteractingPawn)
	{
		return;
	}

	check(InPawn && (InPawn == InteractingPawn));

	FVector2D CursorPosition;
	if (InteractingPawnComponent && TraceScreenPoint(InPawn, CursorPosition))
	{
		if (InPawn->IsLocallyControlled())
		{
			InteractingPawnComponent->SetCursorPosition(CursorPosition);
		}
	}
	else if (ShouldSimulate(InPawn))
	{
		CancelInteraction(InPawn);
	}
}

bool ARadiantWebViewActor::ShouldSimulate(APawn* InPawn)
{
	if (bReplicatesInteraction)
	{
		return HasAuthority();
	}

	// only care about local actors
	return InPawn ? InPawn->IsLocallyControlled() : true;
}

void ARadiantWebViewActor::OnKeyEvent(const FKey& InKey, EInputEvent InEvent)
{
	if (InteractingPawn)
	{
		if (bReplicatesInteraction && InteractingPawn->IsLocallyControlled() && InteractingPawnComponent)
		{
			InteractingPawnComponent->Server_ForwardKeyEvent(InKey, InEvent);
		}

		DispatchKeyEvent(InKey, InEvent);
	}
}

bool ARadiantWebViewActor::TraceScreenPoint(APawn* InPawn, FVector2D& OutUV)
{
	FVector Location;
	FRotator Rotation;

	APlayerController *PC = Cast<APlayerController>(InPawn->GetController());

	if (PC)
	{
		PC->GetPlayerViewPoint(Location, Rotation);
	}
	else
	{
		InPawn->GetActorEyesViewPoint(Location, Rotation);
	}

	const FVector VectorToTarget = Location - GetActorLocation();
	const float DistanceToTrace = VectorToTarget.Size() + TraceOversize;
	const FVector EndTrace = Location + Rotation.Vector()*DistanceToTrace;

	// Is this actor "pointing" at our surface mesh?
	
	static FName TraceTag = FName(TEXT("RadiantGUITrace"));
	
	FCollisionQueryParams TraceParams(TraceTag, true, InPawn);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnFaceIndex = true;

	FHitResult HitResult;
	
	UWorld* const World = GetWorld();
	if (World->LineTraceSingle(HitResult, Location, EndTrace, TraceChannel, TraceParams))
	{
		if (HitResult.Actor.IsValid() && (HitResult.Actor.Get() == this))
		{
			FVector ActorRelativeLocation = ActorToWorld().InverseTransformPosition(HitResult.Location);
			return GetUVForPoint(HitResult.FaceIndex, ActorRelativeLocation, OutUV);
		}
	}

	return false;
}

void ARadiantWebViewActor::OnExecuteJSHook(const FString& HookName, ICefRuntimeVariantList* Arguments)
{
	FJavaScriptHelper::ExecuteHook(this, HookName, Arguments);
}

void ARadiantWebViewActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadiantWebViewActor, bInteractive);
}

#if WITH_EDITOR
void ARadiantWebViewActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	/*if ((PropertyName == TEXT("MaterialIndex")))
	{
		BindDynamicMaterial();
	}*/

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif