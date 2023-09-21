// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Widget/UW_BasicMenu.h"
#include "UW_PauseMenu.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FOnCallTutorial);
DECLARE_DYNAMIC_DELEGATE(FOnContinueGame);

UCLASS()
class PARKOURCOMBAT_API UUW_PauseMenu : public UUW_BasicMenu
{
	GENERATED_BODY()
private:

	
protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* ContinueButton;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* DisplayTutorialButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* DisplayControlSchemeButton;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* BackToMainMenuButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* ExitGameButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UGridPanel* InputDisplayGrid;
	
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
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* CancelKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Keyboard;

	
	
	
	virtual void NativeConstruct() override;

	void BindButtonFunctions();
	
	UFUNCTION()
	void OnButtonIsHovered_ContinueButton() {if(MenuManager) MenuManager->OnHoverButton(ContinueButton);}

	UFUNCTION()
	void OnButtonIsHovered_DisplayTutorialButton() {if(MenuManager) MenuManager->OnHoverButton(DisplayTutorialButton);}

	UFUNCTION()
	void OnButtonIsHovered_DisplayControlSchemeButton() {if(MenuManager) MenuManager->OnHoverButton(DisplayControlSchemeButton);}

	UFUNCTION()
	void OnButtonIsHovered_BackToMainMenuButton() {if(MenuManager) MenuManager->OnHoverButton(BackToMainMenuButton);}
	
	UFUNCTION()
	void OnButtonIsHovered_ExitButton() {if(MenuManager) MenuManager->OnHoverButton(ExitGameButton);}

	
	UFUNCTION()
	void OnButtonPressed_ContinueButton() {if(MenuManager) MenuManager->OnExecuteButton(ContinueButton);}

	UFUNCTION()
	void OnButtonPressed_DisplayTutorialButton() {if(MenuManager) MenuManager->OnExecuteButton(DisplayTutorialButton);}

	UFUNCTION()
	void OnButtonPressed_DisplayControlSchemeButton() {if(MenuManager) MenuManager->OnExecuteButton(DisplayControlSchemeButton);}

	UFUNCTION()
	void OnButtonPressed_BackToMainMenuButton() {if(MenuManager) MenuManager->OnExecuteButton(BackToMainMenuButton);}
	
	UFUNCTION()
	void OnButtonPressed_ExitButton() {if(MenuManager) MenuManager->OnExecuteButton(ExitGameButton);}


	UFUNCTION()
	void ContinueGame() { if(OnContinueGameDelegate.ExecuteIfBound()) OnPlayerHUDVisibilityDelegate.ExecuteIfBound(true); }
	
	UFUNCTION()
	void DisplayTutorialHUD() { CallTutorialDelegate.ExecuteIfBound();}
	
	UFUNCTION()
	void DisplayControlScheme() { OnShowControlSchemeDelegate.ExecuteIfBound(); }
	
	UFUNCTION()
	void BackToMain();
	
	UFUNCTION()
	void ExitGame();

public:

	FOnPlayerHUDVisibility OnPlayerHUDVisibilityDelegate;

	void SetInputDisplayGridVisibility(bool bVisibility);

	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;
	
	FOnCallTutorial CallTutorialDelegate;
	FOnContinueGame OnContinueGameDelegate;
};
