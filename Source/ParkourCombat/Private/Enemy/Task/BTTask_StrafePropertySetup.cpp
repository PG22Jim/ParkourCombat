// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_StrafePropertySetup.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_StrafePropertySetup::UBTTask_StrafePropertySetup()
{
	NodeName = "Initialize Strafe Property";

	BBKey_OverwriteMaxStrafeTime.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_StrafePropertySetup, BBKey_OverwriteMaxStrafeTime));
	BBKey_CurrentDifficulty.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_StrafePropertySetup, BBKey_CurrentDifficulty), StaticEnum<EnemyDifficulty>());
	BBKey_IsStrafing.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_StrafePropertySetup, BBKey_IsStrafing));

}

EBTNodeResult::Type UBTTask_StrafePropertySetup::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	const EnemyDifficulty CurrentDifficulty = static_cast<EnemyDifficulty>(BlackBoard->GetValueAsEnum(BBKey_CurrentDifficulty.SelectedKeyName));


	const float ReturnFloat = DecideNewStrafeTotalTime(CurrentDifficulty);

	BlackBoard->SetValueAsFloat(BBKey_OverwriteMaxStrafeTime.SelectedKeyName, ReturnFloat);

	BlackBoard->SetValueAsBool(BBKey_IsStrafing.SelectedKeyName, true);

	
	return EBTNodeResult::Succeeded;
}

float UBTTask_StrafePropertySetup::DecideNewStrafeTotalTime(EnemyDifficulty CurrentDifficulty)
{
	float BaseRemainTime = 0;
	
	if(CurrentDifficulty == EnemyDifficulty::Easy)
		BaseRemainTime = StrafeInterval_Easy;
	
	else if(CurrentDifficulty == EnemyDifficulty::Normal)
		BaseRemainTime = StrafeInterval_Normal;
	
	else BaseRemainTime = StrafeInterval_Hard;

	const float DeviationResult = UKismetMathLibrary::RandomFloatInRange(0 - RandomDeviation, RandomDeviation);

	return BaseRemainTime + DeviationResult;
}
