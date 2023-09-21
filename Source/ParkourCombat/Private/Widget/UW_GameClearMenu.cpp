// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_GameClearMenu.h"

#include "Kismet/GameplayStatics.h"

void UUW_GameClearMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		if(MenuManager)
		{
			MenuManager->AddAtTail(BackToMainMenuButton);
			MenuManager->AddAtTail(ExitGameButton);
			
			MenuManager->SelectedMenu = MenuManager->HeadMenu;
		}

		BindButtonFunctions();
		IsWidgetInitialized = true;
	}
}

void UUW_GameClearMenu::BindButtonFunctions()
{

	BackToMainMenuButton->OnHovered.AddDynamic(this, &UUW_GameClearMenu::OnButtonIsHovered_BackToMainMenuButton);
	BackToMainMenuButton->OnPressed.AddDynamic(this, &UUW_GameClearMenu::OnButtonPressed_BackToMainMenuButton);
	BackToMainMenuButton->SynchronizeProperties();

	
	ExitGameButton->OnHovered.AddDynamic(this, &UUW_GameClearMenu::OnButtonIsHovered_ExitButton);
	ExitGameButton->OnPressed.AddDynamic(this, &UUW_GameClearMenu::OnButtonPressed_ExitButton);
	ExitGameButton->SynchronizeProperties();

	
	MenuManager->SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::Green);
	MenuManager->SelectedMenu->BindButtonFunction(this, "BackToMain");
	MenuManager->SelectedMenu->NextNode->BindButtonFunction(this, "ExitGame");
}

void UUW_GameClearMenu::OnUpdateControlUI(ControlType NewControlType)
{
	Super::OnUpdateControlUI(NewControlType);

	if(NewControlType == ControlType::Gamepad)
	{
		ConfirmKeyImage->SetBrush(ConfirmKeyImage_Gamepad);
		NextAndPreviousKeyImage->SetBrush(NextAndPreviousKeyImage_Gamepad);
	}
	else
	{
		ConfirmKeyImage->SetBrush(ConfirmKeyImage_Keyboard);
		NextAndPreviousKeyImage->SetBrush(NextAndPreviousKeyImage_Keyboard);
	}
}

void UUW_GameClearMenu::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);

	if(ReceiveKey == PlayerMenuInputType::ConfirmKey) ExecuteSelectedMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::UpKey) SelectPreviousMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::DownKey) SelectNextMenuNode();
	
}

void UUW_GameClearMenu::BackToMain()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	UGameplayStatics::OpenLevel(World, "MainMenu");
}

void UUW_GameClearMenu::ExitGame()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(AController* PlayerControl = UGameplayStatics::GetPlayerController(World,0))
		if(APlayerController* CastResult = Cast<APlayerController>(PlayerControl))
			UKismetSystemLibrary::QuitGame(World, CastResult, EQuitPreference::Quit, false);
}
