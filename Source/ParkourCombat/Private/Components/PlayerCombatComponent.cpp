// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Components/PlayerCombatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UPlayerCombatComponent::UPlayerCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TryGetOwnerReference();
	TryGetCapsuleComponent();
	TryGetCharacterMovementComponent();
	
	InitializeDelegate();
}

void UPlayerCombatComponent::InitializeDelegate()
{
	Super::InitializeDelegate();

	PlayerOwnerRef->OnExecuteCombatAction.BindDynamic(this, &UPlayerCombatComponent::TryExecuteCombatCommand);
	PlayerOwnerRef->OnFinishCombatAction.BindDynamic(this, &UPlayerCombatComponent::OnFinishCombatAction);
	PlayerOwnerRef->OnCancelParry.BindDynamic(this, &UPlayerCombatComponent::TryCancelParry);
	PlayerOwnerRef->OnBufferCheck.BindDynamic(this, &UPlayerCombatComponent::BufferActionCheck);
	PlayerOwnerRef->OnParryCounterAttack.BindDynamic(this, &UPlayerCombatComponent::ExecuteParryCounterAttack);
	PlayerOwnerRef->OnReceiveDamage.BindDynamic(this, &UPlayerCombatComponent::OnReceiveDamageFromEnemy);
	PlayerOwnerRef->OnDodgeSuccessDelegate.BindDynamic(this, &UPlayerCombatComponent::OnDodgeSuccess);
	PlayerOwnerRef->CancelChargeAttackDelegate.BindDynamic(this, &UPlayerCombatComponent::CancelChargeAttack);
	PlayerOwnerRef->PerformChargeAttackDelegate.BindDynamic(this, &UPlayerCombatComponent::ExecuteChargeAttack);
}

void UPlayerCombatComponent::TryExecuteCombatCommand(CombatStatus CommandStatus)
{
	if(!PlayerOwnerRef) TryGetOwnerReference();
	const ParkourStatus CurParkourStatus = PlayerOwnerRef->GetCurrentParkourStatus();
	
	if(CurParkourStatus != ParkourStatus::Idle && CurParkourStatus != ParkourStatus::Running) return;
	
	// Check if player is dodge success or idle
	const CombatStatus CurCombatStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	
	switch (CommandStatus)
	{
	case CombatStatus::Idle:

		break;
	case CombatStatus::NormalAttack:
		if(CurCombatStatus == CombatStatus::DodgeSuccess)
		{
			ExecuteDodgeCounterAttack();
			break;
		}
		
		ExecuteNormalAttack();
		break;
	case CombatStatus::PreAttacking:

		break;
	case CombatStatus::Recovering:

		break;
	case CombatStatus::BeforeRecovering: break;
	case CombatStatus::Parry:
		ExecuteParry();
		break;
	case CombatStatus::Block: break;
	case CombatStatus::SpecialAttack:
		ExecuteRageAttack();
		break;
	case CombatStatus::ReceiveDamage: break;
	case CombatStatus::RangeAttack:
		ExecuteRangeAttack();
		break;
	case CombatStatus::CounterDodge:
		ExecuteDodge();
		break;
	case CombatStatus::PreChargeAttack:
		TryChargeAttack();

	default: ;
	}
}

void UPlayerCombatComponent::OnFinishCombatAction(CombatStatus FinishStatus)
{
	if(!PlayerOwnerRef) TryGetOwnerReference();
	
	switch (FinishStatus)
	{
	case CombatStatus::Idle:

		break;
	case CombatStatus::NormalAttack:
		PlayerOwnerRef->ResetComboCount();
		PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::Idle);
		break;
	case CombatStatus::PreAttacking:

		break;
	case CombatStatus::Recovering:

		break;
	case CombatStatus::BeforeRecovering: break;
	default: ;
	}
}

void UPlayerCombatComponent::ResetBufferCommand()
{
	BufferingAction = CombatStatus::Idle;
}


// ====================================================== Normal Attack ================================================

