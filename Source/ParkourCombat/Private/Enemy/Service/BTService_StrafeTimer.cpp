// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Service/BTService_StrafeTimer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTService_StrafeTimer::UBTService_StrafeTimer()
{
	NodeName = "Strafe Timer Count Down";
	BBKey_CurrentRemainStrafeTime.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_StrafeTimer, BBKey_CurrentRemainStrafeTime));
	BBKey_IsStrafing.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_StrafeTimer, BBKey_IsStrafing));
	BBKey_MaxStrafeTime.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_StrafeTimer, BBKey_MaxStrafeTime));

}

void UBTService_StrafeTimer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Grab necessary references 
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return;

	//if(!BlackBoard->GetValueAsBool(BBKey_IsStrafing.SelectedKeyName)) return;
	
	float RemainStrafeTime = BlackBoard->GetValueAsFloat(BBKey_CurrentRemainStrafeTime.SelectedKeyName);
	RemainStrafeTime += ServiceTickTime;

	if(RemainStrafeTime >= BlackBoard->GetValueAsFloat(BBKey_MaxStrafeTime.SelectedKeyName))
	{
		BlackBoard->SetValueAsFloat(BBKey_CurrentRemainStrafeTime.SelectedKeyName,0);
		BlackBoard->SetValueAsBool(BBKey_IsStrafing.SelectedKeyName, false);
		return;
	}

	BlackBoard->SetValueAsFloat(BBKey_CurrentRemainStrafeTime.SelectedKeyName,RemainStrafeTime);

}
