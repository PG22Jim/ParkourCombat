// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/RewindManager.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/PlayerParkourComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
ARewindManager::ARewindManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TimelineComponent = CreateDefaultSubobject<UTimelineComponent>("Timeline Comp");

}

void ARewindManager::OnUpdateRewindVariables()
{
	const int32 NumInList = CurrenBackTrackData->GetSizeNum();
	RewindHUDRef->RewindSlider->SetMaxValue(NumInList);
	RewindHUDRef->RewindSlider->SetValue(NumInList);
	CurrentRewindIndex = NumInList;
	CurrentIndexData = CurrenBackTrackData->Peek();
}

void ARewindManager::OnRewindSliderVisibility(bool SliderVisible)
{
	if(SliderVisible)
	{
		RewindHUDRef->AddToViewport();
		return;	
	}

	RewindHUDRef->RemoveFromParent();
}

void ARewindManager::RewindToIndex(int32 RequestIndex)
{
	if(!PlayerCharacter)
	{
		TryGetPlayerChar();
		if(!PlayerCharacter) return;
	}
	if(!TryGetEnemyChar()) return;

	if(!CurrentIndexData)
	{
		CurrentIndexData = CurrenBackTrackData->Peek();
		if(!CurrentIndexData) return;
	}
	
	int32 NumNeedToMove = RequestIndex - CurrentRewindIndex;

	while (NumNeedToMove != 0)
	{
		if(NumNeedToMove > 0)
		{
			CurrentIndexData = CurrentIndexData->PreviousFrameData;
			NumNeedToMove--;
			if(!CurrentIndexData)
				CurrentIndexData = CurrenBackTrackData->Peek();
		}
		else
		{
			CurrentIndexData = CurrentIndexData->NextFrameData;
			NumNeedToMove++;
			if(!CurrentIndexData)
				CurrentIndexData = CurrenBackTrackData->GetTailData();
		}
	}

	// Player 
	const ParkourStatus PlayerFrameParkourStatus = CurrentIndexData->CurrentPlayerFrameData.GetCurrentParkourStatus();
	PlayerCharacter->SetCurrentParkourStatus(PlayerFrameParkourStatus);
	
	const CombatStatus PlayerFrameCombatStatus = CurrentIndexData->CurrentPlayerFrameData.GetCurrentCombatStatus();
	PlayerCharacter->SetCurrentCombatStatus(PlayerFrameCombatStatus);
	
	const FTransform PlayerFrameTransform = CurrentIndexData->CurrentPlayerFrameData.GetTransformData();
	PlayerCharacter->SetActorTransform(PlayerFrameTransform);
	
	if(UAnimMontage* PlayerFrameMontage = CurrentIndexData->CurrentPlayerFrameData.GetCurrentMontage())
	{
		const float FrameMontagePosition = CurrentIndexData->CurrentPlayerFrameData.GetCurrentAnimationTime();
		PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);
		PlayerCharacter->SetReplayMontageStartTime(FrameMontagePosition);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("FrameMontagePos %f"), FrameMontagePosition));
	}

	WeaponRewindData PlayerFrameWeaponData = CurrentIndexData->CurrentPlayerFrameData.GetFramePlayerWeaponData();
	PlayerCharacter->TryRewindWeaponData(PlayerFrameWeaponData);

	const float IndexFramePlayerHealth = CurrentIndexData->CurrentPlayerFrameData.GetFramePlayerHealth();
	const float IndexFramePlayerStamina = CurrentIndexData->CurrentPlayerFrameData.GetFramePlayerStamina();
	const float IndexFramePlayerRage = CurrentIndexData->CurrentPlayerFrameData.GetFrameRageGage();
	const float IndexFramePlayerRevenge = CurrentIndexData->CurrentPlayerFrameData.GetFramePlayerRevengeMeter();
	const int32 IndexFramePlayerComboCount = CurrentIndexData->CurrentPlayerFrameData.GetFrameComboCount();
	PlayerCharacter->RewindProperty(IndexFramePlayerHealth, IndexFramePlayerStamina, IndexFramePlayerRage, IndexFramePlayerRevenge, IndexFramePlayerComboCount);
	

	// Enemy
	const EnemyFrameData SelectedEnemyFrameData = CurrentIndexData->CurrentEnemyFrameData; 
	
	const CombatStatus EnemyFrameCombatStatus = SelectedEnemyFrameData.GetCurrentCombatStatus();
	EnemyRef->SetCurrentCombatStatus(EnemyFrameCombatStatus);
	
	const FTransform EnemyFrameTransform = SelectedEnemyFrameData.GetTransformData();
	EnemyRef->SetActorTransform(EnemyFrameTransform);
	
	if(UAnimMontage* EnemyFrameMontage = SelectedEnemyFrameData.GetCurrentMontage())
	{
		const float FrameMontagePosition = SelectedEnemyFrameData.GetCurrentAnimationTime();
		
		EnemyRef->PlayFrameMontage(EnemyFrameMontage, FrameMontagePosition);
		EnemyRef->SetReplayMontageStartTime(FrameMontagePosition);
	}

	const EEnemyReplayCommand CurrentFrameCommand = SelectedEnemyFrameData.GetFrameReplayCommand();
	EnemyRef->UpdateStoredFrameReplayCommand(CurrentFrameCommand);

	EnemyRef->OnOverwriteEnemyNASequence(SelectedEnemyFrameData.GetFrameNAData());

	EnemyRef->SetCurrentHealth(SelectedEnemyFrameData.GetFrameHealth());

	EnemyRef->SetCurrentRageGage(SelectedEnemyFrameData.GetFrameRageValue());

	WeaponRewindData FrameEnemyWeaponData = CurrentIndexData->CurrentEnemyFrameData.GetFrameWeaponData();
	EnemyRef->TryRewindWeaponData(FrameEnemyWeaponData);
	
	//EnemyRef->UpdateStoredFrameTaskStatus(SelectedEnemyFrameData.GetFrameTaskStatus());

	
	// Projectiles
	for(FProjectileFrameData EachProjectileData : CurrentIndexData->ProjectileListDataSet)
	{
		ASwordProjectile* EachProjectile = EachProjectileData.ProjectileRef;

		EachProjectile->SetActorTransform(EachProjectileData.ProjectileFrameTransforms);
		EachProjectile->SetProjectileOwner(EachProjectileData.ProjectileFrameOwner);
		EachProjectile->SetIsActivated(EachProjectileData.ProjectileFrameIsActive);
	}
	
	CurrentRewindIndex = RequestIndex;
}