void UPlayerCombatComponent::ExecuteNormalAttack()
{

	if(!PlayerOwnerRef) TryGetOwnerReference();

	if(!CanExecuteCommand(CombatStatus::NormalAttack))
	{
		if(PlayerOwnerRef->GetCurrentCombatStatus() == CombatStatus::NormalAttack || PlayerOwnerRef->GetCurrentCombatStatus() == CombatStatus::PreAttacking) StoreBufferingCommand(CombatStatus::NormalAttack);
		return;
	}
	
	CurrentMontage = GetNormalAttackMontage();
	
	if(!CurrentMontage) return;

	ABaseWeapon* PlayerWeapon = PlayerOwnerRef->GetWeaponOnHand();
	if(!PlayerWeapon) return;

	PlayerOwnerRef->ClearAttackTarget();
	PlayerOwnerRef->ClearMotionWarpingDestination_NormalAttack();
	

	const FVector InputDir = GetInputDirection();
	
	// Changing direction if enemy in front of suppose facing direction
	AActor* TargetEnemyActor = GetTargetEnemyFromDir(InputDir, 0.5, true);

	if(!TargetEnemyActor)
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(InputDir));
	else
	{
		const FTransform TransformToEnemy = GetAttackTargetTransform(TargetEnemyActor, 350);
		PlayerOwnerRef->SetAttackTarget(TargetEnemyActor);
		PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);
	}

	PlayerWeapon->SetDamageType(CharacterDamageType::LightDamage);
	
	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1.0f, "Default");

	// Set PreAction State
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::PreAttacking);

	const int32 CurrentCombCount = PlayerOwnerRef->GetComboCount();
	PlayerOwnerRef->SetComboCount(CurrentCombCount + 1);
}

UAnimMontage* UPlayerCombatComponent::GetNormalAttackMontage()
{
	if(!PlayerOwnerRef) return nullptr;

	const int32 CurrentCombCount = PlayerOwnerRef->GetComboCount();

	if(CurrentCombCount >= NormalAttacks.Num()) return nullptr;
	
	return NormalAttacks[CurrentCombCount];
}

// =================================================== Charge Attack Action ===========================================

void UPlayerCombatComponent::TryChargeAttack()
{
	if(!PlayerOwnerRef) TryGetOwnerReference();
	if(!ChargeAttack) return;
	
	const CombatStatus CurrentCombatStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	if(CurrentCombatStatus == CombatStatus::Idle || CurrentCombatStatus == CombatStatus::BeforeRecovering)
	{
		PlayerOwnerRef->StopAnimMontage();


		const FVector InputDir = GetInputDirection();
		// Changing direction if enemy in front of suppose facing direction
		AActor* TargetEnemyActor = GetTargetEnemyFromDir(InputDir, 0.5, true);
		
		if(!TargetEnemyActor)
			PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(InputDir));
		else
		{
			const FTransform TransformToEnemy = GetAttackTargetTransform(TargetEnemyActor, 550);
			PlayerOwnerRef->SetAttackTarget(TargetEnemyActor);
			PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);
		}
		
		PlayerOwnerRef->SetIsChargeAttack(true);
		PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::PreChargeAttack);
		PlayerOwnerRef->PlayAnimMontage(ChargeAttack,0.7, "Default");
	}
}

void UPlayerCombatComponent::ExecuteChargeAttack()
{
	if(!PlayerOwnerRef) return;
	ABaseWeapon* PlayerWeapon = PlayerOwnerRef->GetWeaponOnHand();
	if(!PlayerWeapon) return;

	PlayerOwnerRef->ResetComboCount();
	
	PlayerOwnerRef->ClearMotionWarpingDestination_NormalAttack();
	
	const FVector InputDir = GetInputDirection();
	
	// Changing direction if enemy in front of suppose facing direction
	AActor* TargetEnemyActor = GetTargetEnemyFromDir(InputDir, 0.5, true);

	if(!TargetEnemyActor)
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(InputDir));
	else
	{
		const FTransform TransformToEnemy = GetAttackTargetTransform(TargetEnemyActor, 550);
		PlayerOwnerRef->SetAttackTarget(TargetEnemyActor);
		PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);
	}

	PlayerWeapon->SetDamageType(CharacterDamageType::HeavyDamage);
	PlayerWeapon->EnableOrDisableWeaponMeshCollision(true);
	
	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->SetIsChargeAttack(false);
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::ChargeAttack);
	PlayerOwnerRef->PlayAnimMontage(ChargeAttack,1.3, "ExecuteAttack");
	
}

