// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UW_BasicMenu.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UW_GameOverMenu.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FOnExecuteRewindButton);

UCLASS()
class PARKOURCOMBAT_API UUW_GameOverMenu : public UUW_BasicMenu
{
	GENERATED_BODY()
protected:

	bool IsRewindButton = true;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* RewindButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RewindButtonText;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* BackToMainMenuButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* ExitGameButton;

	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* ConfirmKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush ConfirmKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush ConfirmKeyImage_Keyboard;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* NextAndPreviousKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush NextAndPreviousKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush NextAndPreviousKeyImage_Keyboard;
	

	virtual void NativeConstruct() override;

	void BindButtonFunctions();


public:

	void OnChangeRewindButtonToRetryButton();

	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;

	FOnExecuteRewindButton OnExecuteRewindButtonDelegate;
	FOnPlayerHUDVisibility OnPlayerHUDVisibilityDelegate;
	
	UFUNCTION()
	void OnButtonIsHovered_RewindButton() {if(MenuManager) MenuManager->OnHoverButton(RewindButton);}

	UFUNCTION()
	void OnButtonIsHovered_BackToMainMenuButton() {if(MenuManager) MenuManager->OnHoverButton(BackToMainMenuButton);}
	
	UFUNCTION()
	void OnButtonIsHovered_ExitButton() {if(MenuManager) MenuManager->OnHoverButton(ExitGameButton);}

	
	UFUNCTION()
	void OnButtonPressed_RewindButton() {if(MenuManager) MenuManager->OnExecuteButton(RewindButton);}

	UFUNCTION()
	void OnButtonPressed_BackToMainMenuButton() {if(MenuManager) MenuManager->OnExecuteButton(BackToMainMenuButton);}
	
	UFUNCTION()
	void OnButtonPressed_ExitButton() {if(MenuManager) MenuManager->OnExecuteButton(ExitGameButton);}


	UFUNCTION()
	void UseRewindAbility();

	UFUNCTION()
	void BackToMain();
	
	UFUNCTION()
	void ExitGame();

	
};