void ARewindManager::TryResetRecordAnimationPosition(bool bIsEnemy)
{
	if(!bIsEnemy)
	{
		if(RecordAnimationPosition_Player > 0)
			RecordAnimationPosition_Player = 0;

		return;
	}

	if(RecordAnimationPosition_Enemy > 0)
		RecordAnimationPosition_Enemy = 0;
}

bool ARewindManager::IsPlayerMoving()
{
	TryGetPlayerChar();

	if(!PlayerCharacter) return false;

	const FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	const float VectorLength = FMath::Sqrt(FMath::Square(PlayerVelocity.X) + FMath::Square(PlayerVelocity.Y) + FMath::Square(PlayerVelocity.Z));
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("VectorLength %f"), VectorLength));

	if(VectorLength > 0) return true;

	return false;
}

void ARewindManager::SetCalculatedPlayPosition(ParkourStatus CurrentStatus)
{
	if(CurrentStatus == ParkourStatus::Idle)
	{
		if(RecordAnimationPosition_Player > PlayerCharacter->GetIdleAnimLength())
		{
			TryResetRecordAnimationPosition(false);
		}
		else RecordAnimationPosition_Player += 0.015f;
	}
	else
	{
		if(RecordAnimationPosition_Player > PlayerCharacter->GetRunForwardAnimLength())
		{
			TryResetRecordAnimationPosition(false);
		}
		else RecordAnimationPosition_Player += 0.015f;
	}
	
}

void ARewindManager::SetCalculatedPlayPosition_Enemy()
{
	if(!TryGetEnemyChar()) return;
	
	if(!EnemyRef->IsCharacterRunning())
	{
		if(RecordAnimationPosition_Enemy > EnemyRef->GetIdleAnimLength())
		{
			TryResetRecordAnimationPosition(true);
		}
		else RecordAnimationPosition_Enemy += 0.015f;
	}
	else
	{
		if(RecordAnimationPosition_Enemy > EnemyRef->GetRunForwardAnimLength())
		{
			TryResetRecordAnimationPosition(true);
		}
		else RecordAnimationPosition_Enemy += 0.015f;
	}
	
}

