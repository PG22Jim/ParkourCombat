// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy/EnemyEnum.h"
#include "BTTask_StrafePropertySetup.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_StrafePropertySetup : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_OverwriteMaxStrafeTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_IsStrafing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_CurrentDifficulty;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StrafeInterval_Easy = 3.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StrafeInterval_Normal = 2.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StrafeInterval_Hard = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RandomDeviation = 0.5f;
	

	UBTTask_StrafePropertySetup();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	float DecideNewStrafeTotalTime(EnemyDifficulty CurrentDifficulty);
};
