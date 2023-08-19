// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyBase.h"
#include "EnemyEnum.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyBaseController.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API AEnemyBaseController : public AAIController
{
	GENERATED_BODY()
private:
	UBehaviorTreeComponent* BehaviorComponent;
	UBlackboardComponent* BlackboardComponent;
	AEnemyBase* OwnerEnemyBase;

	void DebugSetDifficulty();

	void TryCastEnemyBaseReference(APawn* OwnerPawn);
	
protected:

	// Base behavior tree, run on start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=AI)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Debug)
	EnemyDifficulty DebugDifficulty = EnemyDifficulty::Easy;
	

	UFUNCTION()
	void OnFinishConditionChangeTask();

	UFUNCTION()
	void ResetBBTaskVariables();
	
	UFUNCTION()
	void OverwriteRewindDataToBT(EEnemyReplayCommand RewindReplayCommand, bool RewindExecutingTask);



	
	AEnemyBaseController();
	
	virtual void OnPossess(APawn* InPawn) override;


public:

	void OnConditionChangeInterrupt();
	
	
	
};
