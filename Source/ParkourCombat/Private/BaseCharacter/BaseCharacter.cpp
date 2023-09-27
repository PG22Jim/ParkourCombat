// Copyright © 2023 by Jim Chen. All rights reserved.


#include "BaseCharacter/BaseCharacter.h"

#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Attached Arrow"));;
	if(!ArrowComp) return;
	ArrowComp->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// If UWorld exist
	if(UWorld* World = GetWorld())
	{
		// set spawn parameter
		FActorSpawnParameters BaseWeaponParams;
		BaseWeaponParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Get Socket Transform
		USkeletalMeshComponent* PlayerMesh = GetMesh();
		FTransform WeaponSpawnTrans = GetMesh()->GetSocketTransform("WeaponSocket");

		// spawn Weapon and attach it to character's right hand socket
		WeaponOnHand = World->SpawnActor<ABaseWeapon>(BaseWeaponClass, WeaponSpawnTrans, BaseWeaponParams);
		if(!WeaponOnHand) return;
		const FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		WeaponOnHand->AttachToComponent(PlayerMesh, AttachmentRule, "WeaponSocket");

		// Allocate reference
		WeaponOnHand->StoreOwnerReference(this);
	}
}

bool ABaseCharacter::IsCharacterAbleToReceiveDamage(CharacterDamageType IncomingDamageType) const
{
	// return true if character is not receiving damage, not dead, not performing special attack, not in dodging state
	return !(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::Died || CurrentCombatStatus == CombatStatus::SpecialAttack || CurrentCombatStatus == CombatStatus::DodgeSuccess || CurrentCombatStatus == CombatStatus::Dodge);
}

void ABaseCharacter::CharacterOnDeath()
{
	// Stop current montage and play death montage
	StopAnimMontage();
	PlayAnimMontage(DeathMontage, 1, NAME_None);

	// set current combat status to dead and turn able to move bool to false to prevent character moving
	CurrentCombatStatus = CombatStatus::Died;
	AbleToMove = false;
}


void ABaseCharacter::PlayReceiveDamageAnimation(FVector AttackImpactPoint, CharacterDamageType ReceivingDamageType, bool bIsEnemy)
{
	// if damage receiver is player or receiving damage type is heavy damage, cancel current montage
	if(!bIsEnemy || (bIsEnemy && ReceivingDamageType == CharacterDamageType::HeavyDamage))
		StopAnimMontage();

	// If Weapon collision is enabled, disable it.
	if(WeaponOnHand)
		if(WeaponOnHand->IsWeaponCollisionEnabled())
			WeaponOnHand->EnableOrDisableWeaponMeshCollision(false);


	// get dot product of character facing dir
	FVector CharacterPosWithSameZ = GetActorLocation();
	CharacterPosWithSameZ.Z = AttackImpactPoint.Z;
	const FVector DirToImpactPoint = UKismetMathLibrary::Normal((AttackImpactPoint - CharacterPosWithSameZ));
	const FVector CharacterFacingDir = GetActorForwardVector();
	const float Dot_CharacterForward = UKismetMathLibrary::Dot_VectorVector(CharacterFacingDir, DirToImpactPoint);

	// If enemy's damage is light, 4 direction check
	if(ReceivingDamageType != CharacterDamageType::HeavyDamage)
	{
		// Damage from back
		if(Dot_CharacterForward < -0.5f)
		{
			PlayAnimMontage(ReceiveDamage_Back, 1, NAME_None);
			return;
		}

		// Damage from front
		if(Dot_CharacterForward > 0.5f)
		{
			PlayAnimMontage(ReceiveDamage_Front, 1, NAME_None);
			return;
		}

		// Damage from side
		const float Dot_CharacterRight = UKismetMathLibrary::Dot_VectorVector(GetActorRightVector(), DirToImpactPoint);
		
		// Damage from Right
		if(Dot_CharacterRight > 0.5f)
		{
			PlayAnimMontage(ReceiveDamage_Right, 1, NAME_None);
			return;
		}
		
		// Damage from Left
		PlayAnimMontage(ReceiveDamage_Left, 1, NAME_None);
		return;
	}
	
	// If damage is heavy damage, only check from front or back
	// Damage from back
	if(Dot_CharacterForward < 0)
	{
		PlayAnimMontage(ReceiveDamage_BackLarge, 1, NAME_None);
		return;
	}

	// Damage from front
	PlayAnimMontage(ReceiveDamage_FrontLarge, 1, NAME_None);
}

