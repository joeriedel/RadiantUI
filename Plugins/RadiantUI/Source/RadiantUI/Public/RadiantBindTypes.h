// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

// Include this somewhere in your project, otherwise packaged version of the your project
// will never find types delcared in the RadiantSDK plugin

#include "RadiantCanvasRenderTarget.h"
#include "RadiantJavaScriptFunctionCall.h"
#include "RadiantJavaScriptFunctionCallLibrary.h"
#include "RadiantJavaScriptFunctionCallTargetInterface.h"
#include "RadiantStaticMeshWebViewActor.h"
#include "RadiantWebViewActor.h"
#include "RadiantWebViewHUD.h"
#include "RadiantWebViewHUDElement.h"
#include "RadiantWebViewInputComponent.h"
#include "RadiantWebViewInteractionComponent.h"
#include "RadiantWebViewInteractionMode.h"
#include "RadiantWebViewRenderComponent.h"

#define RADIANTSDK_BIND_TYPES(__API) \
	void __API StaticBindRadiantSDKTypes() \
	{ \
		URadiantCanvasRenderTarget::StaticClass(); \
		URadiantJavaScriptFunctionCall::StaticClass(); \
		URadiantJavaScriptFunctionCallLibrary::StaticClass(); \
		URadiantJavaScriptFunctionCallTargetInterface::StaticClass(); \
		ARadiantStaticMeshWebViewActor::StaticClass(); \
		ARadiantWebViewActor::StaticClass(); \
		ARadiantWebViewHUD::StaticClass(); \
		URadiantWebViewHUDElement::StaticClass(); \
		URadiantWebViewInputComponent::StaticClass(); \
		URadiantWebViewInteractionComponent::StaticClass(); \
		URadiantWebViewRenderComponent::StaticClass(); \
	}
