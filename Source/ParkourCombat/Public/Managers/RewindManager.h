// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseManager.h"
#include "ObjectPoolManager.h"
#include "ParkourMovementLinkedList.h"
#include "Components/TimelineComponent.h"
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

	void OnUpdateRewindVariables();
	void OnRewindSliderVisibility(bool SliderVisible);

	void RewindToIndex(int32 RequestIndex);

	void TryResetRecordAnimationPosition(bool bIsEnemy);
	bool IsPlayerMoving();
	void SetCalculatedPlayPosition(ParkourStatus CurrentStatus);
	void SetCalculatedPlayPosition_Enemy();


	void ResetTimeStopEffectMPCValue();

	void TryBindPlayerFunction();

	
	bool IsRewinding = false;
	bool IsPlayingRewindEffect = false;

	float RecordAnimationPosition_Player = 0;
	float RecordAnimationPosition_Enemy = 0;
	int32 CurrentRewindIndex = 0;

protected:


	BackTrackData_Stack* CurrenBackTrackData = new BackTrackData_Stack();
	ParkourPositionData* CurrentIndexData;

	UPROPERTY()
	FTimeline StopTimeEffectTimeline;

	FTimerHandle RecordPositionTimerHandle;
	FTimerHandle BackTrackingTimerHandle;
	FTimerHandle StopBackTrackTimerHandle;
	FTimerHandle InputKeyCheckTickTimerHandle;

	bool ObjectPoolManagerValidationCheck();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTimelineComponent* TimelineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UUW_RewindHUD* RewindHUDRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUW_RewindHUD> RewindHUDClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AObjectPoolManager* ObjectPoolManagerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AObjectPoolManager> ObjectPoolManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TimeStopSetting)
	UCurveFloat* TimeStopEffectCurveFloat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TimeStopSetting)
	UMaterialParameterCollection* TimeStopMPC;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=InputSetting)
	// FKey 

	
	UPROPERTY()
	APlayerController* PlayerControllerRef;

	bool TryCachePlayerControllerRef();

	
	
	UFUNCTION()
	void RecordCurrentPositionToList();

	EnemyFrameData GrabCurrentEnemyFrameData();
	

	UFUNCTION()
	void BackTrackTransform();


	UFUNCTION()
	void InputCheckTick();

	UFUNCTION()
	void OnTimeStopEffectUpdate(float Alpha);
	UFUNCTION()
	void OnTimeStopEffectFinish();

	UFUNCTION()
	void OnFreezeToLastFrame();


	
	

	UFUNCTION(BlueprintImplementableEvent)
	void PrintAnimationMontage(UAnimMontage* FrameMontage, UAnimMontage* PlayerCurrentMontage);

	UFUNCTION()
	void OnRewindSliderValueChanged(float Value);

	virtual void TryGetPlayerChar() override;
	
public:


	UFUNCTION()
	void PauseRecording();
	
	void RewindSliderIncrement();
	void RewindSliderDecrement();
	
	bool GetIsRewinding() const {return IsRewinding;}

	bool GetIsPlayingRewindEffect() const {return IsPlayingRewindEffect;}

	UFUNCTION()
	void TryStartRewinding();
	
	UFUNCTION()
	void StartRewinding();
	UFUNCTION()
	void StopRewinding();
	
	
	void TrySetPlayerCharacter(AParkourCombatCharacter* NewPlayerCharacter) { if(PlayerCharacter != NewPlayerCharacter) PlayerCharacter = NewPlayerCharacter; }

	void OnUpdateRewindHUD(ControlType NewControlType) {RewindHUDRef->OnUpdateControlUI(NewControlType);}

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
