// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ParkourCombatCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "Managers/HUDManager.h"
#include "ParkourCombatGameMode.generated.h"




UCLASS(minimalapi)
class AParkourCombatGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	UPROPERTY()
	AHUDManager* HUDManagerRef;

	UPROPERTY()
	AParkourCombatCharacter* PlayerRef;
	
	bool TryGetPlayerRef();

	bool TryGetHUDManager();
	
protected:

	UFUNCTION()
	void OnPauseGame();

	UFUNCTION()
	void TryUnPauseGame();
	
public:
	AParkourCombatGameMode();



	virtual void BeginPlay() override;
	
};



