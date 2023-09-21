// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_BaseActionTask.h"

void UBTTask_BaseActionTask::TryStoreReference(APawn* OwnerPawn)
{
	if(EnemyRef && OwnerPawn)
	{
		if(EnemyRef->GetController() != OwnerPawn->GetController())
		{
			AEnemyBase* CastedEnemy = Cast<AEnemyBase>(OwnerPawn);
			if(CastedEnemy)	EnemyRef = CastedEnemy;
		}
		
		return;
	}
	
	AEnemyBase* CastedEnemy = Cast<AEnemyBase>(OwnerPawn);
	if(!CastedEnemy) return;

	EnemyRef = CastedEnemy;
}

UBTTask_BaseActionTask::UBTTask_BaseActionTask()
{
	BBKey_IsExecutingTask.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_BaseActionTask, BBKey_IsExecutingTask));
}