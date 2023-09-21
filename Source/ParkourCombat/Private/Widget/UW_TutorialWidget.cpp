// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_TutorialWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UUW_TutorialWidget::InitializeTutorialData()
{
	const int32 ImageListSize = TutorialImages.Num();
	
	for (int32 i = 0; i < ImageListSize; i++)
	{
		UTexture2D* IterateImageTexture = TutorialImages[i];
		const FString IterateSectionName = SectionNames[i];
		const FString IterateDescription = Descriptions[i];
		UTexture2D* IterateButtonTexture_Gamepad = ButtonImages_Gamepad[i];
		UTexture2D* IterateButtonTexture_Keyboard = ButtonImages_Keyboard[i];
		const FString IterateButtonText = ButtonTexts[i];
		
		TutorialPageData IteratePageData = TutorialPageData(IterateImageTexture, IterateSectionName, IterateDescription, IterateButtonTexture_Gamepad, IterateButtonTexture_Keyboard, IterateButtonText);
		PageDataList.Add(IteratePageData);
	}
}

void UUW_TutorialWidget::BindButtonFunctions()
{
	NextPageButton->OnPressed.AddDynamic(this, &UUW_TutorialWidget::OnNextTutorial);
	NextPageButton->SynchronizeProperties();

	PreviousPageButton->OnPressed.AddDynamic(this, &UUW_TutorialWidget::OnPreviousTutorial);
	PreviousPageButton->SynchronizeProperties();
	
	ExitTutorialMenuButton->OnPressed.AddDynamic(this, &UUW_TutorialWidget::ExitTutorial);
	ExitTutorialMenuButton->SynchronizeProperties();
}

void UUW_TutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		InitializeTutorialData();
		
		BindButtonFunctions();

		UpdateTutorialPage();

		IsWidgetInitialized = true;
	}
}

void UUW_TutorialWidget::OnUpdateControlUI(ControlType NewControlType)
{
	Super::OnUpdateControlUI(NewControlType);

	// TODO: Later Change Input Key Image

	if(NewControlType == ControlType::Gamepad)
	{
		CancelKeyImage->SetBrush(CancelKeyImage_Gamepad);
		NextAndPreviousKeyImage->SetBrush(NextAndPreviousKeyImage_Gamepad);
	}
	else
	{
		CancelKeyImage->SetBrush(CancelKeyImage_Keyboard);
		NextAndPreviousKeyImage->SetBrush(NextAndPreviousKeyImage_Keyboard);
	}

}

void UUW_TutorialWidget::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);

	if(ReceiveKey == PlayerMenuInputType::BackKey) ExitTutorialMenuButton->OnPressed.Broadcast();
	else if(ReceiveKey == PlayerMenuInputType::LeftKey) PreviousPageButton->OnPressed.Broadcast();
	else if(ReceiveKey == PlayerMenuInputType::RightKey) NextPageButton->OnPressed.Broadcast();
	
}

void UUW_TutorialWidget::OnNextTutorial()
{
	if(CurrentTutorialPageIndex == PageDataList.Num() - 1 || CurrentTutorialPageIndex == UnlockedPageNum - 1) return;

	CurrentTutorialPageIndex++;
	UpdateTutorialPage();
}

void UUW_TutorialWidget::OnPreviousTutorial()
{
	if(CurrentTutorialPageIndex == 0) return;

	CurrentTutorialPageIndex--;
	UpdateTutorialPage();
}

void UUW_TutorialWidget::ExitTutorial()
{
	RemoveFromParent();

	
	if(OnCheckPauseMenuIsCloseDelegate.IsBound())
	{
		OnExitTutorialDelegate.ExecuteIfBound();
		const bool PauseMenuClosed = OnCheckPauseMenuIsCloseDelegate.Execute();
		if(PauseMenuClosed)
		{
			const UWorld* World = GetWorld();
			if(!World) return;
			
			UGameplayStatics::SetGamePaused(World, false);
			UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = false;
			FOnPlayerHUDShowOrHideDelegate.ExecuteIfBound(true);
		}
	}
}

void UUW_TutorialWidget::UpdateTutorialPage()
{
	TutorialPageData IndexData = PageDataList[CurrentTutorialPageIndex];

	DisplayImage->SetBrushFromTexture(IndexData.GetPageImageTexture());

	DisplayDescription->SetText(FText::FromString(IndexData.GetTutorialDescription()));
	DisplaySectionName->SetText(FText::FromString(IndexData.GetTutorialSectionName()));
	DisplayButtonText->SetText(FText::FromString(IndexData.GetButtonPressText()));

	DisplayButtonImg_Gamepad->SetBrushFromTexture(IndexData.GetButtonPressTexture_Gamepad());
	DisplayButtonImg_Keyboard->SetBrushFromTexture(IndexData.GetButtonPressTexture_Keyboard());
}

void UUW_TutorialWidget::FlipToNewestPage()
{
	CurrentTutorialPageIndex = UnlockedPageNum - 1;
	
	if(CurrentTutorialPageIndex < 3)
	{
		CurrentTutorialPageIndex = 0;
	}
	else if(CurrentTutorialPageIndex == 7)
	{
		CurrentTutorialPageIndex = 6;
	}
	else if(CurrentTutorialPageIndex == 9)
	{
		CurrentTutorialPageIndex = 8;
	}
	
	UpdateTutorialPage();
}

void UUW_TutorialWidget::UnlockedPageNumIncrement(const int32 UnlockPageCount)
{
	UnlockedPageNum = UKismetMathLibrary::Clamp(UnlockedPageNum+UnlockPageCount,0,PageDataList.Num());

	if(UnlockedPageNum == 4) OnTryToUnlockAbilityDelegate.ExecuteIfBound(TutorialUnLockAbility::RangeAttack);
	if(UnlockedPageNum == 5) OnTryToUnlockAbilityDelegate.ExecuteIfBound(TutorialUnLockAbility::RageAttack);
	if(UnlockedPageNum == 6) OnTryToUnlockAbilityDelegate.ExecuteIfBound(TutorialUnLockAbility::Heal);
	if(UnlockedPageNum == 8) OnTryToUnlockAbilityDelegate.ExecuteIfBound(TutorialUnLockAbility::Guard);
	if(UnlockedPageNum == 10) OnTryToUnlockAbilityDelegate.ExecuteIfBound(TutorialUnLockAbility::Dodge);
	if(UnlockedPageNum == 11) OnTryToUnlockAbilityDelegate.ExecuteIfBound(TutorialUnLockAbility::Rewind);
	
}
