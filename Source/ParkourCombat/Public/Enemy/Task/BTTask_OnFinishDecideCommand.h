// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Task/BTTask_BaseActionTask.h"
#include "BTTask_OnFinishDecideCommand.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_OnFinishDecideCommand : public UBTTask_BaseActionTask
{
	GENERATED_BODY()
protected:

	UBTTask_OnFinishDecideCommand();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ValueToRead)
	FBlackboardKeySelector BBKey_DecidingAttackCategory;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ValueToRead)
	FBlackboardKeySelector BBKey_DecidingActionCommand;

	void StoreDecidedCommandToReplay(EnemyActionCommand DecidedCommand, EnemyAttackCategory DecidedAttack);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
