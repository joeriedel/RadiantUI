// Copyright 2014 Joseph Riedel. All Rights Reserved.

#include "Assert.h"
#include "Variants.h"
#include "CefApplication.h"
#include <string>

class JSHookExtensionHandler : public CefV8Handler
{
	IMPLEMENT_REFCOUNTING(JSHookExtensionHandler);
public:
	explicit JSHookExtensionHandler(CefRefPtr<CefFrameworkApp> InApp) : App(InApp) {}

	virtual bool Execute(const CefString& InName,
		CefRefPtr<CefV8Value> InObject,
		const CefV8ValueList& InArguments,
		CefRefPtr<CefV8Value>& OutRetval,
		CefString& OutException) 
	{
		bool Handled = false;
		bool ValidAPI = true;

		if (InName == "TriggerEvent")
		{
			// dispatch hook to the browser process for execution on the game thread
			if ((InArguments.size() == 1) && InArguments[0]->IsArray())
			{
				CefRefPtr<CefV8Value> ArgumentArray = InArguments[0];
				if (ArgumentArray->GetArrayLength() > 0)
				{
					CefRefPtr<CefBrowser> Browser = CefV8Context::GetCurrentContext()->GetBrowser();
					ASSERT(Browser.get());

					CefString HookName = ArgumentArray->GetValue(0)->GetStringValue();
					if (!HookName.empty())
					{
						CefRefPtr<CefProcessMessage> Message = CefProcessMessage::Create(HookName);

						// translate remaining args.
						if ((ArgumentArray->GetArrayLength() > 1) && ArgumentArray->GetValue(1)->IsArray())
						{
							CefRefPtr<CefV8Value> InParameters = ArgumentArray->GetValue(1);
							const int NumParameters = InParameters->GetArrayLength();

							CefRefPtr<CefListValue> OutParameters = Message->GetArgumentList();
							OutParameters->SetSize(NumParameters);

							for (int i = 0; i < (int)NumParameters; ++i)
							{
								V8ValueToListItem_RenderThread(InParameters->GetValue(i), OutParameters, i);
							}
						}

						Browser->SendProcessMessage(PID_BROWSER, Message);
						Handled = true;
					}
				}
			}
		}
		else if (InName == "SetHook")
		{
			if ((InArguments.size() == 2) && (InArguments[0]->IsString()) && (InArguments[1]->IsFunction()))
			{
				CefString HookName = InArguments[0]->GetStringValue().ToString();
				CefRefPtr<CefV8Context> Context = CefV8Context::GetCurrentContext();
				App->SetJSHook(HookName, Context->GetBrowser()->GetIdentifier(), Context, InArguments[1]);
				Handled = true;
			}
		}
		else if (InName == "RemoveHook")
		{
			if ((InArguments.size() == 1) && InArguments[0]->IsString())
			{
				CefRefPtr<CefV8Context> Context = CefV8Context::GetCurrentContext();
				App->RemoveJSHook(InArguments[0]->GetStringValue(), Context->GetBrowser()->GetIdentifier());
				Handled = true;
			}
		}
		else
		{
			ValidAPI = false;
			OutException = std::string("Unrecognized JSHook API Call: '") + InName.ToString() + std::string("'");
		}

		if (!Handled && ValidAPI)
		{
			OutException = std::string("Invalid Arguments Passed To '") + InName.ToString() + std::string("'");
		}

		return Handled;
	}
private:

	CefRefPtr<CefFrameworkApp> App;
};

void CefFrameworkApp::SetJSHook(
	const CefString& InHookName,
	int InBrowserID,
	CefRefPtr<CefV8Context> InContext,
	CefRefPtr<CefV8Value> InFunction
	)
{
	REQUIRE_RENDER_THREAD()

	JSHook Hook;
	Hook.Context = InContext;
	Hook.Function = InFunction;

	Hooks.insert(JSHookMap::value_type(std::make_pair(InHookName.ToString(), InBrowserID), Hook));
}

void CefFrameworkApp::RemoveJSHook(const CefString& InHookName, int InBrowserId)
{
	REQUIRE_RENDER_THREAD()

	Hooks.erase(std::make_pair(InHookName.ToString(), InBrowserId));
}

// CefRenderProcessHandler methods.
void CefFrameworkApp::OnWebKitInitialized()
{
	// Register our hook extension
	std::string script =
		"var RadiantUI;"
		"if (!RadiantUI)"
		"  RadiantUI = {};"
		"(function() {"
		"  RadiantUI.TriggerEvent = function() {"
		"    native function TriggerEvent();"
		"    return TriggerEvent(Array.prototype.slice.call(arguments));"
		"  };"
		"  RadiantUI.SetCallback = function(name, callback) {"
		"    native function SetHook();"
		"    return SetHook(name, callback);"
		"  };"
		"  RadiantUI.RemoveCallback = function(name) {"
		"    native function RemoveHook();"
		"    return RemoveHook(name);"
		"  };"
		"})();";
	CefRegisterExtension("RadiantUI JSHooks Extension", script, new JSHookExtensionHandler(this));
}

void CefFrameworkApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefV8Context> context)
{
	for (JSHookMap::iterator it = Hooks.begin(); it != Hooks.end();)
	{
		if (it->second.Context->IsSame(context))
		{
			JSHookMap::iterator next = it; ++next;
			Hooks.erase(it);
			it = next;
		}
		else
		{
			++it;
		}
	}
}

void CefFrameworkApp::OnFocusedNodeChanged(
	CefRefPtr<CefBrowser> InBrowser,
	CefRefPtr<CefFrame> InFrame,
	CefRefPtr<CefDOMNode> InNode)
{
	bool IsEditableNode = InNode.get() && InNode->IsEditable();

	CefRefPtr<CefProcessMessage> Message = CefProcessMessage::Create(RADUIIPCMSG_FOCUSNODECHANGED);
	Message->GetArgumentList()->SetBool(0, IsEditableNode);
	InBrowser->SendProcessMessage(PID_BROWSER, Message);
}

bool CefFrameworkApp::OnProcessMessageReceived(
	CefRefPtr<CefBrowser> InBrowser,
	CefProcessId InSourceProcess,
	CefRefPtr<CefProcessMessage> InMessage)
{
	ASSERT(InSourceProcess == PID_BROWSER); // call should have come from browser process.

	if (!Hooks.empty())
	{
		CefString MessageName = InMessage->GetName();
		JSHookMap::iterator it = Hooks.find(std::make_pair(MessageName, InBrowser->GetIdentifier()));
		if (it != Hooks.end())
		{
			// invoke JS callback
			JSHook Hook(it->second);

			Hook.Context->Enter();

			CefRefPtr<CefListValue> MessageArguments = InMessage->GetArgumentList();
			const int NumMessageArguments = (int)MessageArguments->GetSize();

			// convert message arguments
			CefV8ValueList Arguments;

			for (int i = 0; i < NumMessageArguments; ++i)
			{
				Arguments.push_back(ListItemToV8Value_RenderThread(MessageArguments, i));
			}

			Hook.Function->ExecuteFunction(nullptr, Arguments);
			Hook.Context->Exit();
			return true;
		}
	}

	return false;
}