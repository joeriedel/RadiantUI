// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantJavaScriptFunctionCallTargetInterface.h"
#include "RadiantJavaScriptFunctionCall.generated.h"

UCLASS(abstract, DefaultToInstanced, Blueprintable, BlueprintType)
class RADIANTUI_API URadiantJavaScriptFunctionCall : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=JavaScript)
	FString FunctionName;

	UFUNCTION(BlueprintCallable, Category=JavaScript)
	void Invoke(const TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface>& Context);
};