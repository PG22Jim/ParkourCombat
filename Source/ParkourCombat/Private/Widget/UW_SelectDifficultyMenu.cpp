// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_SelectDifficultyMenu.h"

#include "ParkourCombatGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UUW_SelectDifficultyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		if(MenuManager)
		{
			MenuManager->AddAtTail(SelectDifficultyButton_Easy);
			MenuManager->AddAtTail(SelectDifficultyButton_Normal);
			MenuManager->AddAtTail(SelectDifficultyButton_Hard);
			MenuManager->AddAtTail(CancelButton);
			
			MenuManager->SelectedMenu = MenuManager->HeadMenu;
		}

		BindButtonFunctions();
		IsWidgetInitialized = true;
	}
}

void UUW_SelectDifficultyMenu::BindButtonFunctions()
{
	SelectDifficultyButton_Easy->OnHovered.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonIsHovered_SelectDifficultyButton_Easy);
	SelectDifficultyButton_Easy->OnPressed.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonPressed_SelectDifficultyButton_Easy);
	SelectDifficultyButton_Easy->SynchronizeProperties();
	
	SelectDifficultyButton_Normal->OnHovered.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonIsHovered_SelectDifficultyButton_Normal);
	SelectDifficultyButton_Normal->OnPressed.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonPressed_SelectDifficultyButton_Normal);
	SelectDifficultyButton_Normal->SynchronizeProperties();
	
	SelectDifficultyButton_Hard->OnHovered.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonIsHovered_SelectDifficultyButton_Hard);
	SelectDifficultyButton_Hard->OnPressed.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonPressed_SelectDifficultyButton_Hard);
	SelectDifficultyButton_Hard->SynchronizeProperties();

		
	CancelButton->OnHovered.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonIsHovered_CancelButton);
	CancelButton->OnPressed.AddDynamic(this, &UUW_SelectDifficultyMenu::OnButtonPressed_CancelButton);
	CancelButton->SynchronizeProperties();
	
	
	MenuManager->SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::Green);
	MenuManager->SelectedMenu->BindButtonFunction(this, "SelectDifficultyEasy");
	MenuManager->SelectedMenu->NextNode->BindButtonFunction(this, "SelectDifficultyNormal");
	MenuManager->SelectedMenu->NextNode->NextNode->BindButtonFunction(this, "SelectDifficultyHard");
	MenuManager->SelectedMenu->NextNode->NextNode->NextNode->BindButtonFunction(this, "CancelDifficultySelectMenu");
}

bool UUW_SelectDifficultyMenu::OnSelectDifficulty(EnemyDifficulty SelectDifficulty)
{
	const UWorld* World = GetWorld();
	if(!World) return false;
	
	UGameInstance* ReturnInstance = UGameplayStatics::GetGameInstance(World);
	if(!ReturnInstance) return false;

	UParkourCombatGameInstance* CastResult = Cast<UParkourCombatGameInstance>(ReturnInstance);
	if(!CastResult) return false;

	CastResult->SetStoredDifficulty(SelectDifficulty);
	if(CastResult->GetStoredDifficulty() == SelectDifficulty) return true;

	return false;
}

void UUW_SelectDifficultyMenu::OpenStage()
{
	const UWorld* World = GetWorld();
	if(!World) return;
	
	UGameplayStatics::OpenLevel(World, "ThirdPersonMap");
}

void UUW_SelectDifficultyMenu::OnUpdateControlUI(ControlType NewControlType)
{
	Super::OnUpdateControlUI(NewControlType);

	if(NewControlType == ControlType::Gamepad)
	{
		ConfirmKeyImage->SetBrush(ConfirmKeyImage_Gamepad);
		NextAndPreviousKeyImage->SetBrush(NextAndPreviousKeyImage_Gamepad);
		CancelKeyImage->SetBrush(CancelKeyImage_Gamepad);
	}
	else
	{
		ConfirmKeyImage->SetBrush(ConfirmKeyImage_Keyboard);
		NextAndPreviousKeyImage->SetBrush(NextAndPreviousKeyImage_Keyboard);
		CancelKeyImage->SetBrush(CancelKeyImage_Keyboard);
	}

}

void UUW_SelectDifficultyMenu::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);

	if(ReceiveKey == PlayerMenuInputType::BackKey) CancelDifficultySelectMenu();
	else if(ReceiveKey == PlayerMenuInputType::ConfirmKey) ExecuteSelectedMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::UpKey) SelectPreviousMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::DownKey) SelectNextMenuNode();
	
}
