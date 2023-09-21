// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Enemy/EnemyBase.h"
#include "BTService_CheckCondition.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTService_CheckCondition : public UBTService_BlackboardBase
{
	GENERATED_BODY()
protected:

	AEnemyBase* EnemyRef;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_ConditionHasChanged;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_ConditionChangeAction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_ConditionChangeAttackCategory;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_IsExecutingConChangeAction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_IsStrafing;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_EnemyDifficulty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	UClass* ProjectileClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DetectionRange = 500.0f;
	
	
	bool GetOwnerReference(APawn* OwnerPawn);
	
	UBTService_CheckCondition();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool DecideEnemyConditionChangeAction(EnemyDifficulty CurrentDifficulty, UBlackboardComponent* BlackBoard);

	bool CheckIfPlayerIsHealing(CombatStatus PlayerCurrentCombatStatus);
};