void ARewindManager::ResetTimeStopEffectMPCValue()
{
	UWorld* World = GetWorld();
	if(!World) return;
	
	UKismetMaterialLibrary::SetScalarParameterValue(World, TimeStopMPC, "TimeStopEffectValue", 0);
	UKismetMaterialLibrary::SetScalarParameterValue(World, TimeStopMPC, "BackGroundEffectValue", 0);
}

void ARewindManager::TryBindPlayerFunction()
{
	PlayerCharacter->OnPlayerPauseRecordOnDeathDelegate.BindDynamic(this, &ARewindManager::PauseRecording);
}

bool ARewindManager::ObjectPoolManagerValidationCheck()
{
	if(ObjectPoolManagerRef) return true;
	
	
	const UWorld* World = GetWorld();
	if(!World) return false;

	AActor* ReturnActor = UGameplayStatics::GetActorOfClass(World, ObjectPoolManagerClass);
	if(!ReturnActor) return false;

	AObjectPoolManager* CastResult = Cast<AObjectPoolManager>(ReturnActor);
	if(!CastResult) return false;

	ObjectPoolManagerRef = CastResult;
	return true;
}

// Called when the game starts or when spawned
void ARewindManager::BeginPlay()
{
	Super::BeginPlay();

	TryGetPlayerChar();

	if(!PlayerCharacter) return;

	//TODO: Maybe remove
	//TryBoundPlayerFunctionDynamic();
	TryBindPlayerFunction();

	if(RewindHUDClass)
	{
		APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(GetWorld(),0);
		RewindHUDRef = CreateWidget<UUW_RewindHUD>(PlayerControl, RewindHUDClass);
		check(RewindHUDRef);
		RewindHUDRef->RewindSlider->OnValueChanged.AddDynamic(this, &ARewindManager::OnRewindSliderValueChanged);
	}


	
	const UWorld* World = GetWorld();
	if(!World) return;

	FTimerManager& WorldTimerManager = World->GetTimerManager();

	WorldTimerManager.SetTimer(RecordPositionTimerHandle,this, &ARewindManager::RecordCurrentPositionToList, 0.015, true, -1);


	
	FOnTimelineFloat TimeStopEffectProgressUpdate;
	TimeStopEffectProgressUpdate.BindDynamic(this, &ARewindManager::ARewindManager::OnTimeStopEffectUpdate);
	StopTimeEffectTimeline.AddInterpFloat(TimeStopEffectCurveFloat,TimeStopEffectProgressUpdate);

	FOnTimelineEvent FlyProgressFinish;
	FlyProgressFinish.BindDynamic(this, &ARewindManager::OnTimeStopEffectFinish);
	StopTimeEffectTimeline.SetTimelineFinishedFunc(FlyProgressFinish);
}

bool ARewindManager::TryCachePlayerControllerRef()
{
	if(PlayerControllerRef) return true;

	const UWorld* World = GetWorld();
	if(!World) return false;

	APlayerController* ReturnController = UGameplayStatics::GetPlayerController(World,0);
	if(!ReturnController) return false;

	PlayerControllerRef = ReturnController;
	
	return true;
}

