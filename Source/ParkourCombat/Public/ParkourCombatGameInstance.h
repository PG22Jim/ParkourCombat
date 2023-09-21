// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyEnum.h"
#include "Engine/GameInstance.h"
#include "ParkourCombatGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UParkourCombatGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	EnemyDifficulty StoredDifficulty = EnemyDifficulty::Easy;
	


public:

	EnemyDifficulty GetStoredDifficulty() const {return StoredDifficulty;}
	void SetStoredDifficulty(EnemyDifficulty NewDifficulty) {StoredDifficulty = NewDifficulty;}
	
};
