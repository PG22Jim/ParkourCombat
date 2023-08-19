// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter/SwordProjectile.h"
#include "Managers/BaseManager.h"
#include "ObjectPoolManager.generated.h"

/**
 * 
 */





UCLASS()
class PARKOURCOMBAT_API AObjectPoolManager : public ABaseManager
{
	GENERATED_BODY()


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ASwordProjectile*> ProjectilesInPool;

	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ABaseCharacter* EnemyCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RangeAttackSpawnSetting)
	TSubclassOf<ASwordProjectile> RangeAttackClass;


	UFUNCTION(BlueprintCallable)
	void InitializeProjectile(ASwordProjectile* InitializingProjectile);
	
	virtual void TryBoundPlayerFunctionDynamic() override;
	
public:

	UPROPERTY()
	TArray<ASwordProjectile*> ActiveProjectiles;

	bool HasProjectileInPool() const {return ProjectilesInPool.Num() > 0;}
	TArray<FProjectileFrameData> OnRecordProjectileFrameData();

	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSpawnProjectile(AActor* NewSpawnOwner, FRotator SpawnRotation);

	
};
