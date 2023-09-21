// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UW_WidgetWithInputDetection.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "UW_ControlScheme.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_DELEGATE(FOnExitControlScheme);

UCLASS()
class PARKOURCOMBAT_API UUW_ControlScheme : public UUW_WidgetWithInputDetection
{
	GENERATED_BODY()


	bool IsInitialized = false;
	




protected:
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* ControlSchemeImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush ControlSchemeBrush_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush ControlSchemeBrush_KeyBoardAndMouse;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* ExitButton;

	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* CancelKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Keyboard;
	
	
	UFUNCTION()
	void OnExitControlScheme();

	virtual void NativeConstruct() override;

public:

	FOnExitControlScheme OnExitControlSchemeDelegate;

	UFUNCTION()
	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	UFUNCTION()
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;

	
};
