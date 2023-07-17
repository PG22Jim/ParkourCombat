// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerComponent.h"
#include "PlayerEnums.h"
#include "PlayerParkourComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PARKOURCOMBAT_API UPlayerParkourComponent : public UBasePlayerComponent
{
	GENERATED_BODY()

private:
	float ParkourTick = 0.02f;

	void ParkourTickStart();

	bool IsPlayerDoingParkourAction();

protected:



	
	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeDelegate() override;
	virtual void TryGetOwnerReference() override;

	UAnimMontage* CurrentMontage;

	FTransform CurrentParkourDestination;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Target_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Target_Setting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Target_Setting)
	float DetectingRadius = 500.0f;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Animations)
	UAnimMontage* JumpFromWall;
	
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

	bool IsWallClimbing();

	
	// =================================================== Jump Execution ===========================================

	UFUNCTION()
	void OnJumpExecution();
	
	
	
	// ============================================= Utility =============================================

	bool IsRunning();

	bool IsExecutingCombat();
	
	void SetNewParkourState(ParkourStatus NewStatus) {if(PlayerOwnerRef->GetCurrentParkourStatus() != NewStatus) PlayerOwnerRef->SetCurrentParkourStatus(NewStatus);}

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
	void TestFunction(FVector Destination, float Radius);


	UFUNCTION(BlueprintCallable)
	void EarlyFinishParkourAction();
	
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
