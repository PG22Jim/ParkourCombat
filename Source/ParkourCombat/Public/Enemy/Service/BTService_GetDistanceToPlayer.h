// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_GetDistanceToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTService_GetDistanceToPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
protected:

	AActor* PLayerActorRef;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_DistanceToPlayer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_RegisterPlayerObject;

	UBTService_GetDistanceToPlayer();
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void TryRegisterPlayerObject(UBlackboardComponent* BlackBoard);
	
};
