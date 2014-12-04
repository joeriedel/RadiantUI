// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
URadiantWebViewRenderComponent::URadiantWebViewRenderComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bTickInEditor = true;
	bAutoActivate = true;
}

void URadiantWebViewRenderComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	static const int ArchiveVersion = 1;

	int Version = ArchiveVersion;
	Ar << Version;
}

void URadiantWebViewRenderComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		WebView = MakeShareable(new FRadiantWebView(DefaultSettings));
	}
}

void URadiantWebViewRenderComponent::OnComponentDestroyed()
{
	WebView.Reset();
	Super::OnComponentDestroyed();
}

void URadiantWebViewRenderComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (WebView.IsValid() && WebView->IsRunning())
	{
		WebView->Tick(FPlatformTime::Seconds() - GStartTime, World->GetTimeSeconds(), DeltaTime, World->FeatureLevel);
	}
}

void URadiantWebViewRenderComponent::StartRefresh()
{
	check(WebView.IsValid());
	WebView->SetNetMode(GetNetMode());
	WebView->Start();
}

void URadiantWebViewRenderComponent::PauseRefresh()
{
	check(WebView.IsValid());
	WebView->PauseRefresh();
}

void URadiantWebViewRenderComponent::StopRefreshAndRelease(bool bPreserveRenderTarget)
{
	check(WebView.IsValid());
	WebView->Stop(bPreserveRenderTarget);
}