void ARewindManager::RecordCurrentPositionToList()
{
	if(!PlayerCharacter)
	{
		TryGetPlayerChar();
		if(!PlayerCharacter) return;
	}

	if(!TryGetEnemyChar()) return;
	EnemyFrameData CurrentEnemyFrameData = GrabCurrentEnemyFrameData();

	if(!ObjectPoolManagerRef->HasProjectileInPool()) return;
	
	TArray<FProjectileFrameData> CurrentProjectileFrameDataSet = ObjectPoolManagerRef->OnRecordProjectileFrameData();
	
	const FTransform CurrentTrans = PlayerCharacter->GetActorTransform();
	const ParkourStatus CurrentParkour = PlayerCharacter->GetCurrentParkourStatus();
	const CombatStatus CurrentCombatStatus = PlayerCharacter->GetCurrentCombatStatus();
	
	UAnimMontage* CurrentMontage = PlayerCharacter->GetCurrentAnimMontage();

	const float CurrentPlayerHP = PlayerCharacter->GetCurrentHealth();
	const float CurrentPlayerStamina = PlayerCharacter->GetCurrentStamina();
	const float CurrentPlayerRevenge = PlayerCharacter->GetRevengeMeter();
	const float CurrentPlayerRageGauge = PlayerCharacter->GetRageGage();

	int32 CurrentPlayerComboCount = PlayerCharacter->GetComboCount();

	ABaseWeapon* PlayerWeapon = PlayerCharacter->GetWeaponOnHand();
	WeaponRewindData CurrentPlayerWeaponRewindData = (PlayerWeapon) ? PlayerCharacter->GetWeaponOnHand()->GetCurrentWeaponRewindData() : WeaponRewindData();
	
	// if player is currently playing montage
	if(CurrentMontage)
	{
		// if rewind manager has data at previous custom tick
		if(const ParkourPositionData* NewestData = CurrenBackTrackData->Peek())
		{
			// if player's was playing montage at previous custom tick 
			if(const UAnimMontage* TopDataMontage = NewestData->CurrentPlayerFrameData.GetCurrentMontage())
			{
				// if player's previous tick montage is same to current playing montage
				if(CurrentMontage == TopDataMontage)
				{
					RecordAnimationPosition_Player += 0.015;
					//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Action Animation"));	
					CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player, CurrentPlayerHP,CurrentPlayerStamina,CurrentPlayerRevenge,CurrentPlayerRageGauge,CurrentProjectileFrameDataSet, CurrentEnemyFrameData, CurrentPlayerWeaponRewindData, CurrentPlayerComboCount);
					return;
				}

				TryResetRecordAnimationPosition(false);
				CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player,CurrentPlayerHP,CurrentPlayerStamina,CurrentPlayerRevenge,CurrentPlayerRageGauge, CurrentProjectileFrameDataSet, CurrentEnemyFrameData, CurrentPlayerWeaponRewindData, CurrentPlayerComboCount);
				return;
			}
		}
	}

	
	

	
	// if player is not playing montage, then player might be moving or idle
	const ParkourStatus PlayerCurrentParkourStatus = PlayerCharacter->GetCurrentParkourStatus();
	
	// if rewind manager has data at previous custom tick
	if(const ParkourPositionData* NewestData = CurrenBackTrackData->Peek())
	{
		const bool bIsDifferentCombatStatus = CurrentCombatStatus != NewestData->CurrentPlayerFrameData.GetCurrentCombatStatus();
		
		
		// if player's last record time is running animation montage
		if(bIsDifferentCombatStatus)
		{
			// if player is doing different action, reset animation position, change recording animation depend on current parkour status
			TryResetRecordAnimationPosition(false);
			if(PlayerCurrentParkourStatus == ParkourStatus::Idle)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Record Idle Montage When player is finish action!"));	
				CurrentMontage = PlayerCharacter->GetIdleMontage();
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Record Running Montage When player is finish action!"));	

				CurrentMontage = PlayerCharacter->GetRunForwardMontage();
			}
			
			CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player,CurrentPlayerHP,CurrentPlayerStamina,CurrentPlayerRevenge,CurrentPlayerRageGauge, CurrentProjectileFrameDataSet, CurrentEnemyFrameData, CurrentPlayerWeaponRewindData, CurrentPlayerComboCount);
			
			return;
		}
		
		// if player is doing same thing
		const ParkourStatus NewestTickParkourStatus = NewestData->CurrentPlayerFrameData.GetCurrentParkourStatus();
		if(PlayerCurrentParkourStatus == NewestTickParkourStatus)
		{
			// animation position increase by tick time 
			SetCalculatedPlayPosition(PlayerCurrentParkourStatus);
			CurrentMontage = NewestData->CurrentPlayerFrameData.GetCurrentMontage();
		}
		else
		{
			// if player is doing different action, reset animation position, change recording animation depend on current parkour status
			TryResetRecordAnimationPosition(false);
			if(PlayerCurrentParkourStatus == ParkourStatus::Idle)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Record Idle Montage When player start to idle!"));	
				CurrentMontage = PlayerCharacter->GetIdleMontage();
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Record Idle Montage When player start to run!"));	

				CurrentMontage = PlayerCharacter->GetRunForwardMontage();
			}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Locomotion Animation"));	
		CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player,CurrentPlayerHP,CurrentPlayerStamina,CurrentPlayerRevenge,CurrentPlayerRageGauge, CurrentProjectileFrameDataSet, CurrentEnemyFrameData, CurrentPlayerWeaponRewindData, CurrentPlayerComboCount);
		
		return;
	}

	// if rewind manager doesn't have any data in list
	TryResetRecordAnimationPosition(false);
	CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, PlayerCharacter->GetIdleMontage(), RecordAnimationPosition_Player, CurrentPlayerHP,CurrentPlayerStamina,CurrentPlayerRevenge,CurrentPlayerRageGauge,CurrentProjectileFrameDataSet, CurrentEnemyFrameData, CurrentPlayerWeaponRewindData, CurrentPlayerComboCount);
}

