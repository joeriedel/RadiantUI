// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebView.h"
#include "RadiantWebViewRenderComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class RADIANTUI_API URadiantWebViewRenderComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	URadiantWebViewRenderComponent(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FRadiantWebView> WebView;

	UPROPERTY(EditAnywhere, Category = "WebView")
	FRadiantWebViewDefaultSettings DefaultSettings;

	// Begin UObject interface.
	virtual void Serialize(FArchive& Ar) override;
	// End UObject interface.

	// Begin UActorComponent interface.
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual void OnComponentDestroyed() override;
	// End UActorComponent interface.

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void StartRefresh();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void PauseRefresh();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void StopRefreshAndRelease(bool bPreserveRenderTarget);
};