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

struct BehaviourCondition
{

protected:
	FVector RecordPosition;
	bool RecordIsAvailableAction; 
	bool RecordIsExecutingTask;
	EnemyAttackCategory RecordDecidingAttackCategory;

public:

	BehaviourCondition()
	{
		RecordPosition = FVector(0,0,0);
		RecordIsAvailableAction = false; 
		RecordIsExecutingTask = false;
		RecordDecidingAttackCategory = EnemyAttackCategory::None;
	}
	
	BehaviourCondition(FVector NewPos, bool ActionAvailable, bool ExecutingTask, EnemyAttackCategory DecidedCategory)
	{
		RecordPosition = NewPos;
		RecordIsAvailableAction = ActionAvailable;
		RecordIsExecutingTask = ExecutingTask;
		RecordDecidingAttackCategory = DecidedCategory;
	}

	FVector GetRecordPosition() const {return RecordPosition;}
	bool GetRecordIsAvailableAction() const {return RecordIsAvailableAction;}
	bool GetRecordIsExecutingTask() const {return RecordIsExecutingTask;}
	EnemyAttackCategory GetRecordDecidingAttackCategory() const {return RecordDecidingAttackCategory;}
};



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
	
	FRotator SmoothTargetRotation;

	FTimerHandle BehaviourCheckTickTimerHandle;

	BehaviourCondition CurrentRecordedBehaviourCondition;

	bool BehaviourConditionCompare(BehaviourCondition RecordedCondition, BehaviourCondition CurrentCondition);

	void ManualResetBehaviour();
	
	float StackedBehaviourStuckTime = 0;
	
protected:

	// Base behavior tree, run on start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=AI)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Debug)
	EnemyDifficulty DebugDifficulty = EnemyDifficulty::Easy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BehaviourStuckTimeLimit = 3.0f;
	
	
	UFUNCTION()
	void OnFinishConditionChangeTask();

	UFUNCTION()
	void ResetBBTaskVariables();
	
	UFUNCTION()
	void OverwriteRewindDataToBT(EEnemyReplayCommand RewindReplayCommand, bool RewindExecutingTask);

	UFUNCTION()
	void OnBeenInterruptedByDamage();
	
	AEnemyBaseController();
	
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void BehaviourConditionCheckTick();

public:

	UFUNCTION()
	void StartBehaviourConditionCheck();
	
	
	void EndBehaviourConditionCheck();

	
	void OnStoreDifficulty(EnemyDifficulty NewDifficulty);


	void OnConditionChangeInterrupt();

	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;
	
	
};