void UPlayerCombatComponent::ExecuteRageAttack()
{
	if(!PlayerOwnerRef) return;
	ABaseWeapon* PlayerWeapon = PlayerOwnerRef->GetWeaponOnHand();
	if(!PlayerWeapon) return;
	if(!PlayerOwnerRef->AbleToExecuteRageAttack()) return;
	
	PlayerOwnerRef->ResetComboCount();

	PlayerOwnerRef->ClearMotionWarpingDestination_NormalAttack();
	
	const FVector InputDir = GetInputDirection();
	
	// Changing direction if enemy in front of suppose facing direction
	AActor* TargetEnemyActor = GetTargetEnemyFromDir(InputDir, 0.5, true);
	
	if(!TargetEnemyActor)
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(InputDir));
	else
	{
		const FTransform TransformToEnemy = GetAttackTargetTransform(TargetEnemyActor, 650);
		PlayerOwnerRef->SetAttackTarget(TargetEnemyActor);
		PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);
	}

	PlayerWeapon->SetDamageType(CharacterDamageType::FlyingDamage);
	
	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::SpecialAttack);
	PlayerOwnerRef->PlayAnimMontage(RageAttack,1, NAME_None);
}


void UPlayerCombatComponent::CancelChargeAttack()
{
	PlayerOwnerRef->ClearAttackTarget();
	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->SetIsChargeAttack(false);
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::Idle);
}


// =================================================== Parry Action ===========================================
void UPlayerCombatComponent::ExecuteParry()
{
	
	if(!PlayerOwnerRef) TryGetOwnerReference();
	
	const CombatStatus CurrentCombatStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	if(!CanExecuteCommand(CombatStatus::Parry))
	{
		if(CurrentCombatStatus == CombatStatus::NormalAttack || CurrentCombatStatus == CombatStatus::PreAttacking)
			StoreBufferingCommand(CombatStatus::Parry);
		
		return;
	}

	if(!PlayerOwnerRef->OnCostStamina(10)) return;
	
	CurrentMontage = BeginParryMontage;
	if(!CurrentMontage) return;
	
	PlayerOwnerRef->StopAnimMontage();
	
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::BeginParry);
	
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1, NAME_None);

	
	PlayerOwnerRef->ResetComboCount();
	
}

void UPlayerCombatComponent::TryCancelParry()
{
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const CombatStatus CurrentStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	
	if(CurrentStatus == CombatStatus::BeginParry)
	{
		StoreBufferingCommand(CombatStatus::CancelParry);
		return;
	}
	else if(CurrentStatus == CombatStatus::Parry)
	{
		CancelParry();
	}
	
}

void UPlayerCombatComponent::CancelParry()
{
	CurrentMontage = EndParryMontage;
	if(!CurrentMontage) return;

	PlayerOwnerRef->StopAnimMontage();
	
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::CancelParry);

	
	
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1, NAME_None);

}

void UPlayerCombatComponent::ExecuteBlocking(AActor* CounteringEnemy)
{
	CurrentMontage = BlockReceiveDamageMontage;
	if(!CurrentMontage) return;

	PlayerOwnerRef->StopAnimMontage();
	
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::Block);
	
	if(PlayerOwnerRef->RevengeMeterIncrease())
	{
		CurrentMontage = BlockRevengeMontage;
		if(!CurrentMontage) return;

		ABaseWeapon* PlayerWeapon = PlayerOwnerRef->GetWeaponOnHand();
		if(!PlayerWeapon) return;

		PlayerOwnerRef->ClearAttackTarget();
		
		PlayerWeapon->SetDamageType(CharacterDamageType::HeavyDamage);
		
		const FTransform TransformToEnemy = GetAttackTargetTransform(CounteringEnemy, 350);
		PlayerOwnerRef->SetAttackTarget(CounteringEnemy);
		PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);
		PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::SpecialAttack);
		PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1, NAME_None);
		PlayerOwnerRef->ResetRevengeMeter();
		return;
	}
	
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1, NAME_None);

}

