// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once 

#include "SlateCore.h"
#include "SCompoundWidget.h"
#include "RadiantWebView.h"
#include "RadiantJavaScriptFunctionCallTargetInterface.h"
#include "RadiantWebViewHUDElement.generated.h"

class ARadiantWebViewHUD;
class URadiantWebViewHUDElement;
struct CefRuntimeMouseEvent;

class SRadiantWebViewHUDElement : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRadiantWebViewHUDElement)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<ARadiantWebViewHUD>, HUDOwner)
	SLATE_ARGUMENT(TWeakObjectPtr<URadiantWebViewHUDElement>, HUDElement)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent) override;
	
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyboardEvent) override;

	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent);
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent);

private:

	friend class ARadiantWebViewHUD;

	void GetMouseState(const FGeometry& InGeometry, const FPointerEvent& InPointerEvent, CefRuntimeMouseEvent &OutMouseEvent);
	
	FVector2D AbsoluteToLocal(const FGeometry& MyGeometry, const FVector2D AbsolutePosition);

	FVector2D ScreenPosition;
	FVector2D ScreenSize;
	bool MouseCaptured;

	TWeakObjectPtr<ARadiantWebViewHUD> HUDOwner;
	TWeakObjectPtr<URadiantWebViewHUDElement> HUDElement;

};

UENUM(BlueprintType)
namespace ERadiantHUDElementInputMode
{
	enum Type
	{
		NoInput,
		MouseOnly,
		MouseAndKeyboard
	};
}

UENUM(BlueprintType)
namespace ERadiantHUDElementHitTest
{
	enum Type
	{
		None,
		Rect,
		Alpha
	};
}

UCLASS(abstract, DefaultToInstanced, Blueprintable)
class RADIANTUI_API URadiantWebViewHUDElement : public UObject, public IRadiantJavaScriptFunctionCallTargetInterface
{
	GENERATED_BODY()

public:

	URadiantWebViewHUDElement(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FRadiantWebView> WebView;

	UPROPERTY(EditDefaultsOnly, Category = "WebView")
	FRadiantWebViewDefaultSettings DefaultSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD|Element")
	uint32 bVisible:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Element")
	uint32 bMouseThumbNavigate:1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD|Element")
	TEnumAsByte<ERadiantHUDElementInputMode::Type> InputMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD|Element")
	TEnumAsByte<ERadiantHUDElementHitTest::Type> HitTest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD|Element")
	FVector2D ViewportResolutionFactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD|Element")
	uint32 bAutoMatchViewportResolution:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Element")
	FVector2D Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Element")
	FVector2D Size;

	UFUNCTION(BlueprintCallable, Category = "HUD|Element")
	void SetVisible(bool IsVisible);

	UFUNCTION(BlueprintCallable, Category = "HUD|Element")
	void SetHitTest(TEnumAsByte<ERadiantHUDElementHitTest::Type> InHitTest);

	UFUNCTION(BlueprintCallable, Category = "HUD|Element")
	void SetInputMode(TEnumAsByte<ERadiantHUDElementInputMode::Type> InInputMode);
	
	UFUNCTION(BlueprintCallable, Category = "WebView")
	bool CanNavigateForward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	bool CanNavigateBackward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void NavigateForward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void NavigateBackward();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void LoadURL(const FString& URL);

	UFUNCTION(BlueprintCallable, Category = "WebView")
	FString GetURL();

	UFUNCTION(BlueprintCallable, Category = "WebView")
	void SetRefreshRate(float FramesPerSecond);

	UFUNCTION(BlueprintCallable, Category = "WebView")
	float GetRefreshRate();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = JavaScript)
	TScriptInterface<IRadiantJavaScriptFunctionCallTargetInterface> GetJavaScriptCallContext();

	virtual void CallJavaScriptFunction(const FString& HookName, UObject* Parameters) override;

	// Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void Serialize(FArchive& Ar) override;
	virtual UWorld* GetWorld() const override;
	// End UObject Interface

private:

	UWorld* World;

	void SetSlateVisibility();
	void OnExecuteJSHook(const FString& HookName, ICefRuntimeVariantList* Arguments);
	
	friend class ARadiantWebViewHUD;

	TSharedPtr<class SWeakWidget> Container;
	TSharedPtr<SRadiantWebViewHUDElement> SWidget;
};