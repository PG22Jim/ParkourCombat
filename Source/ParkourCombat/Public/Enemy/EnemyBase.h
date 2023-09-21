// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnemyActionInterface.h"
#include "EnemyEnum.h"
#include "BaseCharacter/BaseCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnEnemyFinishTask, class UBehaviorTreeComponent*, BehaviorTreeCompRef, bool, IsInterrupted);
DECLARE_DYNAMIC_DELEGATE(FOnFinishConditionChangeActionTask);
DECLARE_DYNAMIC_DELEGATE(FOnEnemyGetInterruptedByStrike);
DECLARE_DYNAMIC_DELEGATE(FOnEnemyBeginBehaviourConditionCheck);
DECLARE_DYNAMIC_DELEGATE(FOnResetBBVariables);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnRewindEnd, EEnemyReplayCommand, RewindReplayCommand, bool, RewindExecutingTask);
DECLARE_DYNAMIC_DELEGATE(FOnEnemyDeath);
DECLARE_DYNAMIC_DELEGATE(FOnGameClear);


UCLASS()
class PARKOURCOMBAT_API AEnemyBase : public ABaseCharacter, public IEnemyActionInterface
{
	GENERATED_BODY()


	

	
private:

	EnemyDifficulty EDifficulty;


	EEnemyReplayCommand StoredFrameReplayCommand = EEnemyReplayCommand::None;
	EEnemyTaskStatus StoredFrameTaskStatus = EEnemyTaskStatus::None;

	UBehaviorTreeComponent* BTCompRef;

	FVector InitialPos = FVector(0,0,0);

	bool IsPlayerPerformChargeAttackNearby();
	void OnChangeToParryState();
	
	void OnNormalAttackContinue();
	void ResetComboCount();

	ABaseCharacter* PlayerTargetRef;

	UAnimMontage* GetNormalAttackMontage();

	bool DecideShouldContinueParry();

	float GetPlayRateOfChargeAction();
	
	void FinishBehaviorTask();
	
protected:

	FTimerHandle ContinueParryCheckTimerHandle;
	FTimerHandle ContinueLockBrainTimerHandle;
	FTimerHandle ContinueUnLockBrainTimerHandle;

	void OnStoppingBT();

	bool ExecutingTask = false;
	
