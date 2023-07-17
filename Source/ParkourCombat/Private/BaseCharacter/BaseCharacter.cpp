// Copyright © 2023 by Jim Chen. All rights reserved.


#include "BaseCharacter/BaseCharacter.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(UWorld* World = GetWorld())
	{
		// set spawn parameter
		FActorSpawnParameters BaseWeaponParams;
		BaseWeaponParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Get ground location by using line trace
		USkeletalMeshComponent* PlayerMesh = GetMesh();
		FTransform WeaponSpawnTrans = GetMesh()->GetSocketTransform("WeaponSocket");

		
		WeaponOnHand = World->SpawnActor<ABaseWeapon>(BaseWeaponClass, WeaponSpawnTrans, BaseWeaponParams);
		
		if(!WeaponOnHand) return;
		const FAttachmentTransformRules AttachmentRule = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		WeaponOnHand->AttachToComponent(PlayerMesh, AttachmentRule, "WeaponSocket");

		WeaponOnHand->StoreOwnerReference(this);
	}
}

void ABaseCharacter::PlayReceiveDamageAnimation(FVector AttackImpactPoint, CharacterDamageType ReceivingDamageType)
{
	StopAnimMontage();
	
	FVector CharacterPosWithSameZ = GetActorLocation();
	CharacterPosWithSameZ.Z = AttackImpactPoint.Z;
	const FVector DirToImpactPoint = UKismetMathLibrary::Normal((AttackImpactPoint - CharacterPosWithSameZ));
	const FVector PlayerFacingDir = GetActorForwardVector();

	const float Dot_CharacterForward = UKismetMathLibrary::Dot_VectorVector(PlayerFacingDir, DirToImpactPoint);

	// Damage from back
	if(Dot_CharacterForward < -0.5f)
	{
		PlayAnimMontage(ReceiveDamage_Back, 1, NAME_None);
		return;
	}

	// Damage from front
	if(Dot_CharacterForward > 0.5f)
	{
		if(ReceivingDamageType != CharacterDamageType::HeavyDamage)
			PlayAnimMontage(ReceiveDamage_Front, 1, NAME_None);
		else
			PlayAnimMontage(ReceiveDamage_FrontLarge, 1, NAME_None);
		
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
	
}

void ABaseCharacter::OnEnableOrDisableWeaponCollision_Implementation(bool bEnableCollision)
{
	IBaseCharacterInterface::OnEnableOrDisableWeaponCollision_Implementation(bEnableCollision);

	if(!WeaponOnHand) return;
	
	WeaponOnHand->EnableOrDisableWeaponMeshCollision(bEnableCollision);
}

void ABaseCharacter::OnConsumeHealItem_Implementation()
{
	IBaseCharacterInterface::OnConsumeHealItem_Implementation();

	AvailableHealTime = UKismetMathLibrary::Clamp(AvailableHealTime - 1, 0, 5);
	Health = UKismetMathLibrary::FClamp(Health + HealAmount, 0.0f, MaxHealth);
}

void ABaseCharacter::TryHeal()
{
	if(CanPerformHealing() && AbleToHeal() && IsNecessaryToHeal())
	{
		ExecuteHeal();
	}
}

void ABaseCharacter::ExecuteHeal()
{
	if(!UseHealItemAnim) return;
	
	CurrentCombatStatus = CombatStatus::Recovering;
	
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

