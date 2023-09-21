// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Managers/HUDManager.h"

#include "AIController.h"
#include "ParkourCombatGameInstance.h"
#include "Enemy/EnemyBaseController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHUDManager::AHUDManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AHUDManager::ControlSchemeRefValidationCheck()
{
	if(ControlSchemeRef) return true;

	TryCreateControlScheme();

	if(ControlSchemeRef)
		return true;
	

	return false;
}

bool AHUDManager::GameOverMenuRefValidationCheck()
{
	if(GameOverMenuRef) return true;

	TryCreateGameOverMenu();

	if(GameOverMenuRef)
		return true;
	
	
	return false;
}

bool AHUDManager::TutorialWidgetValidationCheck()
{
	if(TutorialWidgetRef) return true;

	TryCreateTutorialWidget();

	if(TutorialWidgetRef)
		return true;
	
	
	return false;
}

bool AHUDManager::PauseMenuRefValidationCheck()
{
	if(PauseMenuRef) return true;

	TryCreatePauseMenu();

	if(PauseMenuRef)
		return true;
	
	
	return false;
}

bool AHUDManager::PlayerHUDRefValidationCheck()
{
	if(PlayerHUDRef) return true;

	TryCreatePlayerHUD();

	if(PlayerHUDRef)
		return true;
	

	return false;
}

bool AHUDManager::RewindManagerRefValidationCheck()
{
	if(RewindManagerRef) return true;

	const UWorld* World = GetWorld();
	if(!World) return false;

	AActor* ReturnActor = UGameplayStatics::GetActorOfClass(World, RewindManagerClass);
	if(!ReturnActor) return false;

	ARewindManager* CastResult = Cast<ARewindManager>(ReturnActor);
	if(!CastResult) return false;

	RewindManagerRef = CastResult;
	
	return true;
}

bool AHUDManager::MainMenuRefValidationCheck()
{
	if(MainMenuRef) return true;

	TryCreateMainMenu();

	if(MainMenuRef)
		return true;
	
	
	return false;
}

bool AHUDManager::GameClearMenuRefValidationCheck()
{
	if(GameClearMenuRef) return true;

	TryCreateGameClearHUD();

	if(GameClearMenuRef)
		return true;
	
	
	return false;
	
}

bool AHUDManager::SelectDifficultyMenuRefValidationCheck()
{
	if(SelectDifficultyMenuRef) return true;

	TryCreateSelectDifficultyMenu();

	if(SelectDifficultyMenuRef)
		return true;
	
	
	return false;
}

bool AHUDManager::TryCachePlayerRef()
{
	if(PlayerRef) return true;
	
	const UWorld* World = GetWorld();
	if(!World) return false;
	
	if(ACharacter* ReturnCharacter = UGameplayStatics::GetPlayerCharacter(World,0))
	{
		if(AParkourCombatCharacter* CastResult = Cast<AParkourCombatCharacter>(ReturnCharacter))
		{
			PlayerRef = CastResult;
			return true;
		}
	}

	return false;
}

bool AHUDManager::TryCacheEnemyRef()
{
	if(EnemyBossRef) return true;
	
	TArray<AActor*> ReturnActors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, ReturnActors);

	if(ReturnActors.Num() > 0)
	{
		for (AActor* EachReturnActor : ReturnActors)
		{
			if(EachReturnActor->ActorHasTag("TargetEnemy"))
			{
				AEnemyBase* CastResult =  Cast<AEnemyBase>(EachReturnActor);
				if(CastResult)
				{
					EnemyBossRef = CastResult;
					return true;
				}
			}
		}
		
		
	}

	return false;
}

