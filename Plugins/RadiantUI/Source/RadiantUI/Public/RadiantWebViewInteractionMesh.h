// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebViewInteractionMesh.generated.h"

USTRUCT()
struct FRadiantWebViewInteractionMeshTriangle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Verts[3];

	UPROPERTY()
	FVector2D UV[3];
};

USTRUCT()
struct FRadiantWebViewInteractionMeshSection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	uint32 FirstTriangle;

	UPROPERTY()
	uint32 LastTriangle;

	UPROPERTY()
	TArray<FRadiantWebViewInteractionMeshTriangle> Triangles;
};

UCLASS()
class URadiantWebViewInteractionMesh : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FRadiantWebViewInteractionMeshSection> Sections;

	bool GetUVForPosition(int InTriIndex, const FVector& InPoint, FVector2D& OutUV);

};