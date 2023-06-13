// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/RewindManager.h"

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
	CurrenBackTrackData->Push(CurrentTrans);
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


	// 
	PlayerCharacter->SetActorTransform(CurrentTopData->TransformData);
	CurrenBackTrackData->Pop();
}


void ARewindManager::StartRewinding()
{
	// Return if manager is rewinding
	if(IsRewinding)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Rewind is in process")));

		return;
	}
	
	const UWorld* World = GetWorld();
	if(!World) return;

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, FString::Printf(TEXT("Rewind Started")));

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

