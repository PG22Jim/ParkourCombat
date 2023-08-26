// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnumList.h"
#include "UObject/Interface.h"
#include "BaseDamageReceiveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBaseDamageReceiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PARKOURCOMBAT_API IBaseDamageReceiveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReceiveDamage(AActor* DamageCauser, float DamageAmount, FVector DamageReceiveLocation, CharacterDamageType ReceivingDamageType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSuccessfulHit(CharacterDamageType ReceivingDamageType);
	
};