bool AHUDManager::WidgetValidationCheck()
{
	if(CurrentGameStageType == eStageType::MainMenu)
	{
		if(!ControlSchemeRefValidationCheck()) return false;
		if(!SelectDifficultyMenuRefValidationCheck()) return false;
		if(!MainMenuRefValidationCheck()) return false;
		
		return true;
	}
	
	if(!TryCachePlayerRef()) return false;
	if(!GameClearMenuRefValidationCheck()) return false;
	
	if(CurrentGameStageType == eStageType::Tutorial)
	{
		if(!TutorialWidgetValidationCheck()) return false;

		if(!PauseMenuRefValidationCheck()) return false;
		
		if(!PlayerHUDRefValidationCheck()) return false;

		if(!RewindManagerRefValidationCheck()) return false;

		if(!ControlSchemeRefValidationCheck()) return false;
		
		if(!GameOverMenuRefValidationCheck()) return false;

		return true;
	}


	if(!ControlSchemeRefValidationCheck()) return false;

	if(!GameOverMenuRefValidationCheck()) return false;

	if(!TutorialWidgetValidationCheck()) return false;

	if(!PauseMenuRefValidationCheck()) return false;
		
	if(!PlayerHUDRefValidationCheck()) return false;

	if(!RewindManagerRefValidationCheck()) return false;
	
	return true;
}

void AHUDManager::WidgetInitialization()
{
	if(!ControlSchemeRefValidationCheck()) return;


	ControlSchemeRef->OnExitControlSchemeDelegate.BindDynamic(this, &AHUDManager::OnDisplayControlScheme);
	
	
	if(CurrentGameStageType == eStageType::MainMenu)
	{
		if(!MainMenuRefValidationCheck()) return;


		MainMenuRef->OnShowControlSchemeDelegate.BindDynamic(this, &AHUDManager::OnDisplayControlScheme);
		MainMenuRef->OnOpenSelectDifficultyDelegate.BindDynamic(this, &AHUDManager::OnOpenSelectDifficultyMenu);
		const UWorld* World = GetWorld();
		if(!World) return;

		UGameplayStatics::SetGamePaused(World, true);
		UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = true;
		MainMenuRef->AddToViewport();
		MainMenuRef->SetIsWidgetOnScreen(true);
		SetCurrentSelectedWidgetRef(MainMenuRef);
		SelectDifficultyMenuRef->OnCancelDifficultySelectionDelegate.BindDynamic(this, &AHUDManager::OnCloseSelectDifficultyMenu);
		return;
	}
	
	if(!TryCachePlayerRef()) return;
	if(!TryCacheEnemyRef()) return;
	
	TryBindPlayerRewindDelegate();
	PauseMenuRef->OnShowControlSchemeDelegate.BindDynamic(this, &AHUDManager::OnDisplayControlScheme);
	PauseMenuRef->CallTutorialDelegate.BindUFunction(this, "DisplayTutorialWidget");
	PauseMenuRef->OnPlayerHUDVisibilityDelegate.BindDynamic(this, &AHUDManager::OnPlayerHUDVisibility);
	TutorialWidgetRef->OnCheckPauseMenuIsCloseDelegate.BindDynamic(this, &AHUDManager::IsPauseMenuOff);
	TutorialWidgetRef->FOnPlayerHUDShowOrHideDelegate.BindDynamic(this, &AHUDManager::OnPlayerHUDVisibility);
	TutorialWidgetRef->OnExitTutorialDelegate.BindDynamic(this, &AHUDManager::OnExitTutorialPage);
	
	GameOverMenuRef->OnExecuteRewindButtonDelegate.BindDynamic(this, &AHUDManager::RewindAbilityActivate);
	GameOverMenuRef->OnPlayerHUDVisibilityDelegate.BindDynamic(this, &AHUDManager::OnPlayerHUDVisibility);

	EnemyBossRef->OnEnemyDeathDelegate.BindDynamic(this, &AHUDManager::OnPauseRewindRecording);
	EnemyBossRef->OnGameClearDelegate.BindDynamic(this, &AHUDManager::OnStageClear);
	
	PlayerHUDRef->AddToViewport();

	SetCurrentSelectedWidgetRef(PlayerHUDRef);
	
	
	if(CurrentGameStageType == eStageType::Tutorial)
	{
		OnTutorialStageBeginPlay();
	}
}

