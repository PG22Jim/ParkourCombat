// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_PauseMenu.h"

#include "Kismet/GameplayStatics.h"

void UUW_PauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		if(MenuManager)
		{
			MenuManager->AddAtTail(ContinueButton);
			MenuManager->AddAtTail(DisplayTutorialButton);
			MenuManager->AddAtTail(DisplayControlSchemeButton);
			MenuManager->AddAtTail(BackToMainMenuButton);
			MenuManager->AddAtTail(ExitGameButton);
			
			MenuManager->SelectedMenu = MenuManager->HeadMenu;
		}

		BindButtonFunctions();
		IsWidgetInitialized = true;
	}
}

void UUW_PauseMenu::BindButtonFunctions()
{
	//if(!ContinueButton || !DisplayTutorialButton || !DisplayControlSchemeButton || !ExitTutorialButton || !ExitGameButton || !MenuManager || MenuManager->ListSize < 5) return;
	
	ContinueButton->OnHovered.AddDynamic(this, &UUW_PauseMenu::OnButtonIsHovered_ContinueButton);
	ContinueButton->OnPressed.AddDynamic(this, &UUW_PauseMenu::OnButtonPressed_ContinueButton);
	ContinueButton->SynchronizeProperties();
	
	DisplayTutorialButton->OnHovered.AddDynamic(this, &UUW_PauseMenu::OnButtonIsHovered_DisplayTutorialButton);
	DisplayTutorialButton->OnPressed.AddDynamic(this, &UUW_PauseMenu::OnButtonPressed_DisplayTutorialButton);
	DisplayTutorialButton->SynchronizeProperties();
	
	DisplayControlSchemeButton->OnHovered.AddDynamic(this, &UUW_PauseMenu::OnButtonIsHovered_DisplayControlSchemeButton);
	DisplayControlSchemeButton->OnPressed.AddDynamic(this, &UUW_PauseMenu::OnButtonPressed_DisplayControlSchemeButton);
	DisplayControlSchemeButton->SynchronizeProperties();

		
	BackToMainMenuButton->OnHovered.AddDynamic(this, &UUW_PauseMenu::OnButtonIsHovered_BackToMainMenuButton);
	BackToMainMenuButton->OnPressed.AddDynamic(this, &UUW_PauseMenu::OnButtonPressed_BackToMainMenuButton);
	BackToMainMenuButton->SynchronizeProperties();

	
	ExitGameButton->OnHovered.AddDynamic(this, &UUW_PauseMenu::OnButtonIsHovered_ExitButton);
	ExitGameButton->OnPressed.AddDynamic(this, &UUW_PauseMenu::OnButtonPressed_ExitButton);
	ExitGameButton->SynchronizeProperties();

	
	MenuManager->SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::Green);
	MenuManager->SelectedMenu->BindButtonFunction(this, "ContinueGame");
	MenuManager->SelectedMenu->NextNode->BindButtonFunction(this, "DisplayTutorialHUD");
	MenuManager->SelectedMenu->NextNode->NextNode->BindButtonFunction(this, "DisplayControlScheme");
	MenuManager->SelectedMenu->NextNode->NextNode->NextNode->BindButtonFunction(this, "BackToMain");
	MenuManager->SelectedMenu->NextNode->NextNode->NextNode->NextNode->BindButtonFunction(this, "ExitGame");
}

void UUW_PauseMenu::SetInputDisplayGridVisibility(bool bVisibility)
{
	if(bVisibility)
		InputDisplayGrid->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	else
		InputDisplayGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UUW_PauseMenu::OnUpdateControlUI(ControlType NewControlType)
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

void UUW_PauseMenu::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);

	if(ReceiveKey == PlayerMenuInputType::BackKey) ContinueGame();
	else if(ReceiveKey == PlayerMenuInputType::ConfirmKey) ExecuteSelectedMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::UpKey) SelectPreviousMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::DownKey) SelectNextMenuNode();
}

void UUW_PauseMenu::BackToMain()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	UGameplayStatics::OpenLevel(World, "MainMenu");
}

void UUW_PauseMenu::ExitGame()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(AController* PlayerControl = UGameplayStatics::GetPlayerController(World,0))
		if(APlayerController* CastResult = Cast<APlayerController>(PlayerControl))
			UKismetSystemLibrary::QuitGame(World, CastResult, EQuitPreference::Quit, false);
}
