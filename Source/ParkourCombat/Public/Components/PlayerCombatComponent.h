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

	UPROPERTY()
	UAnimMontage* CurrentMontage;
	
	// Timer Handle for calling functions in time
	FTimerHandle BufferTimerHandle;
protected:

	// combat variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	CombatStatus BufferingAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BufferDuration = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	float NormalAtkRadius = 300.0f;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	float LongRangeAttackRadius = 1500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TSubclassOf<UDamageType> DamageType;

	// montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Animation)
	TArray<UAnimMontage*> NormalAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Animation)
	UAnimMontage* ChargeAttack;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Animation)
	UAnimMontage* RageAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Attack_Animation)
	UAnimMontage* RangeAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* BeginParryMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* EndParryMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* ParryCounterMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* BlockReceiveDamageMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Parry_Animation)
	UAnimMontage* BlockRevengeMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Dodge_Animation)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Dodge_Animation)
	UAnimMontage* DodgeSuccessMontage_Right;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Dodge_Animation)
	UAnimMontage* DodgeSuccessMontage_Left;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Dodge_Animation)
	UAnimMontage* DodgeCounterMontage;

	
	
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

	
	// ====================================================== Charge Attack ================================================

	void TryChargeAttack();

	UFUNCTION()
	void ExecuteChargeAttack();

	// ====================================================== Charge Attack ================================================
	UFUNCTION()
	void ExecuteRageAttack();
	
	
	// =================================================== Parry Action ===========================================
	void ExecuteParry();

	UFUNCTION()
	void TryCancelParry();

	void CancelParry();

	UFUNCTION()
	void ExecuteBlocking(AActor* CounteringEnemy);

	UFUNCTION()
	void ExecuteParryCounterAttack(AActor* CounteringEnemy);
	
	// =================================================== Range Attack Action ===========================================
	UFUNCTION()
	void ExecuteRangeAttack();

	void PerformRangeAttack();

	UFUNCTION()
	void CancelChargeAttack();
	
	// =================================================== Dodge Action ===========================================

	UFUNCTION()
	void ExecuteDodge();

	UFUNCTION()
	void OnDodgeSuccess();

	UFUNCTION()
	void ExecuteDodgeCounterAttack();

	
	// ====================================================== Utility ================================================
	UFUNCTION()
	void OnReceiveDamageFromEnemy(AActor* DamageCauser, bool IsBlockingDamage, FVector DamageReceiveLocation, CharacterDamageType ReceiveDamageType);

	AActor* TryGetFrontEnemy(float MinAcceptableDot);
	
	bool CanExecuteCommand(CombatStatus RequestingCommand);

	void StoreBufferingCommand(CombatStatus BufferingActionCommand);

	UFUNCTION()
	void BufferActionCheck();
	
	FVector GetInputDirection();

	AActor* GetTargetEnemyFromDir(FVector AimDir, float MinDot, bool bIsCloseRangeAttack);

	FTransform GetAttackTargetTransform(const AActor* TargetActor, float AttackTravelLimitation);

	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


		
};


