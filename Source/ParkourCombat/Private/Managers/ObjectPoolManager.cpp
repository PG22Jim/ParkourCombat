// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/ObjectPoolManager.h"

#include "Kismet/GameplayStatics.h"

void AObjectPoolManager::InitializeProjectile(ASwordProjectile* InitializingProjectile)
{
	const FVector InitializePos = InitializingProjectile->GetActorLocation();
	InitializingProjectile->SetPoolPosition(InitializePos);
	InitializingProjectile->DeactivateProjectile();
	ProjectilesInPool.Add(InitializingProjectile);
}



void AObjectPoolManager::TryBoundPlayerFunctionDynamic()
{
	Super::TryBoundPlayerFunctionDynamic();

	TryGetPlayerChar();
	if(!PlayerCharacter) return;
	
	if(!PlayerCharacter->OnSpawnProjectileDelegate.IsBound())
	{
		PlayerCharacter->OnSpawnProjectileDelegate.BindDynamic(this, &AObjectPoolManager::OnSpawnProjectile);
	}

	if(!EnemyCharacterRef) return;

	if(!EnemyCharacterRef->OnSpawnProjectileDelegate.IsBound())
	{
		EnemyCharacterRef->OnSpawnProjectileDelegate.BindDynamic(this, &AObjectPoolManager::OnSpawnProjectile);
	}
}

TArray<FProjectileFrameData> AObjectPoolManager::OnRecordProjectileFrameData()
{
	TArray<FProjectileFrameData> ReturnDataSet;

	for(ASwordProjectile* EachProjectile : ProjectilesInPool)
	{
		FProjectileFrameData EachProjectileFrameData = EachProjectile->ExtractProjectileFrameData();
		ReturnDataSet.Add(EachProjectileFrameData);
	}

	return ReturnDataSet;
}

void AObjectPoolManager::BeginPlay()
{
	Super::BeginPlay();
}

void AObjectPoolManager::OnSpawnProjectile(AActor* NewSpawnOwner, FRotator SpawnRotation)
{
	for(ASwordProjectile* EachProjectile : ProjectilesInPool)
	{
		if(!EachProjectile->GetIsActivated())
		{
			const FVector SpawningPos = NewSpawnOwner->GetActorLocation() + (NewSpawnOwner->GetActorForwardVector() * 100.0f);
			const FRotator Rot = UKismetMathLibrary::FindLookAtRotation(NewSpawnOwner->GetActorLocation(), SpawningPos);
			EachProjectile->SetProjectileOwner(NewSpawnOwner);
			EachProjectile->ActivateProjectile(SpawningPos, Rot);
			return;
		}
	}
}

void AObjectPoolManager::UnFreezeAllProjectiles()
{
	for (ASwordProjectile* EachProjectileInPool : ProjectilesInPool)
	{
		if(EachProjectileInPool) EachProjectileInPool->UnFreezeProjectile();
	}
}
