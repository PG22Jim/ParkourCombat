// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Widget/UW_BasicMenu.h"
#include "UW_MainMenu.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FOnOpenSelectDifficulty);

UCLASS()
class PARKOURCOMBAT_API UUW_MainMenu : public UUW_BasicMenu
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* StartGameButton;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* StartTutorialButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* DisplayControlSchemeButton;

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

	void BindButtonDelegates();
	

	UFUNCTION()
	void OnButtonIsHovered_StartGameButton() {if(MenuManager) MenuManager->OnHoverButton(StartGameButton);}

	UFUNCTION()
	void OnButtonIsHovered_StartTutorialButton() {if(MenuManager) MenuManager->OnHoverButton(StartTutorialButton);}

	UFUNCTION()
	void OnButtonIsHovered_DisplayControlSchemeButton() {if(MenuManager) MenuManager->OnHoverButton(DisplayControlSchemeButton);}

	UFUNCTION()
	void OnButtonIsHovered_ExitButton() {if(MenuManager) MenuManager->OnHoverButton(ExitGameButton);}

	UFUNCTION()
	void OnButtonPressed_StartGameButton() {if(MenuManager) MenuManager->OnExecuteButton(StartGameButton);}

	UFUNCTION()
	void OnButtonPressed_StartTutorialButton() {if(MenuManager) MenuManager->OnExecuteButton(StartTutorialButton);}

	UFUNCTION()
	void OnButtonPressed_DisplayControlSchemeButton() {if(MenuManager) MenuManager->OnExecuteButton(DisplayControlSchemeButton);}
	
	UFUNCTION()
	void OnButtonPressed_ExitButton() {if(MenuManager) MenuManager->OnExecuteButton(ExitGameButton);}

	UFUNCTION()
	void StartGame();
	
	UFUNCTION()
	void StartTutorial();
	
	UFUNCTION()
	void DisplayControlScheme() { OnShowControlSchemeDelegate.ExecuteIfBound(); }

	UFUNCTION()
	void ExitGame();

public:

	FOnOpenSelectDifficulty OnOpenSelectDifficultyDelegate;
	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;

};