EnemyFrameData ARewindManager::GrabCurrentEnemyFrameData()
{
	EnemyFrameData ReturnFrameData = EnemyFrameData();
	if(!TryGetEnemyChar())
	{
		return ReturnFrameData;
	}

	const EEnemyReplayCommand CurrentReplayCommand = EnemyRef->GetStoredFrameReplayCommand();
	const EnemyNormalAttackSequence CurrentNASequence = EnemyRef->GetCurrentNASequence();
	const EEnemyTaskStatus CurrentFrameTaskStatus = EnemyRef->GetStoredFrameTaskStatus();

	ABaseWeapon* EnemyWeaponRef = EnemyRef->GetWeaponOnHand();
	WeaponRewindData CurrentEnemyWeaponData = (EnemyWeaponRef) ? EnemyWeaponRef->GetCurrentWeaponRewindData() : WeaponRewindData();

	
	const FTransform CurrentTrans = EnemyRef->GetActorTransform();
	
	const CombatStatus CurrentCombatStatus = EnemyRef->GetCurrentCombatStatus();
	
	UAnimMontage* CurrentMontage = EnemyRef->GetCurrentMontage();

	const bool CurrentlyExecutingTask = EnemyRef->GetExecutingTask();

	const float CurrentEnemyHealth = EnemyRef->GetCurrentHealth();

	const float CurrentEnemyRageGauge = EnemyRef->GetRageGage();

	
	// if enemy is currently playing montage
	if(CurrentMontage)
	{
		// if rewind manager has data at previous custom tick
		if(const ParkourPositionData* NewestData = CurrenBackTrackData->Peek())
		{
			// if Enemy's was playing montage at previous custom tick 
			if(const UAnimMontage* TopDataMontage = NewestData->CurrentEnemyFrameData.GetCurrentMontage())
			{
				// if Enemy's previous tick montage is same to current playing montage
				if(CurrentMontage == TopDataMontage)
				{
					RecordAnimationPosition_Enemy += 0.015;
					
					ReturnFrameData = EnemyFrameData(CurrentTrans, CurrentCombatStatus, CurrentMontage,false, RecordAnimationPosition_Enemy, CurrentReplayCommand, CurrentNASequence, CurrentFrameTaskStatus, CurrentlyExecutingTask,CurrentEnemyHealth, CurrentEnemyRageGauge, CurrentEnemyWeaponData);
					return ReturnFrameData;
				}

				TryResetRecordAnimationPosition(true);
				ReturnFrameData = EnemyFrameData(CurrentTrans, CurrentCombatStatus, CurrentMontage,false, RecordAnimationPosition_Enemy, CurrentReplayCommand, CurrentNASequence, CurrentFrameTaskStatus, CurrentlyExecutingTask,CurrentEnemyHealth, CurrentEnemyRageGauge, CurrentEnemyWeaponData);
				return ReturnFrameData;
			}
		}
	}

	// if Enemy is not playing montage, then Enemy might be moving or idle
	const bool bCurrentEnemyRunning = EnemyRef->IsCharacterRunning();
	
	// if rewind manager has data at previous custom tick
	if(const ParkourPositionData* NewestData = CurrenBackTrackData->Peek())
	{
		// if Enemy is doing same thing
		const bool NewestTickIsRunning = NewestData->CurrentEnemyFrameData.GetIsRunning();
		if(bCurrentEnemyRunning == NewestTickIsRunning)
		{
			// animation position increase by tick time 
			SetCalculatedPlayPosition_Enemy();
			CurrentMontage = NewestData->CurrentEnemyFrameData.GetCurrentMontage();
		}
		else
		{
			// if Enemy is doing different action, reset animation position, change recording animation depend on current parkour status
			TryResetRecordAnimationPosition(true);
			if(!bCurrentEnemyRunning)
			{
				CurrentMontage = EnemyRef->GetIdleMontage();
			}
			else
			{
				CurrentMontage = EnemyRef->GetRunForwardMontage();
			}
		}

		ReturnFrameData = EnemyFrameData(CurrentTrans, CurrentCombatStatus, CurrentMontage, bCurrentEnemyRunning, RecordAnimationPosition_Enemy, CurrentReplayCommand, CurrentNASequence, CurrentFrameTaskStatus, CurrentlyExecutingTask, CurrentEnemyHealth, CurrentEnemyRageGauge, CurrentEnemyWeaponData);
		return ReturnFrameData;
	}

	
	return ReturnFrameData;
}

