// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Widget/UW_BasicMenu.h"
#include "UW_GameClearMenu.generated.h"

/**
 * 
 */


UCLASS()
class PARKOURCOMBAT_API UUW_GameClearMenu : public UUW_BasicMenu
{
	GENERATED_BODY()
	protected:
	
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


	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;
	
	UFUNCTION()
	void OnButtonIsHovered_BackToMainMenuButton() {if(MenuManager) MenuManager->OnHoverButton(BackToMainMenuButton);}
	
	UFUNCTION()
	void OnButtonIsHovered_ExitButton() {if(MenuManager) MenuManager->OnHoverButton(ExitGameButton);}

	
	UFUNCTION()
	void OnButtonPressed_BackToMainMenuButton() {if(MenuManager) MenuManager->OnExecuteButton(BackToMainMenuButton);}
	
	UFUNCTION()
	void OnButtonPressed_ExitButton() {if(MenuManager) MenuManager->OnExecuteButton(ExitGameButton);}
	
	UFUNCTION()
	void BackToMain();
	
	UFUNCTION()
	void ExitGame();

};
