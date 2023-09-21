// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

UCLASS()
class PARKOURCOMBAT_API ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialManager();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AEnemyBase* GuardTutorialEnemy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AEnemyBase* DodgeTutorialEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AEnemyBase* TutorialEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AEnemyBase*> StoppingTutorialEnemies;


	void SetPlayerTutorialProperty();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void StopAllTutorialEnemy();

	void ActivateGuardTutorialEnemy() {GuardTutorialEnemy->OnTryUnLockBTBrain();}
	void ActivateDodgeTutorialEnemy() {DodgeTutorialEnemy->OnTryUnLockBTBrain();}
	void ActivateTutorialEnemy() {TutorialEnemy->OnTryUnLockBTBrain();}

	void DeactivateGuardTutorialEnemy() {GuardTutorialEnemy->OnTryLockBTBrain();}
	void DeactivateDodgeTutorialEnemy() {DodgeTutorialEnemy->OnTryLockBTBrain();}
	void DeactivateTutorialEnemy() {TutorialEnemy->OnTryLockBTBrain();}

	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
