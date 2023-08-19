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

void ABaseCharacter::CharacterOnDeath()
{
	StopAnimMontage();
	PlayAnimMontage(DeathMontage, 1, NAME_None);

	AbleToMove = false;
}


void ABaseCharacter::PlayReceiveDamageAnimation(FVector AttackImpactPoint, CharacterDamageType ReceivingDamageType, bool bIsEnemy)
{
	if(!bIsEnemy || (bIsEnemy && ReceivingDamageType == CharacterDamageType::HeavyDamage))
		StopAnimMontage();
	
	FVector CharacterPosWithSameZ = GetActorLocation();
	CharacterPosWithSameZ.Z = AttackImpactPoint.Z;
	const FVector DirToImpactPoint = UKismetMathLibrary::Normal((AttackImpactPoint - CharacterPosWithSameZ));
	const FVector PlayerFacingDir = GetActorForwardVector();

	const float Dot_CharacterForward = UKismetMathLibrary::Dot_VectorVector(PlayerFacingDir, DirToImpactPoint);

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

void ABaseCharacter::ForceEnableOrDisableWeaponCollision(bool bEnable)
{
	if(!WeaponOnHand) return;
	WeaponOnHand->EnableOrDisableWeaponMeshCollision(bEnable);
}

void ABaseCharacter::OnEnableOrDisableWeaponCollision_Implementation(bool bEnableCollision)
{
	IBaseCharacterInterface::OnEnableOrDisableWeaponCollision_Implementation(bEnableCollision);

	if(IsInRewind) return;
	
	if(!WeaponOnHand) return;
	WeaponOnHand->EnableOrDisableWeaponMeshCollision(bEnableCollision);
}

void ABaseCharacter::OnConsumeHealItem_Implementation()
{
	IBaseCharacterInterface::OnConsumeHealItem_Implementation();

	if(IsInRewind) return;
	
	AvailableHealTime = UKismetMathLibrary::Clamp(AvailableHealTime - 1, 0, 5);
	Health = UKismetMathLibrary::FClamp(Health + HealAmount, 0.0f, MaxHealth);
}

void ABaseCharacter::OnSpawnRangeAttackProjectile_Implementation()
{
	IBaseCharacterInterface::OnSpawnRangeAttackProjectile_Implementation();

	if(IsInRewind) return;
	
	// TODO: DELEGATE
	OnSpawnProjectileDelegate.ExecuteIfBound(this, GetActorRotation());
	//if(ArrowComp) OnSpawnProjectileDelegate.ExecuteIfBound(this, ArrowComp->GetComponentRotation());
}

void ABaseCharacter::TryHeal()
{
	
	if(CanPerformHealing() && AbleToHeal() && IsNecessaryToHeal())
		ExecuteHeal();
		
}

bool ABaseCharacter::IsCharacterRunning()
{
	const FVector PlayerVelocity = GetVelocity();
	const float VectorLength = FMath::Sqrt(FMath::Square(PlayerVelocity.X) + FMath::Square(PlayerVelocity.Y) + FMath::Square(PlayerVelocity.Z));
	return VectorLength > 0;
}

void ABaseCharacter::TryContinueRewindAnimation()
{
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
	//FString MontageName = CurrentPlayingMontage->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Yellow, FString::Printf(TEXT("MontageName: %s | PlaySecond: %f"), *MontageName, ReplayMontageStartTime));
		
}

void ABaseCharacter::ExecuteHeal()
{
	if(!UseHealItemAnim) return;
	
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

