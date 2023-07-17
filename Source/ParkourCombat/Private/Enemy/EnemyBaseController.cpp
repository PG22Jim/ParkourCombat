// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/EnemyBaseController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyBase.h"

void AEnemyBaseController::TryCastEnemyBaseReference(APawn* OwnerPawn)
{
	if(OwnerEnemyBase) return;

	AEnemyBase* CastResult = Cast<AEnemyBase>(OwnerPawn);
	if(CastResult) OwnerEnemyBase = CastResult;
}


AEnemyBaseController::AEnemyBaseController()
{
	//Initialize Behaviour Tree
	BehaviorComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AEnemyBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(!OwnerEnemyBase) TryCastEnemyBaseReference(InPawn);

	if(!BehaviorTree) return;


	// start the behaviour tree
	if (BehaviorComponent && BehaviorTree)
	{
		if (BehaviorTree->BlackboardAsset != nullptr)
		{
			BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		}
		
		BehaviorComponent->StartTree(*BehaviorTree);
	}
}
