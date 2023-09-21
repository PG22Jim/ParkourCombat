// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Task/BTTask_BaseActionTask.h"
#include "BTTask_SetActorSpeed.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_SetActorSpeed : public UBTTask_BaseActionTask
{
	GENERATED_BODY()
protected:
	
	UBTTask_SetActorSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NewMovementSpeed = 500.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void SetEnemyRefSpeed(float Speed);
	
};
