// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "RewindManager.h"
#include "TutorialManager.h"
#include "GameFramework/Actor.h"
#include "SceneActors/TutorialPageTriggerActor.h"
#include "Widget/UW_ControlScheme.h"
#include "Widget/UW_GameClearMenu.h"
#include "Widget/UW_GameOverMenu.h"
#include "Widget/UW_MainMenu.h"
#include "Widget/UW_PauseMenu.h"
#include "Widget/UW_PlayerHUD.h"
#include "Widget/UW_SelectDifficultyMenu.h"
#include "Widget/UW_TutorialWidget.h"
#include "HUDManager.generated.h"

UENUM(BlueprintType)
enum eStageType
{
	MainMenu = 0 UMETA(DisplayName = "MainMenu"),
	Tutorial = 1 UMETA(DisplayName = "Tutorial"),
	Game = 2 UMETA(DisplayName = "Game"),
};

UCLASS()
class PARKOURCOMBAT_API AHUDManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHUDManager();

protected:
	
	UPROPERTY()
	UUW_TutorialWidget* TutorialWidgetRef;

	UPROPERTY()
	UUW_ControlScheme* ControlSchemeRef;

	UPROPERTY()
	UUW_PauseMenu* PauseMenuRef;

	UPROPERTY()
	UUW_GameOverMenu* GameOverMenuRef;

	UPROPERTY()
	UUW_PlayerHUD* PlayerHUDRef;

	UPROPERTY()
	UUW_GameClearMenu* GameClearMenuRef;
	
	UPROPERTY()
	UUW_SelectDifficultyMenu* SelectDifficultyMenuRef;
	
	UPROPERTY()
	AParkourCombatCharacter* PlayerRef;

	UPROPERTY()
	AEnemyBase* EnemyBossRef;
	

	UPROPERTY()
	UUW_MainMenu* MainMenuRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ARewindManager* RewindManagerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ARewindManager> RewindManagerClass;

	ControlType CurrentControlType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UUW_WidgetWithInputDetection* CurrentSelectedWidgetRef;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<eStageType> CurrentGameStageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATutorialManager* TutorialManagerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEnemyBase> EnemyClass;

	
	
	bool ControlSchemeRefValidationCheck();
	bool GameOverMenuRefValidationCheck();
	bool TutorialWidgetValidationCheck();
	bool PauseMenuRefValidationCheck();
	bool PlayerHUDRefValidationCheck();
	bool RewindManagerRefValidationCheck();
	bool MainMenuRefValidationCheck();
	bool GameClearMenuRefValidationCheck();
	bool SelectDifficultyMenuRefValidationCheck();
	bool TryCachePlayerRef();
	bool TryCacheEnemyRef();

	bool WidgetValidationCheck();
	void WidgetInitialization();

	void TryUpdateWidgetControlUI();
	

	void TryBindPlayerRewindDelegate();

	

	UFUNCTION()
	void RewindAbilityActivate();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreateGameOverMenu();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreateControlScheme();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreateTutorialWidget();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreatePauseMenu();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreateMainMenu();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreatePlayerHUD();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreateGameClearHUD();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TryCreateSelectDifficultyMenu();

	
	UFUNCTION(BlueprintCallable)
	void SetGameOverMenuRef(UUW_GameOverMenu* NewGameOverMenu) {GameOverMenuRef = NewGameOverMenu;}
	
	UFUNCTION(BlueprintCallable)
	void SetControlSchemeRef(UUW_ControlScheme* NewControlScheme) {ControlSchemeRef = NewControlScheme;}
	
	UFUNCTION(BlueprintCallable)
	void SetTutorialWidgetRef(UUW_TutorialWidget* NewTutorialWidget) {TutorialWidgetRef = NewTutorialWidget;}

	UFUNCTION(BlueprintCallable)
	void SetMainMenuRef(UUW_MainMenu* NewMainMenu) {MainMenuRef = NewMainMenu;}
	
	UFUNCTION(BlueprintCallable)
	void SetPauseMenuRef(UUW_PauseMenu* NewPauseMenuRef) {PauseMenuRef = NewPauseMenuRef;}

	UFUNCTION(BlueprintCallable)
	void SetGameClearMenuRef(UUW_GameClearMenu* NewGameClearMenuRef) {GameClearMenuRef = NewGameClearMenuRef;}
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerHUDRef(UUW_PlayerHUD* NewPlayerHUDRef) {PlayerHUDRef = NewPlayerHUDRef;}

	UFUNCTION(BlueprintCallable)
	void SetSelectDifficultyMenuRef(UUW_SelectDifficultyMenu* NewSelectDifficultyMenuRef) {SelectDifficultyMenuRef = NewSelectDifficultyMenuRef;}
	

	void StageInitialization();
	

	UFUNCTION()
	void OnPlayerHUDVisibility(const bool bShow);

	void OnControlTypeChange();

	UFUNCTION()
	void OnDisplayControlScheme();
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting, meta = (ExposeOnSpawn = "true", OverrideNativeName = "IsTutorialStage"))
	bool IsTutorialStage = false;

	bool IsGameFinished = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Setting)
	TArray<ATutorialPageTriggerActor*> TutorialPageTriggers;

	
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnUnlockAbility(TutorialUnLockAbility UnLockingAbility);

	UFUNCTION()
	void OnPauseRewindRecording();
	
	void OnTutorialStageBeginPlay();

	UFUNCTION(BlueprintCallable)
	void DebugBPFunction();
	
	
public:	

	
	UFUNCTION(BlueprintCallable)
	void OnGameOver();

	UFUNCTION()
	void OnStageClear();

	UFUNCTION()
	void OnOpenSelectDifficultyMenu();

	UFUNCTION()
	void OnCloseSelectDifficultyMenu();
	
	bool IsOnlyPauseMenuRemain();

	UFUNCTION()
	bool IsPauseMenuOff();

	UFUNCTION(BlueprintCallable)
	void OnInputKeyCheck(FKey InputKey);

	void TryChangeControlType(ControlType NewControlType) {if(CurrentControlType != NewControlType){CurrentControlType = NewControlType; TryUpdateWidgetControlUI();}}
	

	UFUNCTION()
	ControlType GetCurrentControlType() const {return CurrentControlType;}

	UFUNCTION(BlueprintCallable)
	void SetControlType(ControlType NewControlType) {CurrentControlType = NewControlType;}
	

	UUW_PauseMenu* GetPauseMenuRef() const {return PauseMenuRef;}

	UFUNCTION()
	void UnLockTutorialPage(int32 UnlockPageNumber);
	
	UFUNCTION(BlueprintCallable)
	void DisplayTutorialWidget();

	UFUNCTION()
	void OnExitTutorialPage();

	UFUNCTION(BlueprintCallable)
	void DisplayPauseMenu();

	UFUNCTION()
	void SetCurrentSelectedWidgetRef(UUW_WidgetWithInputDetection* NewSelectedWidget) { CurrentSelectedWidgetRef = NewSelectedWidget;}
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
