// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

void URadiantJavaScriptFunctionCall::Invoke(const TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface>& Target)
{
	if (!FunctionName.IsEmpty() && Target && Target.GetObject())
	{
		Target->CallJavaScriptFunction(FunctionName, this);
	}
}
