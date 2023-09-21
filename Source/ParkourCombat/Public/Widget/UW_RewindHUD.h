// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UW_WidgetWithInputDetection.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "UW_RewindHUD.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_DELEGATE(FOnFinishRewind);


UCLASS()
class PARKOURCOMBAT_API UUW_RewindHUD : public UUW_WidgetWithInputDetection
{
	GENERATED_BODY()
private:

	bool IsWidgetInitialized = false;
	
protected:

	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	void BindButtonFunctions();

	UFUNCTION()
	void TryFinishRewind() {OnFinishRewindDelegate.ExecuteIfBound();}
	
public:

	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;

	FOnFinishRewind OnFinishRewindDelegate;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	USlider* RewindSlider;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* RewindSliderMoveKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush RewindSliderMoveKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush RewindSliderMoveKeyImage_Keyboard;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* CancelButton;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* CancelButtonImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Keyboard;

	
};
