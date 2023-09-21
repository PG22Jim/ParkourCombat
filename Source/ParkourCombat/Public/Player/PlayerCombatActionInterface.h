// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerEnums.h"
#include "UObject/Interface.h"
#include "PlayerCombatActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerCombatActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARKOURCOMBAT_API IPlayerCombatActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFinishNormalAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEnterSpecificState(CombatStatus State);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnEnterSpecificParryState(CombatStatus State);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFinishDodgeSuccessTime();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnPlayerDeath();
};