	UPROPERTY(BlueprintReadOnly)
	int32 PerformNAttackComboCount = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 IndexToChargeAttack = 99;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentNAttackCount = 0;
	void OnResetNAData() { PerformNAttackComboCount = 0; IndexToChargeAttack = 99; CurrentNAttackCount = 0;}
	
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsTutorialEnemy = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attack_NormalAttack)
	TArray<UAnimMontage*> NormalAttacks_Short; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attack_NormalAttack)
	TArray<UAnimMontage*> NormalAttacks_Long; 
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attack_ChargetAttack)
	UAnimMontage* ChargeAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attack_LaunchAttack)
	UAnimMontage* LaunchAttackMontage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attack_LongRangeAttack)
	UAnimMontage* RangeAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attack_RageAttack)
	UAnimMontage* RageAttackMontage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Defend_Parry)
	float ChargeAttackParryDetectDis = 200.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Defend_Parry)
	UAnimMontage* StartParryMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Defend_Parry)
	UAnimMontage* FinishParryMontage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Defend_Parry)
	UAnimMontage* ParryReceiveDamageMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Defend_Parry)
	UAnimMontage* ParryCounterAttack;
	
	UFUNCTION()
	void NormalAttack();

	
	

	FTransform GetMoveToTargetTransform(ABaseCharacter* PlayerTarget, FVector SelfPosition, float AttackTravelLimitation);

	void ChargeAttack();

	UFUNCTION()
	void ParryContinueCheck();

	void PerformCounterAttack();

	void PerformGuardReceiveDmgAnim();

	void ResetEnemyPosAndRot();


	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_HorizontalAttack(FTransform UpdatingTransform);


	UFUNCTION(BlueprintCallable)
	void SetWeaponToZeroDamage();

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	FOnEnemyFinishTask OnEnemyFinishTask;
	FOnEnemyGetInterruptedByStrike OnEnemyGetInterruptedByStrikeDelegate;
	FOnFinishConditionChangeActionTask OnFinishConditionChangeActionTask;
	FOnResetBBVariables OnResetBBVariablesDelegate;
	FOnRewindEnd OnRewindEndDelegate;
	FOnEnemyDeath OnEnemyDeathDelegate;
	FOnGameClear OnGameClearDelegate;
	FOnEnemyBeginBehaviourConditionCheck OnEnemyBeginBehaviourConditionCheckDelegate;
	

	// Task Execution
	void ExecuteNormalAttack(int32 ExecutingComboCount, int32 IndexToExecuteChargeAttack);
	void ExecuteParry();
	void ExecuteLaunchAttack();
	void ExecuteChargeAttack();
	void ExecuteRangeAttack();
	void ExecuteRageAttack();
	void ExecuteUseHealItem();

	void ClearTaskVariables();

	UFUNCTION(BlueprintCallable)
	virtual void CharacterOnDeath() override;
	void TryStopBehaviorTree();	
	void TryResumeBehaviorTree();

	void OnTryLockBTBrain();
	void OnTryUnLockBTBrain();
	
	void TryLockBehaviourTree();
	void TryUnlockBehaviourTree();

	void OnOverwriteEnemyNASequence(EnemyNormalAttackSequence NewSequence);
	
	void OnBeginBehaviourConditionCheck();

	// Get and Set
	bool GetExecutingTask() const {return ExecutingTask;}
	void SetExecutingTask(bool NewExecutingStatus) {ExecutingTask = NewExecutingStatus;}
	
	int32 GetNAttackAnimCount_Short() const {return NormalAttacks_Short.Num();}
	int32 GetNAttackAnimCount_Long() const {return NormalAttacks_Long.Num();}

	void SetBTCompRef(UBehaviorTreeComponent* NewBTCompRef) {BTCompRef = NewBTCompRef;}

	EnemyDifficulty GetCurrentDifficulty() const {return EDifficulty;}
	void SetDifficulty(EnemyDifficulty NewDifficulty) {EDifficulty = NewDifficulty;}


	EnemyNormalAttackSequence GetCurrentNASequence() const { return EnemyNormalAttackSequence(PerformNAttackComboCount, IndexToChargeAttack, CurrentNAttackCount); }

	EEnemyReplayCommand GetStoredFrameReplayCommand() const {return StoredFrameReplayCommand;}
	void UpdateStoredFrameReplayCommand(EEnemyReplayCommand NewCommand) {StoredFrameReplayCommand = NewCommand;}
	
	EEnemyTaskStatus GetStoredFrameTaskStatus() const {return StoredFrameTaskStatus;}
	void UpdateStoredFrameTaskStatus(EEnemyTaskStatus NewTaskStatus) {StoredFrameTaskStatus = NewTaskStatus;}

	
	

	// Sets default values for this character's properties
	AEnemyBase();

	bool TryCachePlayerTargetRef();
	bool TryStorePlayerTargetRef(ABaseCharacter* PlayerTargetActor);
	ABaseCharacter* GetPlayerTargetRef() const {return PlayerTargetRef;}
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void ReceiveDamage_Implementation(AActor* PlayerActor, float DamageAmount, FVector DamageReceiveLocation, CharacterDamageType ReceivingDamageType) override;
	virtual void OnUpdateRotationToTarget_Implementation() override;

	UFUNCTION()
	virtual void OnEnemyEnterSpecificCombatState_Implementation(CombatStatus EnteringStatus) override;
	virtual void PlayChargeAttackDamageAnim_Implementation() override;
	virtual void OnGameClearWhenEnemyDie_Implementation() override;
	
	UFUNCTION()
	virtual void OnFinishHealItemAnim_Implementation() override;
	
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DebugSphere(FVector Location);
	
};
