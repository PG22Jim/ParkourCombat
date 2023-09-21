// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/BaseManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseManager::ABaseManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseManager::TryGetPlayerChar()
{
	// if player character is valid, just return
	if(PlayerCharacter) return;

	// Get player character and cast class
	ACharacter* ReturnCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	if(!ReturnCha) return;

	AParkourCombatCharacter* CastedCha = Cast<AParkourCombatCharacter>(ReturnCha);
	if(!CastedCha) return;

	PlayerCharacter = CastedCha;
	
}

bool ABaseManager::TryGetEnemyChar()
{
	// if enemy character is valid, just return true
	if(EnemyRef) return true;

	// Get enemy actor
	AActor* ReturnCha = UGameplayStatics::GetActorOfClass(GetWorld(), EnemyClass);
	if(!ReturnCha) return false;

	AEnemyBase* CastedCha = Cast<AEnemyBase>(ReturnCha);
	if(!CastedCha) return false;

	EnemyRef = CastedCha;
	return true;
}

void ABaseManager::TryBoundPlayerFunctionDynamic()
{
}

// Called when the game starts or when spawned
void ABaseManager::BeginPlay()
{
	Super::BeginPlay();

	TryGetPlayerChar();

	if(!PlayerCharacter) return;

	TryBoundPlayerFunctionDynamic();
}

// Called every frame
void ABaseManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

