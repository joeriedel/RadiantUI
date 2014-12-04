// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "CanvasTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RadiantCanvasRenderTarget.generated.h"

UCLASS(transient)
class RADIANTUI_API URadiantCanvasRenderTarget : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(transient, VisibleAnywhere, BlueprintReadOnly, Category="Render Targets")
	UTextureRenderTarget2D* RenderTargetTexture;

	/*! Only valid between BeginPaint() and EndPaint(). */
	FCanvas* Canvas;

	void BeginPaint(float InRealTime, float InWorldTime, float InWorldDeltaTime, ERHIFeatureLevel::Type FeatureLevel);
	void EndPaint();
	void Destroy();

	static URadiantCanvasRenderTarget* CreateTransient(uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, const FColor& InClearColor);

private:

	MS_ALIGN(16) uint8 FCanvasBytes[sizeof(FCanvas)] GCC_ALIGN(16);
};
