// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerEnums.h"
#include "Components/ActorComponent.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "PlayerParkourComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PARKOURCOMBAT_API UPlayerParkourComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	float ParkourTick = 0.02f;

	void InitializeDelegate();
	void ParkourTickStart();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AParkourCombatCharacter* PlayerOwnerRef;

	UCapsuleComponent* PlayerCapsuleCompRef;
	UCharacterMovementComponent* PlayerChaMoveCompRef;

	
	
	// Called when the game starts
	virtual void BeginPlay() override;

	void TryGetOwnerReference();

	UAnimMontage* CurrentMontage;

	FTransform CurrentParkourDestination;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Vaulting)
	float MaxVaultDistance = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Vaulting)
	float GapBetweenLandPosAndWall = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Vaulting)
	float VaultHeight = 100.0f;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Animations)
	UAnimMontage* StylishJumpThroughSmallWall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Animations)
	UAnimMontage* StylishSlideThrough;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Animations)
	UAnimMontage* WallClimbToSprint;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	ParkourStatus CurrentParkourStatus = ParkourStatus::Idle;

	FTimerHandle ParkourTickTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTransform> ParkourMovingDestinations;

	

	// ============================================= Parkour =============================================

	UFUNCTION()
	void ParkourTickCheck();


	// ============================================= Running =============================================

	void EnterRunningState();

	void InRunningState();

	
	
	// ============================================= Idle =============================================

	void EnterIdleState();
	
	// ============================================= Flip Through Table =============================================
	bool VaultingCheck() const;

	void BeginVaulting();
	
	void EnterVaultingState();

	UFUNCTION(BlueprintCallable)
	void TryToGetPositionsToVault();
	
	// ============================================= Slide =============================================
	bool SlidingCheck();

	void BeginSlide();

	void EnterSlideState();


	// =================================================== Quick Wall Climb =========================================

	bool WallClimbCheck();

	void BeginWallClimb();

	void EnterWallClimbState();
	
	
	
	// ============================================= Utility =============================================

	bool IsRunning();
	
	void SetNewParkourState(ParkourStatus NewStatus) {if(CurrentParkourStatus != NewStatus) CurrentParkourStatus = NewStatus;}

	void BeginParkourAction();

	UFUNCTION()
	void FinishParkourAction();

	void ClearParkourList();

	void StoreFoundDestinations(TArray<FVector> FoundDestArray);

	UFUNCTION()
	void OnUpdateDestination();
	
	UFUNCTION(BlueprintImplementableEvent)
	void MotionWrapUpdatDestination(FTransform NextDestination);
	

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TestFunction(FVector Destination);
	
	
public:
	
	// Sets default values for this component's properties
	UPlayerParkourComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

inline void UPlayerParkourComponent::OnUpdateDestination()
{
	// // TODO: Use Linkedlist instead
	// if(CurrentParkourDestination == FTransform())
	// {
	// 	
	// }
	// int32 CurrentIndex = ParkourMovingDestinations.Find(CurrentParkourDestination);
	
}
