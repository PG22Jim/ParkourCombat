// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ParkourMovementLinkedList.h"
#include "GameFramework/Actor.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "RewindManager.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PARKOURCOMBAT_API ARewindManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewindManager();

private:
	void TryGetPlayerChar();
	void TryBoundPlayerFunctionDynamic();
	
	bool IsRewinding = false;

protected:


	BackTrackData_Stack* CurrenBackTrackData = new BackTrackData_Stack();


	FTimerHandle RecordPositionTimerHandle;
	FTimerHandle BackTrackingTimerHandle;
	FTimerHandle StopBackTrackTimerHandle;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AParkourCombatCharacter* PlayerCharacter;

	UFUNCTION()
	void RecordCurrentPositionToList();

	UFUNCTION()
	void BackTrackTransform();

	UFUNCTION()
	void StopRewinding();

	
	UFUNCTION()
	void StartRewinding();

	
	
public:


	void TrySetPlayerCharacter(AParkourCombatCharacter* NewPlayerCharacter) { if(PlayerCharacter != NewPlayerCharacter) PlayerCharacter = NewPlayerCharacter; }


	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
