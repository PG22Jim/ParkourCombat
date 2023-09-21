// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_SetActorSpeed.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetActorSpeed::UBTTask_SetActorSpeed()
{
	NodeName = "Set Enemy Speed";
}

EBTNodeResult::Type UBTTask_SetActorSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!EnemyRef)
	{
		AAIController* AIOwnerController = OwnerComp.GetAIOwner();
		if(!AIOwnerController) return EBTNodeResult::Failed;

		APawn* ReturnPawn = AIOwnerController->GetPawn();
		if(!ReturnPawn) return EBTNodeResult::Failed;

		TryStoreReference(ReturnPawn);

		if(!EnemyRef) return EBTNodeResult::Failed;
	}

	SetEnemyRefSpeed(NewMovementSpeed);
	
	return EBTNodeResult::Succeeded;
}

void UBTTask_SetActorSpeed::SetEnemyRefSpeed(float Speed)
{
	if(UCharacterMovementComponent* EnemyRefMovementComp = EnemyRef->GetCharacterMovement())
		EnemyRefMovementComp->MaxWalkSpeed = Speed;
	
}