void ARewindManager::BackTrackTransform()
{
	if(!PlayerCharacter)
	{
		TryGetPlayerChar();
		if(!PlayerCharacter) return;
	}
	
	const ParkourPositionData* CurrentTopData = CurrenBackTrackData->Peek();
	if(!CurrentTopData)
	{
		StopRewinding();
		return;
	}

	const ParkourStatus PlayerFrameParkourStatus = CurrentTopData->CurrentPlayerFrameData.GetCurrentParkourStatus();
	PlayerCharacter->SetCurrentParkourStatus(PlayerFrameParkourStatus);
	
	const CombatStatus PlayerFrameCombatStatus = CurrentTopData->CurrentPlayerFrameData.GetCurrentCombatStatus();
	PlayerCharacter->SetCurrentCombatStatus(PlayerFrameCombatStatus);
	
	
	const FTransform PlayerFrameTransform = CurrentTopData->CurrentPlayerFrameData.GetTransformData();
	PlayerCharacter->SetActorTransform(PlayerFrameTransform);
	
	if(UAnimMontage* PlayerFrameMontage = CurrentTopData->CurrentPlayerFrameData.GetCurrentMontage())
	{
		const float FrameMontagePosition = CurrentTopData->CurrentPlayerFrameData.GetCurrentAnimationTime();
		PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);		
	}
	
	//PreviousCopyData = *CurrentTopData;
	CurrenBackTrackData->Pop();
}


void ARewindManager::TryStartRewinding()
{
	// Return if manager is rewinding
	if(IsRewinding)
	{
		StopRewinding();
		return;
	}

	StartRewinding();
}

void ARewindManager::StartRewinding()
{
	const UWorld* World = GetWorld();
	if(!World) return;
	
	FTimerManager& WorldTimerManager = World->GetTimerManager();

	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(World, 0);
	
	// Freeze all recorded members
	if(UGameplayStatics::IsGamePaused(World))
		UGameplayStatics::SetGamePaused(World, false);

	OnFreezeToLastFrame();
	//UGameplayStatics::SetGamePaused(World, true);

	// Stop velocity
	EnemyRef->GetMovementComponent()->Velocity = FVector(0,0,0);
	PlayerCharacter->GetMovementComponent()->Velocity = FVector(0,0,0);

	
	
	PlayerCharacter->FocusEnemy(EnemyRef);
	
	EnemyRef->TryStopBehaviorTree();
	EnemyRef->SetIsInRewind(true);
	PlayerCharacter->SetIsInRewind(true);
	IsRewinding = true;
	OnUpdateRewindVariables();
	WorldTimerManager.PauseTimer(RecordPositionTimerHandle);

	IsPlayingRewindEffect = true;
	StopTimeEffectTimeline.PlayFromStart();
}