void UPlayerCombatComponent::ExecuteParryCounterAttack(AActor* CounteringEnemy)
{
	if(!PlayerOwnerRef) TryGetOwnerReference();

	// Set Playing montage
	CurrentMontage = ParryCounterMontage;
	if(!CurrentMontage) return;

	ABaseWeapon* PlayerWeapon = PlayerOwnerRef->GetWeaponOnHand();
	if(!PlayerWeapon) return;

	PlayerOwnerRef->ClearMotionWarpingDestination_NormalAttack();
	
	PlayerWeapon->SetDamageType(CharacterDamageType::HeavyDamage);
	
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::SpecialAttack);
	PlayerOwnerRef->SetCurrentParkourStatus(ParkourStatus::Idle);

	// TODO: Store enemy's location to perform motion warping montage
	const FTransform TransformToEnemy = GetAttackTargetTransform(CounteringEnemy, 650);

	PlayerOwnerRef->SetAttackTarget(CounteringEnemy);
	
	PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);

	PlayerOwnerRef->PlayAnimMontage(CurrentMontage,1, NAME_None);
}

// ====================================================== RangeAttack ================================================
void UPlayerCombatComponent::ExecuteRangeAttack()
{
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const CombatStatus CurrentStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	if(!CanExecuteCommand(CombatStatus::RangeAttack))
	{
		if(CurrentStatus == CombatStatus::NormalAttack || CurrentStatus == CombatStatus::PreAttacking)
			StoreBufferingCommand(CombatStatus::RangeAttack);
		return;
	}

	PerformRangeAttack();
}

void UPlayerCombatComponent::PerformRangeAttack()
{
	// Set Playing montage
	CurrentMontage = RangeAttack;
	if(!CurrentMontage) return;

	const FVector InputDir = GetInputDirection();
	
	// Changing direction if enemy in front of suppose facing direction
	AActor* TargetEnemyActor = GetTargetEnemyFromDir(InputDir, 0.3, false);

	if(!TargetEnemyActor)
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(InputDir));
	else
	{
		const FVector PlayerLocation = PlayerOwnerRef->GetActorLocation();
		const FVector TargetLocation = TargetEnemyActor->GetActorLocation();

		const FVector DirToEnemy = UKismetMathLibrary::Normal((TargetLocation - PlayerLocation));
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToEnemy));
	}
	
	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage,1, NAME_None);

	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::RangeAttack);
}


// ====================================================== Dodge ================================================
void UPlayerCombatComponent::ExecuteDodge()
{
	if(!DodgeMontage) return;

	const CombatStatus CurrentCombatStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	if(CurrentCombatStatus != CombatStatus::Idle) return;

	if(!PlayerOwnerRef->OnCostStamina(10)) return;

	PlayerOwnerRef->ResetComboCount();


	// Changing direction to input direction
	const FVector InputDir = GetInputDirection();
	const FVector FacingDir = InputDir != FVector(0,0,0) ? InputDir : PlayerOwnerRef->GetActorForwardVector(); 
		
	PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(FacingDir));
	
	
	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::CounterDodge);

	PlayerOwnerRef->PlayAnimMontage(DodgeMontage,1, NAME_None);
}

void UPlayerCombatComponent::OnDodgeSuccess()
{
	if(!DodgeSuccessMontage_Right || !DodgeSuccessMontage_Left) return;
	
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::DodgeSuccess);
	PlayerOwnerRef->StopAnimMontage();

	// TODO: time slow
	
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);

	

	// play dodge success montage
	const bool bDodgeLeft = UKismetMathLibrary::RandomFloatInRange(0,100) >= 50;

	if(bDodgeLeft)
		PlayerOwnerRef->PlayAnimMontage(DodgeSuccessMontage_Left,1, NAME_None);
	
	else PlayerOwnerRef->PlayAnimMontage(DodgeSuccessMontage_Right,1, NAME_None);
}

