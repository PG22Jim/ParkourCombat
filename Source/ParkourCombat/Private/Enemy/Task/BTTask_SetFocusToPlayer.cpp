// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_SetFocusToPlayer.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_SetFocusToPlayer::UBTTask_SetFocusToPlayer()
{
	NodeName = "Set Focus To Player";
}

EBTNodeResult::Type UBTTask_SetFocusToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwnerController = OwnerComp.GetAIOwner();
	if(!AIOwnerController) return EBTNodeResult::Failed;

	APawn* ReturnPawn = AIOwnerController->GetPawn();
	if(!ReturnPawn) return EBTNodeResult::Failed;
	
	const UWorld* World = GetWorld();
	if(!World) return EBTNodeResult::Failed;
	
	ACharacter* ReturnCharacter = UGameplayStatics::GetPlayerCharacter(World,0);
	if(!ReturnCharacter) return EBTNodeResult::Failed;
	
	AIOwnerController->SetFocus(ReturnCharacter, EAIFocusPriority::Gameplay);
	
	return EBTNodeResult::Succeeded;
}
