// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantJavaScriptFunctionCall.h"
#include "RadiantJavaScriptFunctionCallLibrary.generated.h"

UCLASS(MinimalAPI)
class URadiantJavaScriptFunctionCallLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=JavaScript, meta=(FriendlyName="New JavaScript Function Call Instance"))
	static URadiantJavaScriptFunctionCall* NewJavaScriptFunctionCallInstance(TSubclassOf<URadiantJavaScriptFunctionCall> Class);
};