void AHUDManager::TryUpdateWidgetControlUI()
{
	ControlSchemeRef->OnUpdateControlUI(CurrentControlType);
	
	if(CurrentGameStageType == eStageType::Game)
	{
		GameOverMenuRef->OnUpdateControlUI(CurrentControlType);
		PauseMenuRef->OnUpdateControlUI(CurrentControlType);
		PlayerHUDRef->OnUpdateControlUI(CurrentControlType);
		TutorialWidgetRef->OnUpdateControlUI(CurrentControlType);
		RewindManagerRef->OnUpdateRewindHUD(CurrentControlType);
		GameClearMenuRef->OnUpdateControlUI(CurrentControlType);
		
		return;
	}
	
	if(CurrentGameStageType == eStageType::Tutorial)
	{
		PauseMenuRef->OnUpdateControlUI(CurrentControlType);
		PlayerHUDRef->OnUpdateControlUI(CurrentControlType);
		TutorialWidgetRef->OnUpdateControlUI(CurrentControlType);
		GameClearMenuRef->OnUpdateControlUI(CurrentControlType);
		RewindManagerRef->OnUpdateRewindHUD(CurrentControlType);

		return;
	}

	MainMenuRef->OnUpdateControlUI(CurrentControlType);


	
}

void AHUDManager::TryBindPlayerRewindDelegate()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	if(!TryCachePlayerRef()) return;

	PlayerRef->OnStartRewindDelegate.BindDynamic(this, &AHUDManager::RewindAbilityActivate);
	PlayerRef->PlayerDeathDelegate.BindDynamic(this, &AHUDManager::OnGameOver);

}

void AHUDManager::OnGameOver()
{
	if(!GameOverMenuRefValidationCheck()) return;

	const UWorld* World = GetWorld();
	if(!World) return;

	OnPlayerHUDVisibility(false);
	UGameplayStatics::SetGamePaused(World, true);
	UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = true;
	GameOverMenuRef->AddToViewport();
	SetCurrentSelectedWidgetRef(GameOverMenuRef);
}

void AHUDManager::OnStageClear()
{
	if(!GameClearMenuRefValidationCheck()) return;

	const UWorld* World = GetWorld();
	if(!World) return;

	OnPlayerHUDVisibility(false);
	UGameplayStatics::SetGamePaused(World, true);
	UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = true;
	GameClearMenuRef->AddToViewport();
	SetCurrentSelectedWidgetRef(GameClearMenuRef);
}

void AHUDManager::OnOpenSelectDifficultyMenu()
{
	if(!SelectDifficultyMenuRefValidationCheck()) return;
	if(!MainMenuRefValidationCheck()) return;

	MainMenuRef->RemoveFromParent();
	SelectDifficultyMenuRef->AddToViewport();
	SetCurrentSelectedWidgetRef(SelectDifficultyMenuRef);
}

void AHUDManager::OnCloseSelectDifficultyMenu()
{
	if(!SelectDifficultyMenuRefValidationCheck()) return;
	if(!MainMenuRefValidationCheck()) return;
	
	SelectDifficultyMenuRef->RemoveFromParent();
	MainMenuRef->AddToViewport();
	SetCurrentSelectedWidgetRef(MainMenuRef);
	
}

void AHUDManager::RewindAbilityActivate()
{
	if(IsGameFinished) return;
	
	if(!RewindManagerRefValidationCheck()) return;
	if(!TryCachePlayerRef()) return;

	
	// early return if time rewind effect is running
	if(RewindManagerRef->GetIsPlayingRewindEffect()) return;

	// if currently rewinding, finish rewind and return
	if(RewindManagerRef->GetIsRewinding())
	{
		RewindManagerRef->StopRewinding();
		return;
	}

	// if player is able to rewind
	if(PlayerRef->IsAbleRewind())
	{
		// consume rewind ability time and check if player still has rewind ability
		const bool bPlayerStillHasRewindAvailableTime = PlayerRef->OnRewindAbilityUseConfirm();
		if(!bPlayerStillHasRewindAvailableTime)
		{
			// if player used his last rewind ability, change game over menu's rewind button
			if(GameOverMenuRefValidationCheck()) GameOverMenuRef->OnChangeRewindButtonToRetryButton();
		}

		// no matter player still has or does not have more rewind time, start rewinding
		RewindManagerRef->StartRewinding();
	}
	
	
}


