// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/TutorialManager.h"

#include "Kismet/GameplayStatics.h"
#include "ParkourCombat/ParkourCombatCharacter.h"

// Sets default values
ATutorialManager::ATutorialManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATutorialManager::SetPlayerTutorialProperty()
{
	 if(ACharacter* ReturnCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	 {
		 if(AParkourCombatCharacter* CastedResult = Cast<AParkourCombatCharacter>(ReturnCharacter))
		 {
		 	CastedResult->SetCurrentHealth(50);
		 	
		 	
		 }
	 }

	
	
}

// Called when the game starts or when spawned
void ATutorialManager::BeginPlay()
{
	Super::BeginPlay();

	StopAllTutorialEnemy();

	SetPlayerTutorialProperty();
}

void ATutorialManager::StopAllTutorialEnemy()
{
	for (AEnemyBase* EachStoppingTutorialEnemy : StoppingTutorialEnemies)
	{
		EachStoppingTutorialEnemy->OnTryLockBTBrain();
	}
	
	DodgeTutorialEnemy->OnTryLockBTBrain();
	TutorialEnemy->OnTryLockBTBrain();
	GuardTutorialEnemy->OnTryLockBTBrain();
}

// Called every frame
void ATutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

