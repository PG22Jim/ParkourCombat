// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_RewindHUD.h"

void UUW_RewindHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		BindButtonFunctions();
		IsWidgetInitialized = true;
	}
}

bool UUW_RewindHUD::Initialize()
{
	
	
	
	return Super::Initialize();

}

void UUW_RewindHUD::BindButtonFunctions()
{
	CancelButton->OnPressed.AddDynamic(this, &UUW_RewindHUD::TryFinishRewind);
	CancelButton->SynchronizeProperties();
}

void UUW_RewindHUD::OnUpdateControlUI(ControlType NewControlType)
{
	Super::OnUpdateControlUI(NewControlType);
	if(NewControlType == ControlType::Gamepad)
	{
		CancelButtonImage->SetBrush(CancelKeyImage_Gamepad);
		RewindSliderMoveKeyImage->SetBrush(RewindSliderMoveKeyImage_Gamepad);
	}
	else
	{
		CancelButtonImage->SetBrush(CancelKeyImage_Keyboard);
		RewindSliderMoveKeyImage->SetBrush(RewindSliderMoveKeyImage_Keyboard);
	}
}

void UUW_RewindHUD::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);
}