void AHUDManager::UnLockTutorialPage(int32 UnlockPageNumber)
{
	if(!TutorialWidgetValidationCheck()) return;

	const UWorld* World = GetWorld();
	if(!World) return;

	
	UGameplayStatics::SetGamePaused(World, true);
	UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = true;
	TutorialWidgetRef->AddToViewport();
	TutorialWidgetRef->SetIsWidgetOnScreen(true);
	TutorialWidgetRef->UnlockedPageNumIncrement(UnlockPageNumber);
	TutorialWidgetRef->FlipToNewestPage();
	SetCurrentSelectedWidgetRef(TutorialWidgetRef);
	OnPlayerHUDVisibility(false);
}

void AHUDManager::DisplayTutorialWidget()
{
	if(!TutorialWidgetValidationCheck()) return;

	if(TutorialWidgetRef->IsInViewport())
	{
		TutorialWidgetRef->RemoveFromParent();
		TutorialWidgetRef->SetIsWidgetOnScreen(false);
		if(IsPauseMenuOff())
			SetCurrentSelectedWidgetRef(PlayerHUDRef);
		else
		{
			SetCurrentSelectedWidgetRef(PauseMenuRef);
			PauseMenuRef->SetInputDisplayGridVisibility(true);
		}
	}
	else
	{
		UGameplayStatics::GetPlayerController(GetWorld(),0)->bShowMouseCursor = true;
		TutorialWidgetRef->AddToViewport();
		TutorialWidgetRef->SetIsWidgetOnScreen(true);
		OnPlayerHUDVisibility(false);
		SetCurrentSelectedWidgetRef(TutorialWidgetRef);
		if(!IsPauseMenuOff()) PauseMenuRef->SetInputDisplayGridVisibility(false);

	}
}

void AHUDManager::OnExitTutorialPage()
{
	if(IsPauseMenuOff())
	{
		SetCurrentSelectedWidgetRef(PlayerHUDRef);
		PauseMenuRef->SetInputDisplayGridVisibility(true);
	}
	else
		SetCurrentSelectedWidgetRef(PauseMenuRef);
}

void AHUDManager::DisplayPauseMenu()
{
	if(!PauseMenuRefValidationCheck()) return;

	if(PauseMenuRef->IsInViewport())
	{
		PauseMenuRef->RemoveFromParent();
		PauseMenuRef->SetIsWidgetOnScreen(false);
		OnPlayerHUDVisibility(true);
		SetCurrentSelectedWidgetRef(PlayerHUDRef);
	}
	else
	{
		PauseMenuRef->AddToViewport();
		PauseMenuRef->SetIsWidgetOnScreen(true);
		OnPlayerHUDVisibility(false);
		SetCurrentSelectedWidgetRef(PauseMenuRef);
	}
}



void AHUDManager::StageInitialization()
{
	if(CurrentGameStageType != eStageType::Game) return;
	if(!TryCacheEnemyRef()) return;

	AController* ReturnEnemyRefControl = EnemyBossRef->GetController();
	if(!ReturnEnemyRefControl) return;

	AEnemyBaseController* ControllerCastResult = Cast<AEnemyBaseController>(ReturnEnemyRefControl);
	if(!ControllerCastResult) return;

	
	const UWorld* World = GetWorld();
	if(!World) return;
	
	if(UGameInstance* ReturnGameInstance = UGameplayStatics::GetGameInstance(World))
	{
		if(UParkourCombatGameInstance* GameInstanceCastResult = Cast<UParkourCombatGameInstance>(ReturnGameInstance))
		{
			const EnemyDifficulty StoringEnemyDifficulty = GameInstanceCastResult->GetStoredDifficulty();
			ControllerCastResult->OnStoreDifficulty(StoringEnemyDifficulty);

		}
	}
	
}



void AHUDManager::OnPlayerHUDVisibility(const bool bShow)
{
	if(!PlayerHUDRefValidationCheck()) return;
	
	if(bShow)
	{
		if(!PlayerHUDRef->IsInViewport()) PlayerHUDRef->AddToViewport(); SetCurrentSelectedWidgetRef(PlayerHUDRef);
	}
	else
	{
		if(PlayerHUDRef->IsInViewport()) PlayerHUDRef->RemoveFromParent();
	}
}

