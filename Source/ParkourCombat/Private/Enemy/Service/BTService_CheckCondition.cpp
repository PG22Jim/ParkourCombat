// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Service/BTService_CheckCondition.h"

#include "AIController.h"
#include "Animation/AnimInstanceProxy.h"
#include "BaseCharacter/SwordProjectile.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/KismetSystemLibrary.h"

bool UBTService_CheckCondition::GetOwnerReference(APawn* OwnerPawn)
{
	if(EnemyRef) return true;
	
	AEnemyBase* CastedEnemy = Cast<AEnemyBase>(OwnerPawn);
	if(!CastedEnemy) return false;

	EnemyRef = CastedEnemy;
	return true;
}

UBTService_CheckCondition::UBTService_CheckCondition()
{
	NodeName = "Check If Condition Change";

	BBKey_ConditionHasChanged.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckCondition, BBKey_ConditionHasChanged));
	BBKey_IsExecutingConChangeAction.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckCondition, BBKey_IsExecutingConChangeAction));
	BBKey_ConditionChangeAction.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckCondition, BBKey_ConditionChangeAction), StaticEnum<EnemyActionCommand>());
	BBKey_EnemyDifficulty.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckCondition, BBKey_ConditionChangeAction), StaticEnum<EnemyDifficulty>());
	BBKey_ConditionChangeAttackCategory.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_CheckCondition, BBKey_ConditionChangeAction), StaticEnum<EnemyAttackCategory>());
}

void UBTService_CheckCondition::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	// If enemy is executing condition change action, no need to check it again
	if(BlackBoard->GetValueAsBool(BBKey_IsExecutingConChangeAction.SelectedKeyName)) return;
	const bool bRecordConditionChange = BlackBoard->GetValueAsBool(BBKey_ConditionHasChanged.SelectedKeyName);
	
	const bool bOwnerRegister = GetOwnerReference(AIOwner->GetPawn());
	if(!bOwnerRegister) return;

	const EnemyDifficulty CurrentDifficulty = static_cast<EnemyDifficulty>(BlackBoard->GetValueAsEnum(BBKey_EnemyDifficulty.SelectedKeyName));
	
	const bool bConditionChange = DecideEnemyConditionChangeAction(CurrentDifficulty, BlackBoard);

	if(bConditionChange) BlackBoard->SetValueAsBool(BBKey_ConditionHasChanged.SelectedKeyName, true);
	else if(bRecordConditionChange) BlackBoard->SetValueAsBool(BBKey_ConditionHasChanged.SelectedKeyName, false);
	
}

bool UBTService_CheckCondition::DecideEnemyConditionChangeAction(EnemyDifficulty CurrentDifficulty, UBlackboardComponent* BlackBoard)
{
	const UWorld* World = GetWorld();
	if(!World) return false;
	
	if(CurrentDifficulty == EnemyDifficulty::Easy) return false;

	const CombatStatus EnemyCombatStatus = EnemyRef->GetCurrentCombatStatus();

	if(EnemyCombatStatus != CombatStatus::Idle) return false;


	const bool bEnemyHasPlayerRef = EnemyRef->TryCachePlayerTargetRef();
	if(!bEnemyHasPlayerRef) return false;

	ABaseCharacter* PlayerRef = EnemyRef->GetPlayerTargetRef();

	
	// spheremultitrace to grab all objects between enemy and player but except them.
	TArray<AActor*> ResultActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(EnemyRef);
	IgnoreActors.Add(PlayerRef);

	TArray<FHitResult> Results;
	
	UKismetSystemLibrary::SphereTraceMulti(World, EnemyRef->GetActorLocation(), PlayerRef->GetActorLocation(), 50, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None,Results, true);
	
	if(Results.Num() > 0)
	{
		// for loop the check each trace result actor is sword projectile.
		for(FHitResult EachResult : Results)
		{
			if(AActor* EachResultActor = EachResult.GetActor())
			{
				// check if actor class is projectile before casting
				if(EachResultActor->IsA(ASwordProjectile::StaticClass()))
					if(ASwordProjectile* CastProjectile = Cast<ASwordProjectile>(EachResultActor))
					{
						// if incoming projectile is not enemy's, change condition action to counter
						AActor* ProjectileOwner = CastProjectile->GetProjectileOwner();
						if(ProjectileOwner != EnemyRef)
						{
							BlackBoard->SetValueAsEnum(BBKey_ConditionChangeAction.SelectedKeyName, static_cast<uint8>(EnemyActionCommand::Counter));
							BlackBoard->SetValueAsEnum(BBKey_ConditionChangeAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::None));
							BlackBoard->SetValueAsBool(BBKey_IsExecutingConChangeAction.SelectedKeyName, true);
							return true;
						}
					}
			}
		}
	}

	// if enemy's difficulty is hard player is healing, change action to launch attack
	if(CurrentDifficulty != EnemyDifficulty::Hard) return false;
	CombatStatus PlayerCombatStatus = PlayerRef->GetCurrentCombatStatus();
	if(PlayerCombatStatus == CombatStatus::Healing)
	{
		BlackBoard->SetValueAsEnum(BBKey_ConditionChangeAction.SelectedKeyName, static_cast<uint8>(EnemyActionCommand::Attack));
		BlackBoard->SetValueAsEnum(BBKey_ConditionChangeAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::LaunchAttack));
		BlackBoard->SetValueAsBool(BBKey_IsExecutingConChangeAction.SelectedKeyName, true);
		return true;
	}
	
	return false;
}

bool UBTService_CheckCondition::CheckIfPlayerIsHealing(CombatStatus PlayerCurrentCombatStatus)
{
	return PlayerCurrentCombatStatus == CombatStatus::Healing;
}
