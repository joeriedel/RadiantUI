// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantJavaScriptFunctionCallTargetInterface.generated.h"

UINTERFACE(Blueprintable, BlueprintType)
class RADIANTUI_API URadiantJavaScriptFunctionCallTargetInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IRadiantJavaScriptFunctionCallTargetInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual void CallJavaScriptFunction(const FString& HookName, UObject* Parameters) = 0;
};