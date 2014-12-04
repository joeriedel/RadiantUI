// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#include "RadiantUIPrivatePCH.h"
#include "ModuleManager.h"

DEFINE_LOG_CATEGORY(RadiantUILog);

namespace
{
	ICefRuntimeAPI* CefRuntimeAPI = 0;
}

ICefRuntimeAPI* GetCefRuntime()
{
#if !WITH_EDITOR
	if (!CefRuntimeAPI)
	{
		FModuleManager::LoadModuleChecked< IModuleInterface >("RadiantUI");
	}
#endif
	return CefRuntimeAPI;
}

class FRadiantUIModule : public IModuleInterface, public ICefRuntimeCallbacks
{
	// ICefRuntimeCallbacks
	virtual void Release() override
	{
	}

	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		CefRuntimeAPI = CefStartup(this);
	}

	virtual void ShutdownModule() override
	{
		if (CefRuntimeAPI)
		{
			CefRuntimeAPI->Release();
			CefRuntimeAPI = nullptr;
		}
	}

	/*virtual bool IsTickable() const override
	{
		return CefApplication.get() != NULL;
	}

	virtual void Tick(float DeltaTime) override
	{
		if (CefApplication.get() != NULL)
		{
			CefDoMessageLoopWork();
		}
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FRadiantUIModule, STATGROUP_Tickables);
	}

	virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}

	virtual bool IsTickableInEditor() const override
	{
		return true;
	}*/


};

IMPLEMENT_MODULE(FRadiantUIModule, RadiantUI)
