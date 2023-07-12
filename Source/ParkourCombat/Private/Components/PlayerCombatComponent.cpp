// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Components/PlayerCombatComponent.h"

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
}

void UPlayerCombatComponent::TryExecuteCombatCommand(CombatStatus CommandStatus)
{
	if(!PlayerOwnerRef) TryGetOwnerReference();
	const ParkourStatus CurParkourStatus = PlayerOwnerRef->GetCurrentParkourStatus();
	
	if(CurParkourStatus != ParkourStatus::Idle && CurParkourStatus != ParkourStatus::Running) return;
	
	switch (CommandStatus)
	{
	case CombatStatus::Idle:

		break;
	case CombatStatus::NormalAttack:
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
		ResetComboCount();
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
		StoreBufferingCommand(CombatStatus::NormalAttack);
		return;
	}
	
	CurrentMontage = GetNormalAttackMontage();
	
	if(!CurrentMontage) return;

	PlayerOwnerRef->ClearMotionWarpingDestination_NormalAttack();
	
	const FVector InputDir = GetInputDirection();

	// Changing direction if enemy in front of suppose facing direction
	AActor* TargetEnemyActor = GetTargetEnemyFromDir(InputDir);

	if(!TargetEnemyActor)
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(InputDir));
	else
	{
		const FVector PlayerLocation = PlayerOwnerRef->GetActorLocation();
		const FVector TargetLocation = TargetEnemyActor->GetActorLocation();

		const FVector DirToEnemy = UKismetMathLibrary::Normal((TargetLocation - PlayerLocation));
		PlayerOwnerRef->SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToEnemy));

		
		const FVector DirToPlayer = UKismetMathLibrary::Normal((PlayerLocation - TargetLocation));
		const FTransform TransformToEnemy = UKismetMathLibrary::MakeTransform(TargetLocation + (DirToPlayer * 100.0f), PlayerOwnerRef->GetActorRotation());
		
		PlayerOwnerRef->UpdateMotionWarpingDestination_NormalAttack(TransformToEnemy);
	}

	PlayerOwnerRef->StopAnimMontage();
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1.0f, "Default");

	// Set PreAction State
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::PreAttacking);

	//
	//UKismetMathLibrary::MakeRotFromX(SupposeFacingDir)
	
	CurrentComboCount++;
}

UAnimMontage* UPlayerCombatComponent::GetNormalAttackMontage()
{
	if(CurrentComboCount >= NormalAttacks.Num()) return nullptr;
	
	return NormalAttacks[CurrentComboCount];
}


// =================================================== Parry Action ===========================================
void UPlayerCombatComponent::ExecuteParry()
{
	
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const CombatStatus CurrentCombatStatus = PlayerOwnerRef->GetCurrentCombatStatus();
	if(CurrentCombatStatus == CombatStatus::NormalAttack || CurrentCombatStatus == CombatStatus::PreAttacking)
	{
		StoreBufferingCommand(CombatStatus::Parry);
		return;
	}

	CurrentMontage = BeginParryMontage;
	if(!CurrentMontage) return;
	
	PlayerOwnerRef->StopAnimMontage();
	
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::Parry);

	PlayerOwnerRef->SetCurrentParryStatus(ParryStatus::BeginParry);
	
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1, NAME_None);

	
}

void UPlayerCombatComponent::TryCancelParry()
{
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const ParryStatus CurrentStatus = PlayerOwnerRef->GetCurrentParryStatus();
	
	if(CurrentStatus == ParryStatus::BeginParry)
	{
		StoreBufferingCommand(CombatStatus::Idle);
		return;
	}
	else if(CurrentStatus == ParryStatus::Parrying)
	{
		CancelParry();
	}
	
}

void UPlayerCombatComponent::CancelParry()
{
	CurrentMontage = EndParryMontage;
	if(!CurrentMontage) return;

	PlayerOwnerRef->StopAnimMontage();
	
	PlayerOwnerRef->SetCurrentParryStatus(ParryStatus::CancelParry);
	PlayerOwnerRef->SetCurrentCombatStatus(CombatStatus::Idle);

	
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage, 1, NAME_None);

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
		
		return (CurrentStatus == CombatStatus::Idle || CurrentStatus == CombatStatus::BeforeRecovering) && PlayerOwnerRef->GetCurrentParryStatus() == ParryStatus::Idle;
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
			ResetBufferCommand();
			if(PlayerOwnerRef->GetCurrentParryStatus() == ParryStatus::Parrying) CancelParry();
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

AActor* UPlayerCombatComponent::GetTargetEnemyFromDir(FVector AimDir)
{
	const UWorld* World = GetWorld();
	if(!World) return nullptr;

	
	if(!PlayerOwnerRef) TryGetOwnerReference();

	const FVector PlayerCurrentPos = PlayerOwnerRef->GetActorLocation();
	
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> ResultActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);

	
	const bool bActorInRange = UKismetSystemLibrary::SphereOverlapActors(World, PlayerCurrentPos, NormalAtkRadius, FilterType, FilteringClass, IgnoreActors, ResultActors);
	
	if(bActorInRange)
	{
		AActor* ClosestEnemy = nullptr;
		float DistanceToClosestEnemy = NormalAtkRadius;
		
		for(AActor* EachEnemyActor : ResultActors)
		{
			FVector EachActorLocation = EachEnemyActor->GetActorLocation();
			EachActorLocation.Z = PlayerCurrentPos.Z;

			const FVector DirToEachActor = UKismetMathLibrary::Normal((EachActorLocation - PlayerCurrentPos));
			const float EachActorDotProduct = UKismetMathLibrary::Dot_VectorVector(AimDir, DirToEachActor);

			if(EachActorDotProduct < 0.5f) continue;

			
			const float EachActorDistance = UKismetMathLibrary::Abs(UKismetMathLibrary::Vector_Distance(PlayerCurrentPos, EachActorLocation));
			if(EachActorDistance >= DistanceToClosestEnemy) continue;

			ClosestEnemy = EachEnemyActor;
			DistanceToClosestEnemy = EachActorDistance;
		}

		return ClosestEnemy;
	}
	
	return nullptr;
}


// Called every frame
void UPlayerCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