void ABaseCharacter::TryRewindWeaponData(WeaponRewindData NewWeaponData)
{
	if(!WeaponOnHand) return;

	// if weapon exist, set damage type and collision to recorded rewind data
	WeaponOnHand->SetDamageType(NewWeaponData.GetRecordDamageType());
	const bool IsCollisionEnable = NewWeaponData.GetIsCollisionActivated();
	WeaponOnHand->EnableOrDisableWeaponMeshCollision(IsCollisionEnable);
}

void ABaseCharacter::ForceEnableOrDisableWeaponCollision(bool bEnable)
{
	// enable or disable weapon's collision 
	if(!WeaponOnHand) return;
	WeaponOnHand->EnableOrDisableWeaponMeshCollision(bEnable);
}

void ABaseCharacter::OnEnableOrDisableWeaponCollision_Implementation(bool bEnableCollision)
{
	IBaseCharacterInterface::OnEnableOrDisableWeaponCollision_Implementation(bEnableCollision);

	// will not change weapon collision if character is in rewinding
	if(IsInRewind) return;

	// enable or disable weapon's collision 
	if(!WeaponOnHand) return;
	WeaponOnHand->EnableOrDisableWeaponMeshCollision(bEnableCollision);
}

void ABaseCharacter::OnConsumeHealItem_Implementation()
{
	IBaseCharacterInterface::OnConsumeHealItem_Implementation();

	if(IsInRewind) return;

	// decrease available heal time and recover health
	AvailableHealTime = UKismetMathLibrary::Clamp(AvailableHealTime - 1, 0, 5);
	Health = UKismetMathLibrary::FClamp(Health + HealAmount, 0.0f, MaxHealth);
}

void ABaseCharacter::OnSpawnRangeAttackProjectile_Implementation()
{
	IBaseCharacterInterface::OnSpawnRangeAttackProjectile_Implementation();

	if(IsInRewind) return;
	
	// try execute projectile spawning delegate function
	OnSpawnProjectileDelegate.ExecuteIfBound(this, GetActorRotation());
}

void ABaseCharacter::OnSuccessfulHit_Implementation(CharacterDamageType ReceivingDamageType)
{
	IBaseDamageReceiveInterface::OnSuccessfulHit_Implementation(ReceivingDamageType);

	// rage gauge increase and clamp the result
	const float RageAddingAmount = (ReceivingDamageType == CharacterDamageType::LightDamage) ? 20.0f : 10.0f;
	RageGage = UKismetMathLibrary::FClamp(RageGage + RageAddingAmount, 0, MaxRageGage);
}

void ABaseCharacter::TryDealRageAttackDamage_Implementation()
{
	IBaseCharacterInterface::TryDealRageAttackDamage_Implementation();

	// if target exist and has Base Damage Receive Interface
	if(RageTarget)
	{
		if(RageTarget->GetClass()->ImplementsInterface(UBaseDamageReceiveInterface::StaticClass()))
		{
			// get overlap location
			FVector BeginOverLapLocation = GetActorLocation();
			BeginOverLapLocation.Z = GetActorLocation().Z;
			
			const float CurrentDamage = 20;
			
			// if it has Base Damage Receive Interface, it means its base character, execute its ReceiveDamage function
			IBaseDamageReceiveInterface::Execute_ReceiveDamage(RageTarget, this, CurrentDamage, BeginOverLapLocation, CharacterDamageType::FlyingDamage);
		}

		// Clear reference 
		RageTarget = nullptr;
	}

}

