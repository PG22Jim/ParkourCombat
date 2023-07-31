// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterInterface.h"
#include "BaseDamageReceiveInterface.h"
#include "BaseWeapon.h"
#include "RangeAttackProjectile.h"
#include "Components/PlayerEnums.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "BaseCharacter.generated.h"


UCLASS()
class PARKOURCOMBAT_API ABaseCharacter : public ACharacter, public IBaseDamageReceiveInterface, public IBaseCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

private:

	bool AbleToHeal() const {return AvailableHealTime > 0;}
	bool CanPerformHealing() const {return CurrentCombatStatus == CombatStatus::Idle;}
	bool IsNecessaryToHeal() const {return Health < MaxHealth;}

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void CharacterOnDeath();
	
	float Health = 100.0f;
	float MaxHealth = 100.0f;
	
	bool AbleToMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	CombatStatus CurrentCombatStatus = CombatStatus::Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	UAnimMontage* UseHealItemAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	float HealAmount = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	int32 AvailableHealTime = 4;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OnDeath_Animation)
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_Front;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_Back;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_Left;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_Right;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_FrontLarge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_BackLarge;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RangeAttackSpawnSetting)
	TSubclassOf<ARangeAttackProjectile> RangeAttackClass;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponAttachment)
	TSubclassOf<ABaseWeapon> BaseWeaponClass;

	ABaseWeapon* WeaponOnHand;



	UFUNCTION()
	virtual void TryHeal();
	
public:


	CombatStatus GetCurrentCombatStatus() const {return CurrentCombatStatus;}
	void SetCurrentCombatStatus(CombatStatus NewStatus) { CurrentCombatStatus = NewStatus;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentHealth() const {return Health;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCharacterAlive() const {return Health > 0;}

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float NewHealth) {Health = UKismetMathLibrary::FClamp(NewHealth, 0, MaxHealth);}

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFrameMontage(UAnimMontage* FrameMontage, float PlayAtPosition);

	
	void ExecuteHeal();

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayReceiveDamageAnimation(FVector AttackImpactPoint, CharacterDamageType ReceivingDamageType, bool bIsEnemy = false);

	ABaseWeapon* GetWeaponOnHand() const {return WeaponOnHand;}

	UFUNCTION()
	virtual void OnEnableOrDisableWeaponCollision_Implementation(bool bEnableCollision) override;

	UFUNCTION()
	virtual void OnConsumeHealItem_Implementation() override;

	UFUNCTION()
	virtual void OnSpawnRangeAttackProjectile_Implementation() override;
	
};
