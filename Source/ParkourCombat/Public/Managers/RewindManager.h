// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseManager.h"
#include "ObjectPoolManager.h"
#include "ParkourMovementLinkedList.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "Widget/UW_RewindHUD.h"
#include "RewindManager.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PARKOURCOMBAT_API ARewindManager : public ABaseManager
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewindManager();

private:
	virtual void TryBoundPlayerFunctionDynamic() override;

	void OnUpdateRewindVariables();
	void OnRewindSliderVisibility(bool SliderVisible);

	void RewindToIndex(int32 RequestIndex);

	void TryResetRecordAnimationPosition(bool bIsEnemy);
	bool IsPlayerMoving();
	void SetCalculatedPlayPosition(ParkourStatus CurrentStatus);
	void SetCalculatedPlayPosition_Enemy();
	
	bool IsRewinding = false;

	float RecordAnimationPosition_Player = 0;
	float RecordAnimationPosition_Enemy = 0;
	int32 CurrentRewindIndex = 0;

protected:


	BackTrackData_Stack* CurrenBackTrackData = new BackTrackData_Stack();
	ParkourPositionData* CurrentIndexData;

	
	

	FTimerHandle RecordPositionTimerHandle;
	FTimerHandle BackTrackingTimerHandle;
	FTimerHandle StopBackTrackTimerHandle;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUW_RewindHUD* RewindHUDRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUW_RewindHUD> RewindHUDClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AObjectPoolManager* ObjectPoolManagerRef;
	

	
	UFUNCTION()
	void RecordCurrentPositionToList();

	EnemyFrameData GrabCurrentEnemyFrameData();
	

	UFUNCTION()
	void BackTrackTransform();

	UFUNCTION()
	void StopRewinding();

	
	UFUNCTION()
	void StartRewinding();

	UFUNCTION()
	void OnFreezeToLastFrame();
	

	UFUNCTION(BlueprintImplementableEvent)
	void PrintAnimationMontage(UAnimMontage* FrameMontage, UAnimMontage* PlayerCurrentMontage);

	UFUNCTION()
	void OnRewindSliderValueChanged(float Value);
	
public:


	void TrySetPlayerCharacter(AParkourCombatCharacter* NewPlayerCharacter) { if(PlayerCharacter != NewPlayerCharacter) PlayerCharacter = NewPlayerCharacter; }


	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
