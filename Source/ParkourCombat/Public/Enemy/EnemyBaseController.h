// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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
	class AEnemyBase* OwnerEnemyBase;


	void TryCastEnemyBaseReference(APawn* OwnerPawn);
	
protected:

	// Base behavior tree, run on start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=AI)
	UBehaviorTree* BehaviorTree;


	AEnemyBaseController();
	
	virtual void OnPossess(APawn* InPawn) override;


public:

	
	
};
