// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActionTask.h"
#include "Enemy/EnemyEnum.h"
#include "BTTask_DecideActionToPerform.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_DecideActionToPerform : public UBTTask_BaseActionTask
{
	GENERATED_BODY()

	ACharacter* PlayerCharacterRef;
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_DecidingAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_IsConditionChange;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_ConditionChangeAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToRead")
	FBlackboardKeySelector BBKey_Difficulty;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToRead")
	FBlackboardKeySelector BBKey_ReplayCommand;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "VariableSetting")
	float HealChanceSetting_Easy = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "VariableSetting")
	float HealChanceSetting_Normal = 35.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "VariableSetting")
	float HealChanceSetting_Hard = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "VariableSetting")
	float UnableToLeaveAndHealDistance = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "VariableSetting")
	float RequestHealPercentageToUseHealItem = 50.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "DebugSetting")
	bool IsDebugOn = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "DebugSetting")
	EnemyActionCommand DebugAction = EnemyActionCommand::Counter;
	
	
	UBTTask_DecideActionToPerform();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


	void DecideToExecuteReplayCommand(EEnemyReplayCommand ReplayCommand, UBlackboardComponent* BlackBoard);
	bool DecideToHeal(const FVector OwnPawnPos);
	float InitialChanceByDifficulty(EnemyDifficulty Difficulty);
	
};