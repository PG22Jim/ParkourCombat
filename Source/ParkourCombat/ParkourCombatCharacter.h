// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ParkourMovementLinkedList.h"
#include "BaseCharacter/BaseCharacter.h"
#include "Player/ParkourInterface.h"
#include "Player/PlayerCombatActionInterface.h"
#include "ParkourCombatCharacter.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnExecuteFinishParkourAction);
DECLARE_DYNAMIC_DELEGATE(FOnUpdateParkourDestination);
DECLARE_DYNAMIC_DELEGATE(FOnStartRewind);
DECLARE_DYNAMIC_DELEGATE(FRegisterParkourComponent);
DECLARE_DYNAMIC_DELEGATE(FJumpExecution);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnExecuteCombatAction, CombatStatus, CombatCommand);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnFinishCombatAction, CombatStatus, CombatCommand);
DECLARE_DYNAMIC_DELEGATE(FOnCancelParry);
DECLARE_DYNAMIC_DELEGATE(FOnBufferCheck);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnParryCounterAttack, AActor*, CounteringEnemy);
DECLARE_DYNAMIC_DELEGATE_FourParams(FOnReceiveDamage, AActor*, DamageCauser, bool, PlayerBlocking, FVector, DamageReceiveLocation, CharacterDamageType, ReceiveDamageType);
DECLARE_DYNAMIC_DELEGATE(FOnDodgeSuccess);
DECLARE_DYNAMIC_DELEGATE(FCancelChargeAttack);
DECLARE_DYNAMIC_DELEGATE(FPerformChargeAttack);
DECLARE_DYNAMIC_DELEGATE(FOnPauseGame);
DECLARE_DYNAMIC_DELEGATE(FOnPlayerDeath);
DECLARE_DYNAMIC_DELEGATE(FOnPlayerPauseRecordOnDeath);



UCLASS(config=Game)
class AParkourCombatCharacter : public ABaseCharacter, public IParkourInterface, public IPlayerCombatActionInterface
{
private:

	ParkourPositionData* CurrentMotionWarpDest;
	
	float MeshCapsuleVerticalOffset = 0;
	float JumpClimbStartZ = 0;

	float CurrentStaminaCooldown = 0;
	
	UAnimMontage* WallClimbMontage = nullptr;

	class UPlayerParkourComponent* OwningParkourComponent;

	FTimerHandle RecordPositionTimerHandle;
	FTimerHandle BackTrackingTimerHandle;
	FTimerHandle StopBackTrackTimerHandle;
	FTimerHandle TargetLockTickTimerHandle;
	FTimerHandle StaminaRegenTimerHandle;
	FTimerHandle StaminaRegenCoolDownTimerHandle;

	AActor* AttackTarget;
	AActor* LockTarget;

	void ResetPlayerMeshPosition();
	bool CanMove();
	bool IsParryingDamage(CharacterDamageType ReceiveDamageType);
	bool IsBlockingDamage(CharacterDamageType ReceiveDamageType);

	int32 CurrentComboCount = 0;

	bool IsChargeAttack = false;

	bool EnableDodge = true;
	bool EnableGuard = true;
	bool EnableRangeAttack = true;
	bool EnableRageAttack = true;
	bool EnableHealItem = true;
	bool EnableRewindAbility = true;

protected:


	UFUNCTION()
	void ClearRecordFunction(const FInputActionValue& Value);


	virtual void CharacterOnDeath() override;
	
	
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Attack Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* NormalAttackAction;

