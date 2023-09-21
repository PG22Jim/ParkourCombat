// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParkourCombatGameMode.h"
#include "ParkourCombatCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

bool AParkourCombatGameMode::TryGetPlayerRef()
{
	if(PlayerRef) return true;
	
	const UWorld* World = GetWorld();
	if(!World) return false;

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if(!PlayerChar) return false;

	AParkourCombatCharacter* CastedResult = Cast<AParkourCombatCharacter>(PlayerChar);
	if(!CastedResult) return false;

	PlayerRef = CastedResult;
	return true;
}

bool AParkourCombatGameMode::TryGetHUDManager()
{
	if(HUDManagerRef) return true;

	const UWorld* World = GetWorld();
	if(!World) return false;

	AActor* ResultActor = UGameplayStatics::GetActorOfClass(World, AHUDManager::StaticClass());
	if(!ResultActor) return false;

	AHUDManager* CastedResult = Cast<AHUDManager>(ResultActor);
	if(!CastedResult) return false;

	HUDManagerRef = CastedResult;
	return true;
	
}

void AParkourCombatGameMode::OnPauseGame()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(!TryGetHUDManager()) return;

	//if(HUDManagerRef->IsOnlyPauseMenuRemain()){}
	
	HUDManagerRef->DisplayPauseMenu();

	const bool IsGamePaused = UGameplayStatics::IsGamePaused(World);
	if(!IsGamePaused)
	{
		// if game is not paused
		UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(World, true);
	}
	else
	{
		UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = false;
		UGameplayStatics::SetGamePaused(World, false);
		
	}
}

void AParkourCombatGameMode::TryUnPauseGame()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(!TryGetHUDManager()) return;
	HUDManagerRef->DisplayPauseMenu();
	
	UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = false;
	UGameplayStatics::SetGamePaused(World, false);
}


AParkourCombatGameMode::AParkourCombatGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AParkourCombatGameMode::BeginPlay()
{
	Super::BeginPlay();

	TryGetPlayerRef();
	TryGetHUDManager();

	if(PlayerRef)
	{
		PlayerRef->OnPauseGameDelegate.BindDynamic(this, &AParkourCombatGameMode::OnPauseGame);
	}
	if(HUDManagerRef)
	{
		UUW_PauseMenu* PauseMenuRefReturn = HUDManagerRef->GetPauseMenuRef();
		if(!PauseMenuRefReturn) return;

		if(!PauseMenuRefReturn->OnContinueGameDelegate.IsBound())
		{
			PauseMenuRefReturn->OnContinueGameDelegate.BindDynamic(this, &AParkourCombatGameMode::TryUnPauseGame);
		}
	}

}
