// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "Enemy/EnemyEnum.h"
#include "Widget/UW_BasicMenu.h"
#include "UW_SelectDifficultyMenu.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FOnCancelDifficultySelection);

UCLASS()
class PARKOURCOMBAT_API UUW_SelectDifficultyMenu : public UUW_BasicMenu
{
	GENERATED_BODY()

	protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* SelectDifficultyButton_Easy;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* SelectDifficultyButton_Normal;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* SelectDifficultyButton_Hard;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* CancelButton;
	

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
	void OnButtonIsHovered_SelectDifficultyButton_Easy() {if(MenuManager) MenuManager->OnHoverButton(SelectDifficultyButton_Easy);}

	UFUNCTION()
	void OnButtonIsHovered_SelectDifficultyButton_Normal() {if(MenuManager) MenuManager->OnHoverButton(SelectDifficultyButton_Normal);}

	UFUNCTION()
	void OnButtonIsHovered_SelectDifficultyButton_Hard() {if(MenuManager) MenuManager->OnHoverButton(SelectDifficultyButton_Hard);}

	UFUNCTION()
	void OnButtonIsHovered_CancelButton() {if(MenuManager) MenuManager->OnHoverButton(CancelButton);}
	

	
	UFUNCTION()
	void OnButtonPressed_SelectDifficultyButton_Easy() {if(MenuManager) MenuManager->OnExecuteButton(SelectDifficultyButton_Easy);}

	UFUNCTION()
	void OnButtonPressed_SelectDifficultyButton_Normal() {if(MenuManager) MenuManager->OnExecuteButton(SelectDifficultyButton_Normal);}

	UFUNCTION()
	void OnButtonPressed_SelectDifficultyButton_Hard() {if(MenuManager) MenuManager->OnExecuteButton(SelectDifficultyButton_Hard);}

	UFUNCTION()
	void OnButtonPressed_CancelButton() {if(MenuManager) MenuManager->OnExecuteButton(CancelButton);}
	

	
	UFUNCTION()
	void SelectDifficultyEasy() { if(OnSelectDifficulty(EnemyDifficulty::Easy)) OpenStage(); }
	
	UFUNCTION()
	void SelectDifficultyNormal() { if(OnSelectDifficulty(EnemyDifficulty::Normal)) OpenStage(); }
	
	UFUNCTION()
	void SelectDifficultyHard() { if(OnSelectDifficulty(EnemyDifficulty::Hard)) OpenStage(); }

	UFUNCTION()
	void CancelDifficultySelectMenu() { OnCancelDifficultySelectionDelegate.ExecuteIfBound(); }


	bool OnSelectDifficulty(EnemyDifficulty SelectDifficulty);

	void OpenStage();
	
	
public:

	FOnCancelDifficultySelection OnCancelDifficultySelectionDelegate;

	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;


	
};
