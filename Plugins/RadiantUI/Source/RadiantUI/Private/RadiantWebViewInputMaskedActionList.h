// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebViewInputMaskedActionList.generated.h"

UCLASS(BlueprintType, Blueprintable)
class URadiantWebViewInputMaskedActionList : public UObject
{
	GENERATED_BODY()

public:

	URadiantWebViewInputMaskedActionList(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Masked Actions")
	TArray<FName> ActionMappings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Masked Actions")
	TArray<FName> AxisMappings;
};