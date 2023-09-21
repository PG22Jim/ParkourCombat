// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerEnums.h"
#include "UObject/Interface.h"
#include "EnemyActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARKOURCOMBAT_API IEnemyActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEnemyEnterSpecificCombatState(CombatStatus EnteringStatus);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayChargeAttackDamageAnim();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnGameClearWhenEnemyDie();
};
