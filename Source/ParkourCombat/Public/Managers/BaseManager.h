// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "GameFramework/Actor.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "BaseManager.generated.h"

UCLASS()
class PARKOURCOMBAT_API ABaseManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseManager();

private:

	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= CharacterReferenceSetting)
	AParkourCombatCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= CharacterReferenceSetting)
	AEnemyBase* EnemyRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= CharacterReferenceSetting)
	TSubclassOf<AEnemyBase> EnemyClass;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void TryGetPlayerChar();

	virtual bool TryGetEnemyChar();

	virtual void TryBoundPlayerFunctionDynamic();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
