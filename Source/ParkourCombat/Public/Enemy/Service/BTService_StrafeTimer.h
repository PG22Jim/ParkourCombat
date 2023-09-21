// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_StrafeTimer.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTService_StrafeTimer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
protected:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_CurrentRemainStrafeTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_IsStrafing;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_MaxStrafeTime;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ServiceTickTime = 0.05f;
	
	UBTService_StrafeTimer();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