	/** Parry Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ParryAction;

	/** Cancel Parry Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelParryAction;

	/** Heal Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* HealAction;

	/** Range Attack Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RangeAttack;

	/** Dodge Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DodgeAction;

	/** Charge Attack Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChargeAttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelChargeAttack;

	/** Rage Attack Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RageAttackAction;
	
	/** Rewind Ability Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RewindAbilityAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Pause Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MovementInputVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	ParkourStatus CurrentParkourStatus = ParkourStatus::Idle;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RevengeMeter = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RevengeMeterAddingAmount = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRevenge = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool ActivateParry = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RewindAvailableTime = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxRewindAvailableTime = 4;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= DebugSetting)
	AActor* DebugCounterTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= DebugSetting)
	bool Debug_DamageBlockable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= DebugSetting)
	float Debug_DamageAmount = 100.0f;

	
public:

	FOnExecuteFinishParkourAction OnExecuteFinishParkourAction;
	FOnUpdateParkourDestination OnUpdateParkourDestination;
	FOnStartRewind OnStartRewindDelegate;
	FJumpExecution OnJumpExecuteDelegate;
	FOnExecuteCombatAction OnExecuteCombatAction;
	FOnFinishCombatAction OnFinishCombatAction;
	FOnCancelParry OnCancelParry;
	FOnBufferCheck OnBufferCheck;
	FOnParryCounterAttack OnParryCounterAttack;
	FOnReceiveDamage OnReceiveDamage;
	FOnDodgeSuccess OnDodgeSuccessDelegate;
	FCancelChargeAttack CancelChargeAttackDelegate;
	FPerformChargeAttack PerformChargeAttackDelegate;
	FOnPlayerPauseRecordOnDeath OnPlayerPauseRecordOnDeathDelegate;

	FOnPlayerDeath PlayerDeathDelegate;
	
	FOnPauseGame OnPauseGameDelegate;

	ParkourMovementLinkedList* StoredDestinationList = new ParkourMovementLinkedList();
	BackTrackData_Stack* CurrenBackTrackData = new BackTrackData_Stack();

	
	AParkourCombatCharacter();

protected:

	/** Called for movement input */
	UFUNCTION()
	void TryJump();
	
	UFUNCTION()
	void TryNormalAttack();

	UFUNCTION()
	void TryChargeAttack();
	
	UFUNCTION()
	void TryCancelChargeAttack();
	
	UFUNCTION()
	void TryParry();
	
	UFUNCTION()
	void TryCancelParry();

	UFUNCTION()
	void TryRangeAttack();

	UFUNCTION()
	void TryDodge();

	UFUNCTION()
	void TryRageAttack();

	UFUNCTION()
	void TryPauseGame() {OnPauseGameDelegate.ExecuteIfBound();}

	virtual void TryHeal() override;

	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	FVector TeleportPoint = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	bool DebugClimb = false;
	
