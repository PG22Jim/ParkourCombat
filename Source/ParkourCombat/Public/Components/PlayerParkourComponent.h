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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Animations)
	UAnimMontage* StylishJumpThroughSmallWall;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parkour_Animations)
	UAnimMontage* StylishSlideThrough;



	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	ParkourStatus CurrentParkourStatus = ParkourStatus::Idle;

	FTimerHandle ParkourTickTimerHandle;


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

	
	// ============================================= Slide =============================================
	bool SlidingCheck();

	void BeginSlide();

	void EnterSlideState();

	
	
	// ============================================= Utility =============================================

	bool IsRunning();
	
	void SetNewParkourState(ParkourStatus NewStatus) {if(CurrentParkourStatus != NewStatus) CurrentParkourStatus = NewStatus;}

	void BeginParkourAction();

	UFUNCTION()
	void FinishParkourAction();
	

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TestFunction();
	
	
public:
	
	// Sets default values for this component's properties
	UPlayerParkourComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
