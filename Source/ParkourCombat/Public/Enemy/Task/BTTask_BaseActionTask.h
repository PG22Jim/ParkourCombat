// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy/EnemyBase.h"
#include "BTTask_BaseActionTask.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_BaseActionTask : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	void TryStoreReference(APawn* OwnerPawn);

	UBTTask_BaseActionTask();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_IsExecutingTask;
	

	AEnemyBase* EnemyRef;
};
