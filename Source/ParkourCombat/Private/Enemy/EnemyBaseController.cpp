// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/EnemyBaseController.h"

#include "ParkourCombatGameInstance.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

bool AEnemyBaseController::BehaviourConditionCompare(BehaviourCondition RecordedCondition,
	BehaviourCondition CurrentCondition)
{
	const FVector RecordPos = RecordedCondition.GetRecordPosition();
	const bool RecordAvailableAction = RecordedCondition.GetRecordIsAvailableAction();
	const bool RecordExecutingTask = RecordedCondition.GetRecordIsExecutingTask();
	const EnemyAttackCategory RecordAttackCate = RecordedCondition.GetRecordDecidingAttackCategory();
	


	const FVector CurrentPos = CurrentCondition.GetRecordPosition();
	const bool CurrentAvailableAction = CurrentCondition.GetRecordIsAvailableAction();
	const bool CurrentExecutingTask = CurrentCondition.GetRecordIsExecutingTask();
	const EnemyAttackCategory CurrentAttackCate = CurrentCondition.GetRecordDecidingAttackCategory();


	const bool IsAnyPartDifferent = (RecordPos != CurrentPos) || RecordAvailableAction != CurrentAvailableAction || RecordExecutingTask != CurrentExecutingTask || RecordAttackCate != CurrentAttackCate;
	
	return IsAnyPartDifferent;
}

void AEnemyBaseController::ManualResetBehaviour()
{
	if(!BlackboardComponent) return;

	UGameInstance* ReturnGameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if(!ReturnGameInstance) return;

	UParkourCombatGameInstance* CastResult = Cast<UParkourCombatGameInstance>(ReturnGameInstance);
	if(!CastResult) return;
	
	for (UBlackboardData* It = BlackboardComponent->GetBlackboardAsset(); It; It = It->Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
		{
			const UBlackboardKeyType* KeyType = It->Keys[KeyIndex].KeyType;
			if (KeyType)
			{
				const int32 UseIdx = KeyIndex + It->GetFirstKeyID();
				Blackboard->ClearValue(UseIdx);
			}
		}
	}

	EnemyDifficulty StoredDifficulty = CastResult->GetStoredDifficulty();
	BlackboardComponent->SetValueAsBool("IsAvailableAction", true);
	BlackboardComponent->SetValueAsBool("IsStrafing", false);
	BlackboardComponent->SetValueAsBool("BrainLocked", false);
	BlackboardComponent->SetValueAsEnum("CurrentEnemyDifficulty", static_cast<int32>(StoredDifficulty));
	BlackboardComponent->SetValueAsFloat("MaxStrafingTime", 1.0f);

	BehaviorComponent->StopLogic("ManualRestart");
	BehaviorComponent->StartLogic();
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
	BlackboardComponent->SetValueAsBool("IsAvailableAction", true);
}

void AEnemyBaseController::OverwriteRewindDataToBT(EEnemyReplayCommand RewindReplayCommand, bool RewindExecutingTask)
{
	if(!BlackboardComponent) return;

	BlackboardComponent->SetValueAsEnum("EnemyReplayCommand", static_cast<int32>(RewindReplayCommand));
	BlackboardComponent->SetValueAsBool("IsExecutingTask", RewindExecutingTask);
	
	
}

void AEnemyBaseController::OnBeenInterruptedByDamage()
{
	BlackboardComponent->SetValueAsBool("IsAvailableAction", false);
	BlackboardComponent->SetValueAsEnum("EnemyReplayCommand", static_cast<int32>(EEnemyReplayCommand::None));
	BlackboardComponent->SetValueAsBool("IsExecutingTask", false);
}

void AEnemyBaseController::OnConditionChangeInterrupt()
{
	if(!BlackboardComponent || !OwnerEnemyBase) return;
	OwnerEnemyBase->ClearTaskVariables();
}

void AEnemyBaseController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, bUpdatePawn);
	
	//Smooth and change the pawn rotation
	if (bUpdatePawn)
	{
		//Get pawn
		APawn* const MyPawn = GetPawn();
		//Get Pawn current rotation
		const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();
		//Calculate smoothed rotation
		SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(MyPawn->GetActorRotation(), ControlRotation, DeltaTime, SmoothFocusInterpSpeed);
		//Check if we need to change
		if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
		{
			//Change rotation using the Smooth Target Rotation
			MyPawn->FaceRotation(SmoothTargetRotation, DeltaTime);
		}
	}
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
		OwnerEnemyBase->OnEnemyGetInterruptedByStrikeDelegate.BindDynamic(this, &AEnemyBaseController::OnBeenInterruptedByDamage);
		OwnerEnemyBase->OnEnemyBeginBehaviourConditionCheckDelegate.BindDynamic(this, &AEnemyBaseController::StartBehaviourConditionCheck);
		DebugSetDifficulty();
	}

}

void AEnemyBaseController::BehaviourConditionCheckTick()
{
	if(!BlackboardComponent || !OwnerEnemyBase) return;
	
	const FVector CurrentPos = OwnerEnemyBase->GetActorLocation();
	const bool AvailableAction = BlackboardComponent->GetValueAsBool("IsAvailableAction");
	const bool ExecutingTask = BlackboardComponent->GetValueAsBool("IsExecutingTask");
	const EnemyAttackCategory AttackCate = static_cast<EnemyAttackCategory>(BlackboardComponent->GetValueAsEnum("DecidingAttackCategory"));
	
	if(BehaviourConditionCompare(CurrentRecordedBehaviourCondition, BehaviourCondition(CurrentPos, AvailableAction, ExecutingTask, AttackCate)))
	{
		EndBehaviourConditionCheck();
		return;
	}

	StackedBehaviourStuckTime += 0.05;
	if(StackedBehaviourStuckTime >= BehaviourStuckTimeLimit)
	{
		StackedBehaviourStuckTime = 0;
		// Reset Behaviour
		ManualResetBehaviour();
		EndBehaviourConditionCheck();
	}
}

void AEnemyBaseController::StartBehaviourConditionCheck()
{
	UWorld* World = GetWorld();
	if(!World) return;
	
	const FVector CurrentPos = OwnerEnemyBase->GetActorLocation();
	const bool AvailableAction = BlackboardComponent->GetValueAsBool("IsAvailableAction");
	const bool ExecutingTask = BlackboardComponent->GetValueAsBool("IsExecutingTask");
	const EnemyAttackCategory AttackCate = static_cast<EnemyAttackCategory>(BlackboardComponent->GetValueAsEnum("DecidingAttackCategory"));
	CurrentRecordedBehaviourCondition = BehaviourCondition(CurrentPos, AvailableAction, ExecutingTask, AttackCate);
	
	World->GetTimerManager().SetTimer(BehaviourCheckTickTimerHandle,this, &AEnemyBaseController::BehaviourConditionCheckTick, 0.05f, true, -1);
}

void AEnemyBaseController::EndBehaviourConditionCheck()
{
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BehaviourCheckTickTimerHandle);
	}
}

void AEnemyBaseController::OnStoreDifficulty(EnemyDifficulty NewDifficulty)
{
	if(!OwnerEnemyBase || !BlackboardComponent) return;
	
	DebugDifficulty = NewDifficulty;
	OwnerEnemyBase->SetDifficulty(NewDifficulty);
	BlackboardComponent->SetValueAsEnum("CurrentEnemyDifficulty", static_cast<int32>(NewDifficulty));

}
