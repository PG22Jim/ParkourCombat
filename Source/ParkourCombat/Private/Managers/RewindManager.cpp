// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/RewindManager.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/PlayerParkourComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARewindManager::ARewindManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARewindManager::TryBoundPlayerFunctionDynamic()
{
	Super::TryBoundPlayerFunctionDynamic();

	if(!PlayerCharacter) return;

	const bool IsBound = PlayerCharacter->OnStartRewindDelegate.IsBound();
	if(IsBound) return;
	
	PlayerCharacter->OnStartRewindDelegate.BindDynamic(this, &ARewindManager::StartRewinding);
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
			if(!CurrentIndexData) CurrentIndexData = CurrenBackTrackData->Peek();
		}
		else
		{
			CurrentIndexData = CurrentIndexData->NextFrameData;
			NumNeedToMove++;
			if(!CurrentIndexData) CurrentIndexData = CurrenBackTrackData->GetTailData();
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
	}


	// TODO: Enemy
	const EnemyFrameData SelectedEnemyFrameData = CurrentIndexData->CurrentEnemyFrameData; 
	
	const CombatStatus EnemyFrameCombatStatus = SelectedEnemyFrameData.GetCurrentCombatStatus();
	EnemyRef->SetCurrentCombatStatus(EnemyFrameCombatStatus);
	
	const FTransform EnemyFrameTransform = SelectedEnemyFrameData.GetTransformData();
	EnemyRef->SetActorTransform(EnemyFrameTransform);
	
	if(UAnimMontage* EnemyFrameMontage = SelectedEnemyFrameData.GetCurrentMontage())
	{
		/*
		 
		FString MontageName = EnemyFrameMontage->GetName();
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Yellow, FString::Printf(TEXT("MontageName: %s | PlaySecond: %f"), *MontageName, FrameMontagePosition));
		 */
		const float FrameMontagePosition = SelectedEnemyFrameData.GetCurrentAnimationTime();
		
		EnemyRef->PlayFrameMontage(EnemyFrameMontage, FrameMontagePosition);
		EnemyRef->SetReplayMontageStartTime(FrameMontagePosition);
	}

	const EEnemyReplayCommand CurrentFrameCommand = SelectedEnemyFrameData.GetFrameReplayCommand();
	EnemyRef->UpdateStoredFrameReplayCommand(CurrentFrameCommand);

	EnemyRef->OnOverwriteEnemyNASequence(SelectedEnemyFrameData.GetFrameNAData());

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

// Called when the game starts or when spawned
void ARewindManager::BeginPlay()
{
	Super::BeginPlay();

	TryGetPlayerChar();

	if(!PlayerCharacter) return;

	TryBoundPlayerFunctionDynamic();

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

	// {
	// 	UAnimMontage* AnimMon = CurrentEnemyFrameData.GetCurrentMontage();
	// 	float AnimTime = CurrentEnemyFrameData.GetCurrentAnimationTime();
	//
	// 	if(AnimMon)
	// 	{
	// 		FString AnimMonName = AnimMon->GetName();
	// 		if(GEngine)
	// 			GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Blue, FString::Printf(TEXT
	// 				("[MontageName: %s]  [AnimTime: %f]"), *AnimMonName, AnimTime));
	// 		
	// 	}
	// 	else
	// 	{
	// 		GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Blue, TEXT("No Recorded Montage"));	
	//
	// 	}
	// 	
	// }

	if(!ObjectPoolManagerRef->HasProjectileInPool()) return;
	
	TArray<FProjectileFrameData> CurrentProjectileFrameDataSet = ObjectPoolManagerRef->OnRecordProjectileFrameData();
	
	const FTransform CurrentTrans = PlayerCharacter->GetActorTransform();
	const ParkourStatus CurrentParkour = PlayerCharacter->GetCurrentParkourStatus();
	const CombatStatus CurrentCombatStatus = PlayerCharacter->GetCurrentCombatStatus();
	
	UAnimMontage* CurrentMontage = PlayerCharacter->GetCurrentAnimMontage();

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
					CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player, CurrentProjectileFrameDataSet, CurrentEnemyFrameData);
					return;
				}

				TryResetRecordAnimationPosition(false);
				CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player, CurrentProjectileFrameDataSet, CurrentEnemyFrameData);
				return;
			}
		}
	}

	// if player is not playing montage, then player might be moving or idle

	const ParkourStatus PlayerCurrentParkourStatus = PlayerCharacter->GetCurrentParkourStatus();
	
	// if rewind manager has data at previous custom tick
	if(const ParkourPositionData* NewestData = CurrenBackTrackData->Peek())
	{
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
				CurrentMontage = PlayerCharacter->GetIdleMontage();
			}
			else
			{
				CurrentMontage = PlayerCharacter->GetRunForwardMontage();
			}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Locomotion Animation"));	
		CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition_Player, CurrentProjectileFrameDataSet, CurrentEnemyFrameData);
		
		return;
	}

	// if rewind manager doesn't have any data in list
	TryResetRecordAnimationPosition(false);
	CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, PlayerCharacter->GetIdleMontage(), RecordAnimationPosition_Player, CurrentProjectileFrameDataSet, CurrentEnemyFrameData);
}

