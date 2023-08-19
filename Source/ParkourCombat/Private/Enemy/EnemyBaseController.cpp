// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/EnemyBaseController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

void AEnemyBaseController::DebugSetDifficulty()
{
	if(!OwnerEnemyBase || !BlackboardComponent) return;

	OwnerEnemyBase->SetDifficulty(DebugDifficulty);

	BlackboardComponent->SetValueAsEnum("CurrentEnemyDifficulty", static_cast<uint8>(DebugDifficulty));
}

void AEnemyBaseController::TryCastEnemyBaseReference(APawn* OwnerPawn)
{
	if(OwnerEnemyBase) return;

	AEnemyBase* CastResult = Cast<AEnemyBase>(OwnerPawn);
	if(!CastResult) return;

	OwnerEnemyBase = CastResult;

	OwnerEnemyBase->OnFinishConditionChangeActionTask.BindDynamic(this, &AEnemyBaseController::OnFinishConditionChangeTask);
	OwnerEnemyBase->OnRewindEndDelegate.BindDynamic(this, &AEnemyBaseController::OverwriteRewindDataToBT);
	OwnerEnemyBase->OnResetBBVariablesDelegate.BindDynamic(this, &AEnemyBaseController::ResetBBTaskVariables);
}


void AEnemyBaseController::OnFinishConditionChangeTask()
{
	if(!BlackboardComponent) return;
	
	BlackboardComponent->SetValueAsBool("IsExecutingConChangeAction", false);
}

void AEnemyBaseController::ResetBBTaskVariables()
{
	BlackboardComponent->SetValueAsEnum("EnemyReplayCommand", static_cast<int32>(EEnemyReplayCommand::None));
	BlackboardComponent->SetValueAsBool("IsExecutingTask", false);
}

void AEnemyBaseController::OverwriteRewindDataToBT(EEnemyReplayCommand RewindReplayCommand, bool RewindExecutingTask)
{
	if(!BlackboardComponent) return;

	BlackboardComponent->SetValueAsEnum("EnemyReplayCommand", static_cast<int32>(RewindReplayCommand));
	BlackboardComponent->SetValueAsBool("IsExecutingTask", RewindExecutingTask);
	
	
}

void AEnemyBaseController::OnConditionChangeInterrupt()
{
	if(!BlackboardComponent || !OwnerEnemyBase) return;
	OwnerEnemyBase->ClearTaskVariables();
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
		
		OwnerEnemyBase->SetBTCompRef(BehaviorComponent);
		DebugSetDifficulty();
	}

}
