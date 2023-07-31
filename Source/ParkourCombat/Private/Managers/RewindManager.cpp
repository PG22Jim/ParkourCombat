// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/RewindManager.h"

#include "Components/PlayerParkourComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARewindManager::ARewindManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ARewindManager::TryGetPlayerChar()
{
	// if player character is valid, just return
	if(PlayerCharacter) return;

	// Get player character and cast class
	ACharacter* ReturnCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	if(!ReturnCha) return;

	AParkourCombatCharacter* CastedCha = Cast<AParkourCombatCharacter>(ReturnCha);
	if(!CastedCha) return;

	PlayerCharacter = CastedCha;

	TryBoundPlayerFunctionDynamic();
}

void ARewindManager::TryBoundPlayerFunctionDynamic()
{
	if(!PlayerCharacter) return;

	const bool IsBound = PlayerCharacter->OnStartRewindDelegate.IsBound();
	if(IsBound) return;
	
	PlayerCharacter->OnStartRewindDelegate.BindDynamic(this, &ARewindManager::StartRewinding);
}

// Called when the game starts or when spawned
void ARewindManager::BeginPlay()
{
	Super::BeginPlay();

	TryGetPlayerChar();

	if(!PlayerCharacter) return;

	PlayerCharacter->OnStartRewindDelegate.BindDynamic(this, &ARewindManager::StartRewinding);

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
					RecordAnimationPosition += 0.015;
					CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition);
					// TODO: Set the pushed data as Variable data
					PreviousCopyData = ParkourPositionData(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition);
					return;
				}
			}
		}
	}
	
	TryResetRecordAnimationPosition();
	CurrenBackTrackData->Push(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition);
	PreviousCopyData = ParkourPositionData(CurrentTrans, CurrentParkour, CurrentCombatStatus, CurrentMontage, RecordAnimationPosition);
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

	
	UAnimMontage* PlayerFrameMontage = CurrentTopData->CurrentPlayerFrameData.GetCurrentMontage();

	// TODO: Instead of using get current montage from player character, use record data will be better?
	UAnimMontage* PlayerCurrentMontage;
	if(&PreviousCopyData)
		PlayerCurrentMontage = PreviousCopyData.CurrentPlayerFrameData.GetCurrentMontage();
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("Cannot Get Record Data")));
		PlayerCurrentMontage = nullptr;
	}
		
	
	//PrintAnimationMontage(PlayerFrameMontage, PlayerCurrentMontage);

	// TODO: Somehow it is playing montage multiple time
	if(PlayerFrameMontage)
	{
		if(!PlayerCurrentMontage)
		{
			const float FrameMontagePosition = CurrentTopData->CurrentPlayerFrameData.GetCurrentAnimationTime();
			PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);
		}
		else
		{
			const FString FrameMontageName = UKismetSystemLibrary::GetDisplayName(PlayerFrameMontage);
			const FString CurrentRecordMontageName = UKismetSystemLibrary::GetDisplayName(PlayerCurrentMontage);

			if(FrameMontageName != CurrentRecordMontageName)
			{
				const float FrameMontagePosition = CurrentTopData->CurrentPlayerFrameData.GetCurrentAnimationTime();
				PlayerCharacter->PlayFrameMontage(PlayerFrameMontage, FrameMontagePosition);
			}
		}
	}
	
	PreviousCopyData = *CurrentTopData;
	CurrenBackTrackData->Pop();
}


void ARewindManager::StartRewinding()
{
	// Return if manager is rewinding
	if(IsRewinding)
	{
		
		return;
	}
	
	const UWorld* World = GetWorld();
	if(!World) return;

	IsRewinding = true;

	FTimerManager& WorldTimerManager = World->GetTimerManager();

	
	WorldTimerManager.PauseTimer(RecordPositionTimerHandle);
	WorldTimerManager.SetTimer(BackTrackingTimerHandle,this, &ARewindManager::BackTrackTransform, 0.015, true, -1);
	WorldTimerManager.SetTimer(StopBackTrackTimerHandle,this, &ARewindManager::StopRewinding, 10.0f, false, -1);
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