EnemyFrameData ARewindManager::GrabCurrentEnemyFrameData()
{
	EnemyFrameData ReturnFrameData = EnemyFrameData();
	if(!TryGetEnemyChar()) return ReturnFrameData;

	const EEnemyReplayCommand CurrentReplayCommand = EnemyRef->GetStoredFrameReplayCommand();
	const EnemyNormalAttackSequence CurrentNASequence = EnemyRef->GetCurrentNASequence();
	const EEnemyTaskStatus CurrentFrameTaskStatus = EnemyRef->GetStoredFrameTaskStatus();

	
	const FTransform CurrentTrans = EnemyRef->GetActorTransform();
	const CombatStatus CurrentCombatStatus = EnemyRef->GetCurrentCombatStatus();
	
	UAnimMontage* CurrentMontage = EnemyRef->GetCurrentMontage();

	const bool CurrentlyExecutingTask = EnemyRef->GetExecutingTask();


	
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
					
					ReturnFrameData = EnemyFrameData(CurrentTrans, CurrentCombatStatus, CurrentMontage,false, RecordAnimationPosition_Enemy, CurrentReplayCommand, CurrentNASequence, CurrentFrameTaskStatus, CurrentlyExecutingTask);
					return ReturnFrameData;
				}

				TryResetRecordAnimationPosition(true);
				ReturnFrameData = EnemyFrameData(CurrentTrans, CurrentCombatStatus, CurrentMontage,false, RecordAnimationPosition_Enemy, CurrentReplayCommand, CurrentNASequence, CurrentFrameTaskStatus, CurrentlyExecutingTask);
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

		ReturnFrameData = EnemyFrameData(CurrentTrans, CurrentCombatStatus, CurrentMontage, bCurrentEnemyRunning, RecordAnimationPosition_Enemy, CurrentReplayCommand, CurrentNASequence, CurrentFrameTaskStatus, CurrentlyExecutingTask);
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
	
	// TODO: Somehow it is playing montage multiple time
	if(UAnimMontage* PlayerFrameMontage = CurrentTopData->CurrentPlayerFrameData.GetCurrentMontage())
	{
		const float FrameMontagePosition = CurrentTopData->CurrentPlayerFrameData.GetCurrentAnimationTime();
		PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);		
		// if(!PlayerCurrentMontage)
		// {
		// 	const float FrameMontagePosition = CurrentTopData->CurrentPlayerFrameData.GetCurrentAnimationTime();
		// 	PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);
		// }
		// else
		// {
		// 	const FString FrameMontageName = UKismetSystemLibrary::GetDisplayName(PlayerFrameMontage);
		// 	const FString CurrentRecordMontageName = UKismetSystemLibrary::GetDisplayName(PlayerCurrentMontage);
		//
		// 	if(FrameMontageName != CurrentRecordMontageName)
		// 	{
		// 		const float FrameMontagePosition = CurrentTopData->CurrentPlayerFrameData.GetCurrentAnimationTime();
		// 		PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);
		// 	}
		// }
	}
	
	//PreviousCopyData = *CurrentTopData;
	CurrenBackTrackData->Pop();
}


void ARewindManager::StartRewinding()
{
	const UWorld* World = GetWorld();
	if(!World) return;
	FTimerManager& WorldTimerManager = World->GetTimerManager();

	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(World, 0);


	
	// Return if manager is rewinding
	if(IsRewinding)
	{
		EnemyRef->SetIsInRewind(false);
		PlayerCharacter->SetIsInRewind(false);
		PlayerCharacter->SetAbleToMove(true);
		OnRewindSliderVisibility(false);
		WorldTimerManager.UnPauseTimer(RecordPositionTimerHandle);
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
		CurrenBackTrackData->ClearList();
		CurrentIndexData = nullptr;



		
		return;
	}


	

	// Freeze all recorded members
	OnFreezeToLastFrame();
	UGameplayStatics::SetGamePaused(World, true);

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
	OnRewindSliderVisibility(true);
	PlayerControl->SetShowMouseCursor(true);
	
	

	
	// WorldTimerManager.SetTimer(BackTrackingTimerHandle,this, &ARewindManager::BackTrackTransform, 0.015, true, -1);
	// WorldTimerManager.SetTimer(StopBackTrackTimerHandle,this, &ARewindManager::StopRewinding, 10.0f, false, -1);
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

void ARewindManager::OnRewindSliderValueChanged(float Value)
{
	const UWorld* World = GetWorld();
	if(!World) return;
	
	const int32 ChangedValueIndex = Value;
	if(ChangedValueIndex - CurrentRewindIndex != 0)
	{
		if(UGameplayStatics::IsGamePaused(World)) UGameplayStatics::SetGamePaused(World,false);
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, FString::Printf(TEXT("Value Changed From: %i to %i"), ChangedValueIndex, CurrentRewindIndex));
		//CurrentRewindIndex = ChangedValueIndex;
		RewindToIndex(ChangedValueIndex);
	}
	

}

void ARewindManager::StopRewinding()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	FTimerManager& WorldTimerManager = World->GetTimerManager();

	WorldTimerManager.UnPauseTimer(RecordPositionTimerHandle);
	WorldTimerManager.ClearTimer(BackTrackingTimerHandle);
	WorldTimerManager.ClearTimer(StopBackTrackTimerHandle);

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Rewind Finished")));

	IsRewinding = false;

}
// Called every frame
void ARewindManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