	UFUNCTION()
	void OnResumeMeshZPosition(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void TickLockOn();

	UFUNCTION()
	void StaminaTickRegen();

	UFUNCTION()
	void StaminaCoolDownTick();

	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void LinkListTest_AddRandomData();	

	UFUNCTION(BlueprintCallable)
	void LinkListTest_PrintAll();

	UFUNCTION(BlueprintCallable)
	void LinkListTest_Pop();
	
	UFUNCTION(BlueprintCallable)
	void LinkListTest_ClearAll();

	UFUNCTION(BlueprintCallable)
	void LinkListTest_DrawPosSphere();

	UFUNCTION()
	void OnRewindButtonPressed();
	
	UFUNCTION(BlueprintImplementableEvent)
	void DrawTransSphere(FTransform DrawingTrans);


	UFUNCTION()
	void RecordCurrentPositionToList();
	
	UFUNCTION()
	void BackTrackTransform();

	UFUNCTION()
	void StopBackTracking();
	
public:



	// ================================================== Get And Set ============================================================

	void ResetComboCount() {CurrentComboCount = 0;}

	UFUNCTION()
	void SetComboCount(int32 NewComboCount) {CurrentComboCount = NewComboCount;}
	UFUNCTION()
	int32 GetComboCount() const {return CurrentComboCount;}
	
	
	ParkourStatus GetCurrentParkourStatus() const {return CurrentParkourStatus;}
	void SetCurrentParkourStatus(ParkourStatus NewStatus) { CurrentParkourStatus = NewStatus;}
	bool InRequestParkourState(ParkourStatus RequestStatus) const {return CurrentParkourStatus == RequestStatus;}

	bool OnCostStamina(float CostAmount);
	void StartStaminaRegenCoolDown();
	
	int32 GetRewindAvailableTime() const {return RewindAvailableTime;}
	int32 GetMaxRewindAvailableTime() const {return MaxRewindAvailableTime;}
	bool OnRewindAbilityUseConfirm() {RewindAvailableTime--; return RewindAvailableTime > 0;}
	bool IsAbleRewind() const {return RewindAvailableTime > 0;}
	void OnRewindTutorial() { MaxRewindAvailableTime = 99; RewindAvailableTime = MaxRewindAvailableTime; }
	void RewindProperty(float CurrentPlayerHP, float CurrentPlayerStamina, float CurrentPlayerRevenge, float CurrentPlayerRageGauge, int ComboCount);
	
	
	
	
	AActor* GetAttackTarget() const {return AttackTarget;}
	void SetAttackTarget(AActor* NewTarget) {AttackTarget = NewTarget;}
	void ClearAttackTarget() {AttackTarget = nullptr;}

	bool GetIsChargeAttack() const {return IsChargeAttack;}
	void SetIsChargeAttack(bool bChargeAttack) {IsChargeAttack = bChargeAttack;}

	bool GetAbleToMove() const {return AbleToMove;}
	void SetAbleToMove(bool bPaused) {AbleToMove = bPaused;}

	float GetRevengeMeter() const {return RevengeMeter;}
	float GetMaxRevenge() const {return MaxRevenge;}

	
	FVector GetMovementInputVector() const {return MovementInputVector;}

	bool IsPlayerInvisible() const;
	
	bool RevengeMeterIncrease();
	void ResetRevengeMeter() {RevengeMeter = 0.0f;}
	
	UAnimMontage* GetCurrentAnimMontage() const;
	
	UAnimMontage* GetWallClimbMontage() const {return WallClimbMontage;}
	void SetWallClimbMontage(UAnimMontage* NewMontage) {WallClimbMontage = NewMontage;}
	

	bool GetEnableDodge() const {return EnableDodge;}
	bool GetEnableGuard() const {return EnableGuard;}
	bool GetEnableRangeAttack() const {return EnableRangeAttack;}
	bool GetEnableRageAttack() const {return EnableRageAttack;}
	bool GetEnableHealItem() const {return EnableHealItem;}

	void SetEnableDodge(bool bEnable) {EnableDodge = bEnable;}
	void SetEnableGuard(bool bEnable) {EnableGuard = bEnable;}
	void SetEnableHealItem(bool bEnable) {EnableHealItem = bEnable;}
	void SetEnableRageAttack(bool bEnable) {EnableRageAttack = bEnable;}
	void SetEnableRangeAttack(bool bEnable) {EnableRangeAttack = bEnable;}
	void SetEnableRewindAbility(bool bEnable) {EnableRewindAbility = bEnable;}


	void PauseStaminaRegenTimers();
	void ResumeStaminaRegenTimers();
	
	
	
	void UpdateJumpClimbStartZ();
	void PlayReceiveRageAttackAnimation();
	

	void FocusEnemy(AActor* LockActor);
	void UnFocusEnemy();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_Vaulting(FTransform FirstDest, FTransform SecondDest, FTransform ThirdDest, FTransform FinalDest);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_Sliding(FTransform FirstDest, FTransform SecondDest);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_Climbing(FTransform FirstDest, FTransform SecondDest, FTransform ThirdDest, FTransform FinalDest);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_NormalAttack(FTransform TransformToTarget);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ClearMotionWarpingDestination_NormalAttack();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_CounterAttack(FTransform TransformToTarget);
	
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateCurrentMotionWarpingDest(FTransform NewTransform);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DebugSphere(FVector Pos);

	UFUNCTION(BlueprintCallable)
	void DebugReceiveDamage();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void OnUpdateDestination_Implementation() override;
	virtual void OnParkourActionEnd_Implementation() override;
	virtual void OnUpdateMeshPosition_Implementation() override;
	virtual void OnFinishNormalAttack_Implementation() override;
	virtual void OnEnterSpecificState_Implementation(CombatStatus State) override;
	virtual void OnUpdateRotationToTarget_Implementation() override;
	virtual void ReceiveDamage_Implementation(AActor* DamageCauser, float DamageAmount, FVector DamageReceiveLocation, CharacterDamageType ReceivingDamageType) override;
	virtual void OnFinishHealItemAnim_Implementation() override;
	virtual void OnFinishDodgeSuccessTime_Implementation() override;
	virtual void OnPlayerDeath_Implementation() override;
};


