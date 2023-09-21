// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_WidgetWithInputDetection.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class PlayerMenuInputType: uint8
{
	UpKey = 0 UMETA(DisplayName = "UpKey"),
	DownKey = 1 UMETA(DisplayName = "DownKey"),
	LeftKey = 2 UMETA(DisplayName = "LeftKey"),
	RightKey = 3 UMETA(DisplayName = "RightKey"),
	ConfirmKey = 4 UMETA(DisplayName = "ConfirmKey"),
	BackKey = 5 UMETA(DisplayName = "BackKey"),
	NULLKey = 6 UMETA(DisplayName = "NULLKey"),
};

UENUM(BlueprintType)
enum ControlType
{
	KeyBoardAndMouse = 0 UMETA(DisplayName = "KeyBoardAndMouse"),
	Gamepad = 1 UMETA(DisplayName = "Gamepad"),
};


//DECLARE_DYNAMIC_DELEGATE_RetVal(ControlType, FOnCheckCurrentControlType);
DECLARE_DYNAMIC_DELEGATE(FOnShowControlScheme);

UCLASS()
class PARKOURCOMBAT_API UUW_WidgetWithInputDetection : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	bool IsWidgetOnScreen = false;

	

public:

	//FOnCheckCurrentControlType OnCheckCurrentControlDelegate;

	FOnShowControlScheme OnShowControlSchemeDelegate;

	virtual void OnUpdateControlUI(ControlType NewControlType);

	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey);


	bool GetIsWidgetOnScreen() const {return IsWidgetOnScreen;}
	void SetIsWidgetOnScreen(bool NewSet) {IsWidgetOnScreen = NewSet;}
	
};
