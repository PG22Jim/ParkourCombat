// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_GameOverMenu.h"

#include "Kismet/GameplayStatics.h"

void UUW_GameOverMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		if(MenuManager)
		{
			MenuManager->AddAtTail(RewindButton);
			MenuManager->AddAtTail(BackToMainMenuButton);
			MenuManager->AddAtTail(ExitGameButton);
			
			MenuManager->SelectedMenu = MenuManager->HeadMenu;
		}

		BindButtonFunctions();
		IsWidgetInitialized = true;
	}


}

void UUW_GameOverMenu::BindButtonFunctions()
{
	RewindButton->OnHovered.AddDynamic(this, &UUW_GameOverMenu::OnButtonIsHovered_RewindButton);
	RewindButton->OnPressed.AddDynamic(this, &UUW_GameOverMenu::OnButtonPressed_RewindButton);
	RewindButton->SynchronizeProperties();
		
	BackToMainMenuButton->OnHovered.AddDynamic(this, &UUW_GameOverMenu::OnButtonIsHovered_BackToMainMenuButton);
	BackToMainMenuButton->OnPressed.AddDynamic(this, &UUW_GameOverMenu::OnButtonPressed_BackToMainMenuButton);
	BackToMainMenuButton->SynchronizeProperties();

	
	ExitGameButton->OnHovered.AddDynamic(this, &UUW_GameOverMenu::OnButtonIsHovered_ExitButton);
	ExitGameButton->OnPressed.AddDynamic(this, &UUW_GameOverMenu::OnButtonPressed_ExitButton);
	ExitGameButton->SynchronizeProperties();

	
	MenuManager->SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::Green);
	MenuManager->SelectedMenu->BindButtonFunction(this, "UseRewindAbility");
	MenuManager->SelectedMenu->NextNode->BindButtonFunction(this, "BackToMain");
	MenuManager->SelectedMenu->NextNode->NextNode->BindButtonFunction(this, "ExitGame");
}

void UUW_GameOverMenu::OnChangeRewindButtonToRetryButton()
{
	IsRewindButton = false;

	RewindButtonText->SetText(FText::FromString("Retry Stage"));
}

void UUW_GameOverMenu::OnUpdateControlUI(ControlType NewControlType)
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

void UUW_GameOverMenu::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);

	if(ReceiveKey == PlayerMenuInputType::ConfirmKey) ExecuteSelectedMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::UpKey) SelectPreviousMenuNode();
	else if(ReceiveKey == PlayerMenuInputType::DownKey) SelectNextMenuNode();

}

void UUW_GameOverMenu::UseRewindAbility()
{
	if(IsRewindButton)
	{
		if(OnExecuteRewindButtonDelegate.IsBound())
		{
			const UWorld* World = GetWorld();
			if(!World) return;
			
			// UnPause Game if game is paused
			if(UGameplayStatics::IsGamePaused(World))
				UGameplayStatics::SetGamePaused(World, false);	
			
			// Remove widget, execute delegate
			RemoveFromParent();
			OnExecuteRewindButtonDelegate.ExecuteIfBound();
			OnPlayerHUDVisibilityDelegate.ExecuteIfBound(true);
		}
	}
	else
	{
		const UWorld* World = GetWorld();
		if(!World) return;

		UGameplayStatics::OpenLevel(World, "ThirdPersonMap");
	}
}

void UUW_GameOverMenu::BackToMain()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	UGameplayStatics::OpenLevel(World, "MainMenu");
}

void UUW_GameOverMenu::ExitGame()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(AController* PlayerControl = UGameplayStatics::GetPlayerController(World,0))
		if(APlayerController* CastResult = Cast<APlayerController>(PlayerControl))
			UKismetSystemLibrary::QuitGame(World, CastResult, EQuitPreference::Quit, false);
}
