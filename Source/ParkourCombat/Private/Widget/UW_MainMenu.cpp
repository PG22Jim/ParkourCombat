// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_MainMenu.h"

#include "Kismet/GameplayStatics.h"

void UUW_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		if(MenuManager)
		{
			MenuManager->AddAtTail(StartGameButton);
			MenuManager->AddAtTail(StartTutorialButton);
			MenuManager->AddAtTail(DisplayControlSchemeButton);
			MenuManager->AddAtTail(ExitGameButton);
			
			MenuManager->SelectedMenu = MenuManager->HeadMenu;
		}

		BindButtonDelegates();
		MenuManager->SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::Green);
		MenuManager->SelectedMenu->BindButtonFunction(this, "StartGame");
		MenuManager->SelectedMenu->NextNode->BindButtonFunction(this, "StartTutorial");
		MenuManager->SelectedMenu->NextNode->NextNode->BindButtonFunction(this, "DisplayControlScheme");
		MenuManager->SelectedMenu->NextNode->NextNode->NextNode->BindButtonFunction(this, "ExitGame");

		IsWidgetInitialized = true;
	}
}

void UUW_MainMenu::BindButtonDelegates()
{
	StartGameButton->OnHovered.AddDynamic(this, &UUW_MainMenu::OnButtonIsHovered_StartGameButton);
	StartGameButton->OnPressed.AddDynamic(this, &UUW_MainMenu::OnButtonPressed_StartGameButton);
	StartGameButton->SynchronizeProperties();
	
	StartTutorialButton->OnHovered.AddDynamic(this, &UUW_MainMenu::OnButtonIsHovered_StartTutorialButton);
	StartTutorialButton->OnPressed.AddDynamic(this, &UUW_MainMenu::OnButtonPressed_StartTutorialButton);
	StartTutorialButton->SynchronizeProperties();
	
	DisplayControlSchemeButton->OnHovered.AddDynamic(this, &UUW_MainMenu::OnButtonIsHovered_DisplayControlSchemeButton);
	DisplayControlSchemeButton->OnPressed.AddDynamic(this, &UUW_MainMenu::OnButtonPressed_DisplayControlSchemeButton);
	DisplayControlSchemeButton->SynchronizeProperties();

	ExitGameButton->OnHovered.AddDynamic(this, &UUW_MainMenu::OnButtonIsHovered_ExitButton);
	ExitGameButton->OnPressed.AddDynamic(this, &UUW_MainMenu::OnButtonPressed_ExitButton);
	ExitGameButton->SynchronizeProperties();
}

void UUW_MainMenu::OnUpdateControlUI(ControlType NewControlType)
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

void UUW_MainMenu::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);
	
	if(ReceiveKey == PlayerMenuInputType::ConfirmKey) ExecuteSelectedMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::UpKey) SelectPreviousMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::DownKey) SelectNextMenuNode();
}

void UUW_MainMenu::StartGame()
{
	OnOpenSelectDifficultyDelegate.ExecuteIfBound();
	// const UWorld* World = GetWorld();
	// if(!World) return;
	//
	// UGameplayStatics::OpenLevel(World, "ThirdPersonMap");
}

void UUW_MainMenu::StartTutorial()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	UGameplayStatics::OpenLevel(World, "Tutorial");
}

void UUW_MainMenu::ExitGame()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(AController* PlayerControl = UGameplayStatics::GetPlayerController(World,0))
		if(APlayerController* CastResult = Cast<APlayerController>(PlayerControl))
			UKismetSystemLibrary::QuitGame(World, CastResult, EQuitPreference::Quit, false);
}
