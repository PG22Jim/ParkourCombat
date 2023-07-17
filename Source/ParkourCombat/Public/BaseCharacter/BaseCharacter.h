// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterInterface.h"
#include "BaseDamageReceiveInterface.h"
#include "BaseWeapon.h"
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

	
	float Health = 100.0f;
	float MaxHealth = 100.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	CombatStatus CurrentCombatStatus = CombatStatus::Idle;


	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	UAnimMontage* UseHealItemAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	float HealAmount = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	int32 AvailableHealTime = 4;

	
	
	
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

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float NewHealth) {Health = UKismetMathLibrary::FClamp(NewHealth, 0, MaxHealth);}

	void ExecuteHeal();

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayReceiveDamageAnimation(FVector AttackImpactPoint, CharacterDamageType ReceivingDamageType);

	ABaseWeapon* GetWeaponOnHand() const {return WeaponOnHand;}

	UFUNCTION()
	virtual void OnEnableOrDisableWeaponCollision_Implementation(bool bEnableCollision) override;

	UFUNCTION()
	virtual void OnConsumeHealItem_Implementation() override;
};
