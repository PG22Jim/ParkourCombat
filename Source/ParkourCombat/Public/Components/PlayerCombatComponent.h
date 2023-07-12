// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerComponent.h"
#include "PlayerEnums.h"
#include "Enemy/EnemyBase.h"
#include "PlayerCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PARKOURCOMBAT_API UPlayerCombatComponent : public UBasePlayerComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerCombatComponent();

private:


	UAnimMontage* CurrentMontage;



	// Timer Handle for calling functions in time
	FTimerHandle BufferTimerHandle;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	CombatStatus BufferingAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentComboCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BufferDuration = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	float NormalAtkRadius = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TSubclassOf<UDamageType> DamageType;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Animation)
	TArray<UAnimMontage*> NormalAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* BeginParryMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* EndParryMontage;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DrawArrowVec;

	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitializeDelegate() override;

	UFUNCTION()
	void TryExecuteCombatCommand(CombatStatus CommandStatus);

	UFUNCTION()
	void OnFinishCombatAction(CombatStatus FinishStatus);

	UFUNCTION()
	void ResetBufferCommand();
	

	// ====================================================== Normal Attack ================================================
	void ExecuteNormalAttack();

	UAnimMontage* GetNormalAttackMontage();

	void ResetComboCount() {CurrentComboCount = 0;}

	
	// =================================================== Parry Action ===========================================
	void ExecuteParry();

	UFUNCTION()
	void TryCancelParry();

	void CancelParry();
	

	// ====================================================== Utility ================================================
	bool CanExecuteCommand(CombatStatus RequestingCommand);

	void StoreBufferingCommand(CombatStatus BufferingActionCommand);

	UFUNCTION()
	void BufferActionCheck();
	
	FVector GetInputDirection();

	AActor* GetTargetEnemyFromDir(FVector AimDir);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


		
};