void UPlayerCombatComponent::ExecuteDodgeCounterAttack()
{
	if(!PlayerOwnerRef) TryGetOwnerReference();
	
	if(AActor* StoredAttackTarget = PlayerOwnerRef->GetAttackTarget())
	{
		PlayerOwnerRef->ClearMotionWarpingDestination_NormalAttack();
		
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);

		// Set Playing montage
		CurrentMontage = DodgeCounterMontage;
		if(!CurrentMontage) return;

		ABaseWeapon* PlayerWeapon = PlayerOwnerRef->GetWeaponOnHand();
		if(!PlayerWeapon) return;
		
		PlayerWeapon->SetDamageType(CharacterDamageType::HeavyDamage);
	
		PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::SpecialAttack);
		PlayerOwnerRef->SetCurrentParkourStatus(ParkourStatus::Idle);

		const FTransform TransformToEnemy = GetAttackTargetTransform(StoredAttackTarget, 650);

		PlayerOwnerRef->SetAttackTarget(StoredAttackTarget);
	
		PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);

		PlayerOwnerRef->PlayAnimMontage(CurrentMontage,1, NAME_None);
	}
}

// ====================================================== Utility ================================================

void UPlayerCombatComponent::OnReceiveDamageFromEnemy(AActor* DamageCauser, bool IsBlockingDamage, FVector DamageReceiveLocation, CharacterDamageType ReceiveDamageType)
{
	
	if(IsBlockingDamage)
	{
		ExecuteBlocking(DamageCauser);
		return;
	}

	if(ReceiveDamageType == CharacterDamageType::FlyingDamage)
	{
		if(PlayerOwnerRef->GetCurrentCombatStatus() != CombatStatus::RageAttackVictim)
		{
			PlayerOwnerRef->StopAnimMontage();
			PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::RageAttackVictim);
			PlayerOwnerRef->FaceToDamageCauser(DamageCauser);
			PlayerOwnerRef->PlayReceiveRageAttackAnimation();
		}
		
		return;
		
	}
	
	
	if(PlayerOwnerRef->GetCurrentCombatStatus() == CombatStatus::Block || PlayerOwnerRef->GetCurrentCombatStatus() == CombatStatus::Parry)
	{
		if(ReceiveDamageType == CharacterDamageType::HeavyDamage)
		{
			PlayerOwnerRef->StartStaminaRegenCoolDown();
		}
	}

	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::ReceiveDamage);
	PlayerOwnerRef->PlayReceiveDamageAnimation(DamageReceiveLocation, ReceiveDamageType);
	PlayerOwnerRef->ResetComboCount();
}

// ====================================================== Utility ================================================
bool UPlayerCombatComponent::CanExecuteCommand(CombatStatus RequestingCommand)
{
	if(!PlayerOwnerRef) TryGetOwnerReference();
	
	const CombatStatus CurrentStatus = PlayerOwnerRef->GetCurrentCombatStatus();

	if(RequestingCommand == CombatStatus::NormalAttack)
	{
		return CurrentStatus == CombatStatus::Idle || CurrentStatus == CombatStatus::BeforeRecovering;
	}

	if(RequestingCommand == CombatStatus::Parry)
	{
		
		return CurrentStatus == CombatStatus::Idle || CurrentStatus == CombatStatus::BeforeRecovering;
	}

	if(RequestingCommand == CombatStatus::RangeAttack)
	{
		
		return CurrentStatus == CombatStatus::Idle || CurrentStatus == CombatStatus::BeforeRecovering;
	}
	
	return true;
}

void UPlayerCombatComponent::StoreBufferingCommand(CombatStatus BufferingActionCommand)
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	// Store buffer command
	BufferingAction = BufferingActionCommand;
	
	World->GetTimerManager().ClearTimer(BufferTimerHandle);
	World->GetTimerManager().SetTimer(BufferTimerHandle,this, &UPlayerCombatComponent::ResetBufferCommand, BufferDuration, false, -1);
}

void UPlayerCombatComponent::BufferActionCheck()
{
	// Stop buffering timer
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	World->GetTimerManager().ClearTimer(BufferTimerHandle);

	switch (BufferingAction)
	{
		case CombatStatus::Idle:
			break;
		
		case CombatStatus::NormalAttack:
			ResetBufferCommand();
			ExecuteNormalAttack();
			break;

		case CombatStatus::PreAttacking: break;
		case CombatStatus::Recovering: break;
		case CombatStatus::BeforeRecovering: break;
		
		case CombatStatus::Parry:
			ResetBufferCommand();
			ExecuteParry();
			break;
		case CombatStatus::BeginParry: break;
		case CombatStatus::CancelParry:
			ResetBufferCommand();
			CancelParry();
			break;
		case CombatStatus::PreChargeAttack:
			ResetBufferCommand();
			TryChargeAttack();
			break;
		default: ;
	}
}

