// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ParkourMovementLinkedList.h"
#include "Player/ParkourInterface.h"
#include "ParkourCombatCharacter.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnExecuteFinishParkourAction);
DECLARE_DYNAMIC_DELEGATE(FOnUpdateParkourDestination);


UCLASS(config=Game)
class AParkourCombatCharacter : public ACharacter, public IParkourInterface
{
private:

	ParkourPositionData* CurrentMotionWarpDest;


	
protected:


	
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

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;


	
public:

	FOnExecuteFinishParkourAction OnExecuteFinishParkourAction;
	FOnUpdateParkourDestination OnUpdateParkourDestination;

	ParkourMovementLinkedList* StoredDestinationList = new ParkourMovementLinkedList();
	
	AParkourCombatCharacter();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	FVector TeleportPoint = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
	bool DebugClimb = false;

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
	void LinkListTest_ClearAll();

	
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMotionWarpingDestination_Vaulting(FTransform FirstDest, FTransform SecondDest, FTransform ThirdDest, FTransform FinalDest);

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

	
};

