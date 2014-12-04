// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "RadiantWebViewInteractionMode.generated.h"

UENUM()
namespace ERadiantWebViewInteractionMode
{
	enum Type
	{
		VisibleCursor,
		CursorAndButtons,
		CursorAndKeyboard
	};
}
