// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_ControlScheme.h"

#include "Kismet/GameplayStatics.h"


void UUW_ControlScheme::OnExitControlScheme()
{
	OnExitControlSchemeDelegate.ExecuteIfBound();
}

void UUW_ControlScheme::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsInitialized)
	{
		ExitButton->OnPressed.AddDynamic(this, &UUW_ControlScheme::OnExitControlScheme);
		ExitButton->SynchronizeProperties();

		OnUpdateControlUI(ControlType::KeyBoardAndMouse);

		IsInitialized = true;
	}




}

void UUW_ControlScheme::OnUpdateControlUI(ControlType NewControlType)
{
	Super::OnUpdateControlUI(NewControlType);

	if(NewControlType == ControlType::Gamepad)
	{
		ControlSchemeImage->SetBrush(ControlSchemeBrush_Gamepad);
		CancelKeyImage->SetBrush(CancelKeyImage_Gamepad);
	}
	else
	{
		ControlSchemeImage->SetBrush(ControlSchemeBrush_KeyBoardAndMouse);
		CancelKeyImage->SetBrush(CancelKeyImage_Keyboard);
	}
}

void UUW_ControlScheme::OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey)
{
	Super::OnReceiveMenuInputKey(ReceiveKey);

	if(ReceiveKey == PlayerMenuInputType::BackKey) OnExitControlScheme();
}



