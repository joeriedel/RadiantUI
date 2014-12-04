// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantUIPrivatePCH.h"

URadiantWebViewInputMaskedActionList::URadiantWebViewInputMaskedActionList(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ActionMappings.Add("Jump");
	ActionMappings.Add("RunModifier");
	ActionMappings.Add("Fire");

	AxisMappings.Add("MoveForward");
	AxisMappings.Add("MoveRight");
}