FVector UPlayerCombatComponent::GetInputDirection()
{
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const FVector CurrentInputVector = PlayerOwnerRef->GetMovementInputVector();

	if(CurrentInputVector == FVector(0,0,0)) return PlayerOwnerRef->GetActorForwardVector();

	// Convert input direction to actual direction in world
	const FRotator Rotation = PlayerOwnerRef->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector SelfPos = PlayerOwnerRef->GetActorLocation();
	const FVector RightX = UKismetMathLibrary::GetRightVector(YawRotation);
	const FVector ForwardY = UKismetMathLibrary::GetForwardVector(YawRotation);
	const FVector RightInputDir = RightX * CurrentInputVector.X;
	const FVector ForwardInputDir = ForwardY * CurrentInputVector.Y;
	const FVector InputDest = SelfPos + ((RightInputDir * 50) + (ForwardInputDir * 50));

	return UKismetMathLibrary::GetDirectionUnitVector(SelfPos, InputDest);
}

AActor* UPlayerCombatComponent::GetTargetEnemyFromDir(FVector AimDir, float MinDot, bool bIsCloseRangeAttack)
{
	const UWorld* World = GetWorld();
	if(!World) return nullptr;

	
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const FVector PlayerCurrentPos = PlayerOwnerRef->GetActorLocation();
	
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> ResultActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);

	const float bDetectionRange = bIsCloseRangeAttack ? NormalAtkRadius : LongRangeAttackRadius;
	const bool bActorInRange = UKismetSystemLibrary::SphereOverlapActors(World, PlayerCurrentPos, bDetectionRange, FilterType, FilteringClass, IgnoreActors, ResultActors);
	
	if(bActorInRange)
	{
		AActor* ClosestEnemy = nullptr;
		float DistanceToClosestEnemy = LongRangeAttackRadius * 2;
		
		for(AActor* EachEnemyActor : ResultActors)
		{
			FVector EachActorLocation = EachEnemyActor->GetActorLocation();
			EachActorLocation.Z = PlayerCurrentPos.Z;

			const FVector DirToEachActor = UKismetMathLibrary::Normal((EachActorLocation - PlayerCurrentPos));
			const float EachActorDotProduct = UKismetMathLibrary::Dot_VectorVector(AimDir, DirToEachActor);

			if(EachActorDotProduct < MinDot) continue;

			
			const float EachActorDistance = UKismetMathLibrary::Abs(UKismetMathLibrary::Vector_Distance(PlayerCurrentPos, EachActorLocation));
			if(EachActorDistance >= DistanceToClosestEnemy) continue;

			ClosestEnemy = EachEnemyActor;
			DistanceToClosestEnemy = EachActorDistance;
		}

		return ClosestEnemy;
	}
	
	return nullptr;
}


FTransform UPlayerCombatComponent::GetAttackTargetTransform(const AActor* TargetActor, float AttackTravelLimitation)
{
	const FVector PlayerLocation = PlayerOwnerRef->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	const FVector DirToEnemy = UKismetMathLibrary::Normal((TargetLocation - PlayerLocation));
	PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToEnemy));


	// Object blocking checking
	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);
	IgnoreActors.Add(PlayerOwnerRef->GetWeaponOnHand());
	
	
	FVector Start = PlayerOwnerRef->GetActorLocation();
	FVector Offset = PlayerOwnerRef->GetActorForwardVector() * AttackTravelLimitation;
	FVector End = Start + Offset; 
	
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 35.0f, UEngineTypes::ConvertToTraceType(ECC_Camera), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bHit)
	{
		const FVector DirToPlayer = UKismetMathLibrary::Normal((PlayerLocation - TargetLocation));
		return UKismetMathLibrary::MakeTransform(TargetLocation + (DirToPlayer * 100.0f), PlayerOwnerRef->GetActorRotation());
	}

	return UKismetMathLibrary::MakeTransform(End, PlayerOwnerRef->GetActorRotation());
}




// Called every frame
void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

