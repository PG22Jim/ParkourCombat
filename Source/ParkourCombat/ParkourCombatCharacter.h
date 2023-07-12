// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ParkourMovementLinkedList.h"
#include "Components/TimelineComponent.h"
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




UCLASS(config=Game)
class AParkourCombatCharacter : public ACharacter, public IParkourInterface, public IPlayerCombatActionInterface
{
private:

	ParkourPositionData* CurrentMotionWarpDest;

	float MeshCapsuleVerticalOffset = 0;
	float JumpClimbStartZ = 0;

	UAnimMontage* WallClimbMontage = nullptr;

	class UPlayerParkourComponent* OwningParkourComponent;

	FTimerHandle RecordPositionTimerHandle;
	FTimerHandle BackTrackingTimerHandle;
	FTimerHandle StopBackTrackTimerHandle;

	void ResetPlayerMeshPosition();

	
protected:


	UFUNCTION()
	void ClearRecordFunction(const FInputActionValue& Value);
	
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CancelParryAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MovementInputVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	ParkourStatus CurrentParkourStatus = ParkourStatus::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	CombatStatus CurrentCombatStatus = CombatStatus::Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	ParryStatus CurrentParryStatus = ParryStatus::Idle;
	
public:

	FOnExecuteFinishParkourAction OnExecuteFinishParkourAction;
	FOnUpdateParkourDestination OnUpdateParkourDestination;
	FOnStartRewind OnStartRewindDelegate;
	FJumpExecution OnJumpExecuteDelegate;
	FOnExecuteCombatAction OnExecuteCombatAction;
	FOnFinishCombatAction OnFinishCombatAction;
	FOnCancelParry OnCancelParry;
	FOnBufferCheck OnBufferCheck;

	ParkourMovementLinkedList* StoredDestinationList = new ParkourMovementLinkedList();
	BackTrackData_Stack* CurrenBackTrackData = new BackTrackData_Stack();

	
	AParkourCombatCharacter();

protected:

	/** Called for movement input */
	UFUNCTION()
	void TryJump();

	/** Called for movement input */
	UFUNCTION()
	void TryNormalAttack();
	
	UFUNCTION()
	void TryParry();

	UFUNCTION()
	void TryCancelParry();
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	FVector TeleportPoint = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	bool DebugClimb = false;
	
	UFUNCTION()
	void OnResumeMeshZPosition(UAnimMontage* Montage, bool bInterrupted);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

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
	ParkourStatus GetCurrentParkourStatus() const {return CurrentParkourStatus;}
	void SetCurrentParkourStatus(ParkourStatus NewStatus) { CurrentParkourStatus = NewStatus;}
	bool InRequestParkourState(ParkourStatus RequestStatus) const {return CurrentParkourStatus == RequestStatus;}

	FVector GetMovementInputVector() const {return MovementInputVector;}
	
	
	CombatStatus GetCurrentCombatStatus() const {return CurrentCombatStatus;}
	void SetCurrentCombatStatus(CombatStatus NewStatus) { CurrentCombatStatus = NewStatus;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ParryStatus GetCurrentParryStatus() const {return CurrentParryStatus;}

	void SetCurrentParryStatus(ParryStatus NewStatus) { CurrentParryStatus = NewStatus;}
	
	UAnimMontage* GetCurrentAnimMontage() const;
	
	UAnimMontage* GetWallClimbMontage() const {return WallClimbMontage;}
	void SetWallClimbMontage(UAnimMontage* NewMontage) {WallClimbMontage = NewMontage;}

	void UpdateJumpClimbStartZ();



	
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
	void UpdateCurrentMotionWarpingDest(FTransform NewTransform);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DebugSphere(FVector Pos);


	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void OnUpdateDestination_Implementation() override;
	virtual void OnParkourActionEnd_Implementation() override;
	virtual void OnUpdateMeshPosition_Implementation() override;
	virtual void OnFinishNormalAttack_Implementation() override;
	virtual void OnEnterSpecificState_Implementation(CombatStatus State) override;
	virtual void OnEnterSpecificParryState_Implementation(ParryStatus State) override;
	
};