void ABaseCharacter::OnStoreRageAttackTarget_Implementation(AActor* HitActor)
{
	IBaseCharacterInterface::OnStoreRageAttackTarget_Implementation(HitActor);

	// try cast the parameter actor and store it as Rage target
	if(!HitActor) return;
	
	ABaseCharacter* CastResult = Cast<ABaseCharacter>(HitActor);
	if(!CastResult) return;

	RageTarget = CastResult;
}

void ABaseCharacter::TryHeal()
{
	// execute healing if condition match
	if(CanPerformHealing() && AbleToHeal() && IsNecessaryToHeal())
		ExecuteHeal();
		
}

void ABaseCharacter::OnDamageReceived(AActor* DamageCauser, CharacterDamageType DamageType)
{
	// check if owner class has Base Damage Receive Interface
	if(DamageCauser->GetClass()->ImplementsInterface(UBaseDamageReceiveInterface::StaticClass()))
	{
		// if it has Base Damage Receive Interface, it means its base character, execute its OnSuccessfulHit function
		IBaseDamageReceiveInterface::Execute_OnSuccessfulHit(DamageCauser, DamageType);
	}
}

bool ABaseCharacter::IsCharacterRunning()
{
	// check if player's velocity length is 0 or more
	const FVector PlayerVelocity = GetVelocity();
	const float VectorLength = FMath::Sqrt(FMath::Square(PlayerVelocity.X) + FMath::Square(PlayerVelocity.Y) + FMath::Square(PlayerVelocity.Z));
	return VectorLength > 0;
}

bool ABaseCharacter::DamageReduction(float DamageAmount, bool IsBlock)
{
	// different damage calculation if character block attack
	if(IsBlock)
	{
		Health = UKismetMathLibrary::FClamp(Health - (DamageAmount - (DamageAmount * (PercentageToReduceDamage_Block / 100.0f))), 0.0f, MaxHealth);
	}
	else
	{
		Health = UKismetMathLibrary::FClamp(Health - DamageAmount, 0.0f, MaxHealth);
	}

	// return if character is still alive
	return Health > 0;
}

void ABaseCharacter::FaceToDamageCauser(AActor* DamageCauser)
{
	// calculate rotation to damage causer and face to him
	const FVector CharacterLocation = GetActorLocation();
	const FVector DamageCauserLocation = DamageCauser->GetActorLocation();

	const FVector DirToDamageCauser = UKismetMathLibrary::Normal((DamageCauserLocation - CharacterLocation));
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToDamageCauser));
}

bool ABaseCharacter::AbleToExecuteRageAttack()
{
	if(RageGage < MaxRageGage) return false;

	// if rage gauge reaches to max, reset rage gauge and return true;
	RageGage = 0;
	return true;
}

void ABaseCharacter::TryContinueRewindAnimation()
{
	// get current montage, if player is not playing montage, force to return
	UAnimMontage* CurrentPlayingMontage = GetCurrentMontage();
	if(!CurrentPlayingMontage) return;

	// Check if player is running or idle
	const bool bPlayerIsInLocomotion = (CurrentPlayingMontage == Locomotion_RunForward) || (CurrentPlayingMontage == Locomotion_Idle);

	// if character is in locomotion, just stop animation montage
	if(bPlayerIsInLocomotion)
	{
		StopAnimMontage();
		return;
	}
	
	// if character is playing montage, then set play rate to 1
	ContinueRewindMontage(CurrentPlayingMontage, ReplayMontageStartTime);
}

void ABaseCharacter::SetIsInRewind(bool Rewind)
{
	// set character and weapon to rewind state
	IsInRewind = Rewind;
	if(WeaponOnHand) WeaponOnHand->SetIsInRewind(IsInRewind);
}

void ABaseCharacter::ExecuteHeal()
{
	// early return if animation reference does not exist
	if(!UseHealItemAnim) return;

	// set combat status and play health animation
	CurrentCombatStatus = CombatStatus::Healing;
	StopAnimMontage();
	PlayAnimMontage(UseHealItemAnim, 1, "Start");
}



// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

