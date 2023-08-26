// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widget/UW_BasicMenu.h"
#include "UW_MainMenu.generated.h"

/**
 * 
 */
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
	void DisplayControlScheme() { GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Show Control Scheme!")); }

	UFUNCTION()
	void ExitGame() {GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Exit Game!"));}
};