void ARewindManager::OnFreezeToLastFrame()
{
	const ParkourPositionData* LatestFrameData = CurrenBackTrackData->Peek();
	if(!LatestFrameData) return;

	// Player
	if(!PlayerCharacter)
	{
		TryGetPlayerChar();
		if(!PlayerCharacter) return;
	}

	// Disable player's input
	PlayerCharacter->SetAbleToMove(false);

	// Disable player stamina regen
	PlayerCharacter->PauseStaminaRegenTimers();
	

	const ParkourStatus PlayerFrameParkourStatus = LatestFrameData->CurrentPlayerFrameData.GetCurrentParkourStatus();
	PlayerCharacter->SetCurrentParkourStatus(PlayerFrameParkourStatus);
		
	const CombatStatus PlayerFrameCombatStatus = LatestFrameData->CurrentPlayerFrameData.GetCurrentCombatStatus();
	PlayerCharacter->SetCurrentCombatStatus(PlayerFrameCombatStatus);
		
		
	const FTransform PlayerFrameTransform = LatestFrameData->CurrentPlayerFrameData.GetTransformData();
	PlayerCharacter->SetActorTransform(PlayerFrameTransform);
		
	if(UAnimMontage* PlayerFrameMontage = LatestFrameData->CurrentPlayerFrameData.GetCurrentMontage())
	{
		const float FrameMontagePosition = LatestFrameData->CurrentPlayerFrameData.GetCurrentAnimationTime();
		PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);
		PlayerCharacter->SetReplayMontageStartTime(FrameMontagePosition);
	}
	
	WeaponRewindData PlayerFrameWeaponData = LatestFrameData->CurrentPlayerFrameData.GetFramePlayerWeaponData();
	PlayerCharacter->TryRewindWeaponData(PlayerFrameWeaponData);

	const float IndexFramePlayerHealth = LatestFrameData->CurrentPlayerFrameData.GetFramePlayerHealth();
	const float IndexFramePlayerStamina = LatestFrameData->CurrentPlayerFrameData.GetFramePlayerStamina();
	const float IndexFramePlayerRage = LatestFrameData->CurrentPlayerFrameData.GetFrameRageGage();
	const float IndexFramePlayerRevenge = LatestFrameData->CurrentPlayerFrameData.GetFramePlayerRevengeMeter();
	const int32 IndexFramePlayerComboCount = LatestFrameData->CurrentPlayerFrameData.GetFrameComboCount();
	PlayerCharacter->RewindProperty(IndexFramePlayerHealth, IndexFramePlayerStamina, IndexFramePlayerRage, IndexFramePlayerRevenge, IndexFramePlayerComboCount);
	


	// TODO: Enemy
	const EnemyFrameData SelectedEnemyFrameData = LatestFrameData->CurrentEnemyFrameData; 
	
	const CombatStatus EnemyFrameCombatStatus = SelectedEnemyFrameData.GetCurrentCombatStatus();
	EnemyRef->SetCurrentCombatStatus(EnemyFrameCombatStatus);
	
	const FTransform EnemyFrameTransform = SelectedEnemyFrameData.GetTransformData();
	EnemyRef->SetActorTransform(EnemyFrameTransform);
	
	if(UAnimMontage* EnemyFrameMontage = SelectedEnemyFrameData.GetCurrentMontage())
	{
		const float FrameMontagePosition = SelectedEnemyFrameData.GetCurrentAnimationTime();
		EnemyRef->PlayFrameMontage(EnemyFrameMontage, FrameMontagePosition);
		EnemyRef->SetReplayMontageStartTime(FrameMontagePosition);
	}

	const EEnemyReplayCommand CurrentFrameCommand = SelectedEnemyFrameData.GetFrameReplayCommand();
	EnemyRef->UpdateStoredFrameReplayCommand(CurrentFrameCommand);

	EnemyRef->OnOverwriteEnemyNASequence(SelectedEnemyFrameData.GetFrameNAData());

	EnemyRef->SetCurrentRageGage(SelectedEnemyFrameData.GetFrameRageValue());

	WeaponRewindData FrameEnemyWeaponData = LatestFrameData->CurrentEnemyFrameData.GetFrameWeaponData();
	EnemyRef->TryRewindWeaponData(FrameEnemyWeaponData);
	
	
	// Projectiles
	for(FProjectileFrameData EachProjectileData : LatestFrameData->ProjectileListDataSet)
	{
		ASwordProjectile* EachProjectile = EachProjectileData.ProjectileRef;

		EachProjectile->OnFreezeProjectile();

		EachProjectile->SetActorTransform(EachProjectileData.ProjectileFrameTransforms);
		EachProjectile->SetProjectileOwner(EachProjectileData.ProjectileFrameOwner);
		EachProjectile->SetIsActivated(EachProjectileData.ProjectileFrameIsActive);
	}
	
}

void ARewindManager::PauseRecording()
{
	const UWorld* World = GetWorld();
	if(!World) return;
	
	FTimerManager& WorldTimerManager = World->GetTimerManager();
	WorldTimerManager.PauseTimer(RecordPositionTimerHandle);
}

void ARewindManager::OnRewindSliderValueChanged(float Value)
{
	const UWorld* World = GetWorld();
	if(!World) return;

	const int32 CurrentRewindData = CurrenBackTrackData->GetSizeNum();
	const int32 ChangedValueIndex = UKismetMathLibrary::ClampInt64(Value, 1,CurrentRewindData);
	if(ChangedValueIndex - CurrentRewindIndex != 0)
	{
		if(UGameplayStatics::IsGamePaused(World)) UGameplayStatics::SetGamePaused(World,false);
		RewindToIndex(ChangedValueIndex);
	}
	

}

