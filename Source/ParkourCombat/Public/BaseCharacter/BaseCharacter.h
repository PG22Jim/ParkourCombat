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

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSpawnProjectile, AActor*, SpawnOwner, FRotator, SpawnRotation);

UCLASS()
class PARKOURCOMBAT_API ABaseCharacter : public ACharacter, public IBaseDamageReceiveInterface, public IBaseCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

private:

	bool CanPerformHealing() const {return CurrentCombatStatus == CombatStatus::Idle;}


	float ReplayMontageStartTime = 0;

	ABaseCharacter* RageTarget;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsNecessaryToHeal() const {return Health < MaxHealth;}

	bool IsCharacterAbleToReceiveDamage(CharacterDamageType IncomingDamageType) const;
	
	virtual void CharacterOnDeath();
	
	float Health = 100.0f;
	float MaxHealth = 100.0f;

	float Stamina = 100.0f;
	float MaxStamina = 100.0f;

	float RageGage = 0.0f;
	float MaxRageGage = 100.0f;

	
	
	bool AbleToMove = true;
	bool IsInRewind = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ArrowComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	
	CombatStatus CurrentCombatStatus = CombatStatus::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=100))
	float PercentageToReduceDamage_Block = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float AttackDistance = 400.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	UAnimMontage* UseHealItemAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	float HealAmount = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	int32 AvailableHealTime = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HealSetting)
	float StaminaRegenCooldownTime = 2.0f;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceive_Animation)
	UAnimMontage* ReceiveDamage_RageAttack;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion_Animation)
	UAnimMontage* Locomotion_RunForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion_Animation)
	float RunForwardAnimLength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion_Animation)
	UAnimMontage* Locomotion_Idle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Locomotion_Animation)
	float IdleAnimLength = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RangeAttackSpawnSetting)
	TSubclassOf<ARangeAttackProjectile> RangeAttackClass;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WeaponAttachment)
	TSubclassOf<ABaseWeapon> BaseWeaponClass;

	ABaseWeapon* WeaponOnHand;



	UFUNCTION()
	virtual void TryHeal();


	void OnDamageReceived(AActor* DamageCauser, CharacterDamageType DamageType);
	
public:

	FOnSpawnProjectile OnSpawnProjectileDelegate;

	bool IsCharacterRunning();
	
	bool AbleToHeal() const {return AvailableHealTime > 0;}
	int32 GetAvailableHealTime() const {return AvailableHealTime;}

	float GetAttackDistance() const {return AttackDistance;}

	CombatStatus GetCurrentCombatStatus() const {return CurrentCombatStatus;}
	void SetCurrentCombatStatus(CombatStatus NewStatus) { CurrentCombatStatus = NewStatus;}

	float GetCurrentHealthPercentage() const {return (Health / MaxHealth) * 100;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentHealth() const {return Health;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const {return MaxHealth;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCharacterAlive() const {return Health > 0;}

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float NewHealth) {Health = UKismetMathLibrary::FClamp(NewHealth, 0, MaxHealth);}

	UFUNCTION(BlueprintCallable)
	void MaxHealthAdjustment(float NewMaxHealth) {MaxHealth = NewMaxHealth; Health = MaxHealth;}

	bool DamageReduction(float DamageAmount, bool IsBlock);
	
	
	void FaceToDamageCauser(AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentStamina() const {return Stamina;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxStamina() const {return MaxStamina;}
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentStamina(float NewStamina) {Stamina = UKismetMathLibrary::FClamp(NewStamina, 0, MaxStamina);}


	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRageGage() const {return RageGage;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool AbleToExecuteRageAttack();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxRageGage() const {return MaxRageGage;}

	UFUNCTION(BlueprintCallable)
	void SetCurrentRageGage(float NewRageGage) {RageGage = UKismetMathLibrary::FClamp(NewRageGage, 0, MaxRageGage);}
	

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFrameMontage(UAnimMontage* FrameMontage, float PlayAtPosition);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ContinueRewindMontage(UAnimMontage* FrameMontage, float PlayAtPosition);

	void TryContinueRewindAnimation();
	void SetReplayMontageStartTime(float NewTime) {ReplayMontageStartTime = NewTime;}
	
	bool GetIsInRewind() const {return IsInRewind;}
	void SetIsInRewind(bool Rewind);

	

	UAnimMontage* GetRunForwardMontage() const {return Locomotion_RunForward;}
	UAnimMontage* GetIdleMontage() const {return Locomotion_Idle;}

	float GetIdleAnimLength() const {return IdleAnimLength;}
	float GetRunForwardAnimLength() const {return RunForwardAnimLength;}


	void OnRewindingPlaySpecificMontage(float PlayRate, UAnimMontage* RequestPlayingMontage) {PlayAnimMontage(RequestPlayingMontage, PlayRate, NAME_None);}
	
	
	void ExecuteHeal();


	ABaseCharacter* GetRageTarget() const {return RageTarget;}
	void SetRageTarget(ABaseCharacter* NewRageTarget) {RageTarget = NewRageTarget;}
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayReceiveDamageAnimation(FVector AttackImpactPoint, CharacterDamageType ReceivingDamageType, bool bIsEnemy = false);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABaseWeapon* GetWeaponOnHand() const {return WeaponOnHand;}

	void TryRewindWeaponData(WeaponRewindData NewWeaponData);

	void ForceEnableOrDisableWeaponCollision(bool bEnable);
	
	UFUNCTION()
	virtual void OnEnableOrDisableWeaponCollision_Implementation(bool bEnableCollision) override;

	UFUNCTION()
	virtual void OnConsumeHealItem_Implementation() override;

	UFUNCTION()
	virtual void OnSpawnRangeAttackProjectile_Implementation() override;

	UFUNCTION()
	virtual void OnSuccessfulHit_Implementation(CharacterDamageType ReceivingDamageType) override;

	UFUNCTION()
	virtual void TryDealRageAttackDamage_Implementation() override;

	UFUNCTION()
	virtual void OnStoreRageAttackTarget_Implementation(AActor* HitActor) override;
};
