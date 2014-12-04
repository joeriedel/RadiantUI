// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"

URadiantJavaScriptFunctionCall* URadiantJavaScriptFunctionCallLibrary::NewJavaScriptFunctionCallInstance(TSubclassOf<URadiantJavaScriptFunctionCall> Class)
{
	return NewObject<URadiantJavaScriptFunctionCall>(GetTransientPackage(), Class);
}