void ARewindManager::TryGetPlayerChar()
{
	Super::TryGetPlayerChar();

	if(PlayerCharacter)
	{
		
	}

}

void ARewindManager::RewindSliderIncrement()
{
	OnRewindSliderValueChanged(CurrentRewindIndex + 1);
	RewindHUDRef->RewindSlider->SetValue(CurrentRewindIndex + 1);
}

void ARewindManager::RewindSliderDecrement()
{
	OnRewindSliderValueChanged(CurrentRewindIndex - 1);
	RewindHUDRef->RewindSlider->SetValue(CurrentRewindIndex + 1);
}

void ARewindManager::StopRewinding()
{
	const UWorld* World = GetWorld();
	if(!World) return;
	
	FTimerManager& WorldTimerManager = World->GetTimerManager();

	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(World, 0);
	
	EnemyRef->SetIsInRewind(false);
	PlayerCharacter->SetIsInRewind(false);
	PlayerCharacter->SetAbleToMove(true);
	PlayerCharacter->ResumeStaminaRegenTimers();
	OnRewindSliderVisibility(false);
	WorldTimerManager.UnPauseTimer(RecordPositionTimerHandle);
	WorldTimerManager.ClearTimer(InputKeyCheckTickTimerHandle);
	PlayerControl->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerControl);
	IsRewinding = false;
	const bool bExecuteRewindDelegate = EnemyRef->OnRewindEndDelegate.ExecuteIfBound(EnemyRef->GetStoredFrameReplayCommand(), EnemyRef->GetExecutingTask());
	//if(bExecuteRewindDelegate) EnemyRef->TryResumeBehaviorTree();
	if(!EnemyRef->GetExecutingTask()) EnemyRef->TryResumeBehaviorTree();
	UGameplayStatics::SetGamePaused(World, false);
	PlayerCharacter->TryContinueRewindAnimation();
	PlayerCharacter->UnFocusEnemy();
	EnemyRef->TryContinueRewindAnimation();
	// Clear Recorded Frame Data
	CurrenBackTrackData->ClearListToIndex(CurrentRewindIndex);
	CurrentIndexData = nullptr;
	ResetTimeStopEffectMPCValue();
	EnemyRef->OnBeginBehaviourConditionCheck();

	if(!ObjectPoolManagerValidationCheck()) return;
	ObjectPoolManagerRef->UnFreezeAllProjectiles();
}

void ARewindManager::InputCheckTick()
{
	if(!TryCachePlayerControllerRef()) return;

	const bool bIsLeftKeyPressed = PlayerControllerRef->IsInputKeyDown(EKeys::A) || PlayerControllerRef->IsInputKeyDown(EKeys::Gamepad_DPad_Left);
	if(bIsLeftKeyPressed)
	{
		RewindSliderDecrement();
	}

	
	const bool bIsRightKeyPressed = PlayerControllerRef->IsInputKeyDown(EKeys::D) || PlayerControllerRef->IsInputKeyDown(EKeys::Gamepad_DPad_Right);
	if(bIsRightKeyPressed)
	{
		RewindSliderIncrement();
	}
}

void ARewindManager::OnTimeStopEffectUpdate(float Alpha)
{
	UWorld* World = GetWorld();
	if(!World) return;
	
	UKismetMaterialLibrary::SetScalarParameterValue(World, TimeStopMPC, "TimeStopEffectValue", Alpha);

	const float CurrentBGEffectValue = UKismetMaterialLibrary::GetScalarParameterValue(World, TimeStopMPC, "BackGroundEffectValue");
	if(Alpha >= 1.6 && CurrentBGEffectValue == 0)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(World, TimeStopMPC, "BackGroundEffectValue", 1);
	}
	
}

void ARewindManager::OnTimeStopEffectFinish()
{
	const UWorld* World = GetWorld();
	if(!World) return;
	FTimerManager& WorldTimerManager = World->GetTimerManager();

	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(World, 0);
	if(!PlayerControl) return;
	
	OnRewindSliderVisibility(true);
	PlayerControl->SetShowMouseCursor(true);
	WorldTimerManager.SetTimer(InputKeyCheckTickTimerHandle,this, &ARewindManager::InputCheckTick, 0.02, true, -1);
	IsPlayingRewindEffect = false;
}

// Called every frame
void ARewindManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StopTimeEffectTimeline.TickTimeline(DeltaTime);
}