void AHUDManager::OnControlTypeChange()
{
	if(!TutorialWidgetValidationCheck()) return;
	
	if(!ControlSchemeRefValidationCheck()) return;
	
	if(!PlayerHUDRefValidationCheck()) return;

	// TODO: Rewind As well

	TutorialWidgetRef->OnUpdateControlUI(CurrentControlType);
	PlayerHUDRef->OnUpdateControlUI(CurrentControlType);
	ControlSchemeRef->OnUpdateControlUI(CurrentControlType);
}

void AHUDManager::OnDisplayControlScheme()
{
	if(!ControlSchemeRefValidationCheck()) return;

	const UWorld* World = GetWorld();
	if(!World) return;

	if(ControlSchemeRef->IsInViewport())
	{
		ControlSchemeRef->RemoveFromParent();
		ControlSchemeRef->SetIsWidgetOnScreen(false);
		
		if(CurrentGameStageType == eStageType::MainMenu)
			SetCurrentSelectedWidgetRef(MainMenuRef);
		else
			SetCurrentSelectedWidgetRef(PauseMenuRef);
	}
	else
	{
		ControlSchemeRef->AddToViewport();
		ControlSchemeRef->SetIsWidgetOnScreen(true);
		SetCurrentSelectedWidgetRef(ControlSchemeRef);
	}
}

// Called when the game starts or when spawned
void AHUDManager::BeginPlay()
{
	Super::BeginPlay();

	if(WidgetValidationCheck())
	{
		WidgetInitialization();
		StageInitialization();
	}
}

void AHUDManager::OnUnlockAbility(TutorialUnLockAbility UnLockingAbility)
{
	if(!PlayerHUDRefValidationCheck()) return;

	switch (UnLockingAbility)
	{
		case TutorialUnLockAbility::RangeAttack:
			PlayerHUDRef->UnlockOrLockRangeAttack(true);
			break;
		case TutorialUnLockAbility::RageAttack:
			PlayerHUDRef->UnlockOrLockRageAttack(true);
			break;
		case TutorialUnLockAbility::Dodge:
			PlayerHUDRef->UnlockOrLockDodge(true);
			if(TutorialManagerRef)
			{
				TutorialManagerRef->ActivateDodgeTutorialEnemy();
				TutorialManagerRef->DeactivateGuardTutorialEnemy();
			}
			break;
		case TutorialUnLockAbility::Guard:
			PlayerHUDRef->UnlockOrLockGuard(true);
			if(TutorialManagerRef) TutorialManagerRef->ActivateGuardTutorialEnemy();
			break;
		case TutorialUnLockAbility::Heal:
			PlayerHUDRef->UnlockOrLockHealItem(true);
			break;
		case TutorialUnLockAbility::Rewind:
			PlayerHUDRef->UnlockOrLockRewindAbility(true);
			PlayerHUDRef->ShowOrHideEnemyBossHealthBar(true);
			if(TutorialManagerRef)
			{
				TutorialManagerRef->ActivateTutorialEnemy();
				TutorialManagerRef->DeactivateDodgeTutorialEnemy();
			}
			break;
		default: ;
	}
}

void AHUDManager::OnPauseRewindRecording()
{
	if(RewindManagerRefValidationCheck()) RewindManagerRef->PauseRecording();
	if(PlayerRef) IsGameFinished = true;
}

