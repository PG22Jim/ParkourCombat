// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Service/BTService_GetDistanceToPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_GetDistanceToPlayer::UBTService_GetDistanceToPlayer()
{
	NodeName = "Get Distance To Player";
	BBKey_DistanceToPlayer.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetDistanceToPlayer, BBKey_DistanceToPlayer));
	BBKey_RegisterPlayerObject.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetDistanceToPlayer, BBKey_RegisterPlayerObject), AActor::StaticClass());
}


void UBTService_GetDistanceToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	// Grab necessary references 
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	const APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!OwningPawn) return;

	if(!PLayerActorRef)
	{
		TryRegisterPlayerObject(BlackBoard);
	
		AActor* PlayerActor = Cast<AActor>(BlackBoard->GetValueAsObject(BBKey_RegisterPlayerObject.SelectedKeyName));
		if(!PlayerActor) return;

		PLayerActorRef = PlayerActor;
	}

	const FVector OwnPawnPos = OwningPawn->GetActorLocation();
	const FVector PlayerPos = PLayerActorRef->GetActorLocation();
	
	const float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(OwnPawnPos, PlayerPos);
	BlackBoard->SetValueAsFloat(BBKey_DistanceToPlayer.SelectedKeyName, DistanceToPlayer);
}

void UBTService_GetDistanceToPlayer::TryRegisterPlayerObject(UBlackboardComponent* BlackBoard)
{
	if(!BlackBoard) return;

	if(BlackBoard->GetValueAsObject(BBKey_RegisterPlayerObject.SelectedKeyName)) return;

	const UWorld* World = GetWorld();
	if(!World) return;
	
	ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(World,0);
	
	BlackBoard->SetValueAsObject(BBKey_RegisterPlayerObject.SelectedKeyName, PlayerCha);
}
