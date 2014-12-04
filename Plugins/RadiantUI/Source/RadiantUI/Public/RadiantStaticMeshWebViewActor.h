// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebViewActor.h"
#include "RadiantWebViewInteractionMesh.h"
#include "RadiantStaticMeshWebViewActor.generated.h"

UCLASS(Blueprintable, BlueprintType)
class RADIANTUI_API ARadiantStaticMeshWebViewActor : public ARadiantWebViewActor
{
	GENERATED_BODY()

public:

	ARadiantStaticMeshWebViewActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	class UStaticMeshComponent* StaticMeshComponent;
	
	virtual void PostInitializeComponents();
	
	virtual void Serialize(FArchive& Ar) override;

	virtual bool CanInteract() override { return InteractionMesh != nullptr; }
	virtual bool GetUVForPoint(int InTriIndex, const FVector& InPoint, FVector2D& OutUV) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	UPROPERTY(transient)
	URadiantWebViewInteractionMesh* InteractionMesh;

#if WITH_EDITORONLY_DATA
	void ExtractInteractionMesh();
#endif
};