void AHUDManager::OnTutorialStageBeginPlay()
{
	for(ATutorialPageTriggerActor* EachTrigger : TutorialPageTriggers)
	{
		EachTrigger->OnUnlockTutorialPageDelegate.AddDynamic(this, &AHUDManager::UnLockTutorialPage);
	}

	if(!TutorialWidgetValidationCheck()) return;

	const UWorld* World = GetWorld();
	if(!World) return;

	
	UGameplayStatics::SetGamePaused(World, true);
	UGameplayStatics::GetPlayerController(World,0)->bShowMouseCursor = true;
	TutorialWidgetRef->AddToViewport();
	TutorialWidgetRef->SetIsWidgetOnScreen(true);
	TutorialWidgetRef->UnlockedPageNumIncrement(3);
	TutorialWidgetRef->FlipToNewestPage();
		
	TutorialWidgetRef->OnTryToUnlockAbilityDelegate.BindDynamic(this, &AHUDManager::OnUnlockAbility);

	PlayerHUDRef->UnlockOrLockDodge(false);
	PlayerHUDRef->UnlockOrLockRangeAttack(false);
	PlayerHUDRef->UnlockOrLockRageAttack(false);
	PlayerHUDRef->UnlockOrLockGuard(false);
	PlayerHUDRef->UnlockOrLockHealItem(false);
	PlayerHUDRef->UnlockOrLockRewindAbility(false);
	PlayerHUDRef->ShowOrHideEnemyBossHealthBar(false);


	OnPlayerHUDVisibility(false);

	SetCurrentSelectedWidgetRef(TutorialWidgetRef);
}

void AHUDManager::DebugBPFunction()
{
	if(!PlayerHUDRefValidationCheck()) return;
	if(PlayerHUDRef->IsInViewport())
	{
		PlayerHUDRef->RemoveFromParent();
	}
	else
	{
		PlayerHUDRef->AddToViewport();

	}
}

bool AHUDManager::IsOnlyPauseMenuRemain()
{
	if(!TutorialWidgetValidationCheck()) return false;
	if(!PauseMenuRefValidationCheck()) return false;

	return PauseMenuRef->GetIsWidgetOnScreen() && !TutorialWidgetRef->GetIsWidgetOnScreen();
}

bool AHUDManager::IsPauseMenuOff()
{
	if(!TutorialWidgetValidationCheck()) return false;
	if(!PauseMenuRefValidationCheck()) return false;
	return !PauseMenuRef->GetIsWidgetOnScreen();
}

void AHUDManager::OnInputKeyCheck(FKey InputKey)
{
	if(InputKey.IsGamepadKey()){TryChangeControlType(ControlType::Gamepad);}
	else {TryChangeControlType(ControlType::KeyBoardAndMouse);}
	
	if(!CurrentSelectedWidgetRef)
	{
		return;
	}

	
	if(InputKey == EKeys::W || InputKey == EKeys::Gamepad_DPad_Up)
	{
		CurrentSelectedWidgetRef->OnReceiveMenuInputKey(PlayerMenuInputType::UpKey);
		return;
	}
	if(InputKey == EKeys::S || InputKey == EKeys::Gamepad_DPad_Down)
	{
		CurrentSelectedWidgetRef->OnReceiveMenuInputKey(PlayerMenuInputType::DownKey);
		return;
	}
	if(InputKey == EKeys::A || InputKey == EKeys::Gamepad_DPad_Left)
	{
		if(RewindManagerRefValidationCheck()) if(RewindManagerRef->GetIsRewinding())
		{
			RewindManagerRef->RewindSliderDecrement();
			return;
		}
		CurrentSelectedWidgetRef->OnReceiveMenuInputKey(PlayerMenuInputType::LeftKey);
		return;
	}
	if(InputKey == EKeys::D || InputKey == EKeys::Gamepad_DPad_Right)
	{
		if(RewindManagerRefValidationCheck()) if(RewindManagerRef->GetIsRewinding())
		{
			RewindManagerRef->RewindSliderDecrement();
			return;
		}
		CurrentSelectedWidgetRef->OnReceiveMenuInputKey(PlayerMenuInputType::RightKey);
		return;
	}
	if(InputKey == EKeys::F || InputKey == EKeys::Gamepad_FaceButton_Bottom)
	{
		CurrentSelectedWidgetRef->OnReceiveMenuInputKey(PlayerMenuInputType::ConfirmKey);
		return;
	}
	if(InputKey == EKeys::BackSpace || InputKey == EKeys::Gamepad_FaceButton_Right)
	{
		CurrentSelectedWidgetRef->OnReceiveMenuInputKey(PlayerMenuInputType::BackKey);
	}


}

// Called every frame
void AHUDManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

