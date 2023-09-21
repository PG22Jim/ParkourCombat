// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/EnemyBase.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AEnemyBase::TryCachePlayerTargetRef()
{
	if(PlayerTargetRef) return true;

	if(const UWorld* World = GetWorld())
	{
		ABaseCharacter* CastResult = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerCharacter(World,0));
		if(!CastResult) return false;

		PlayerTargetRef = CastResult;
		return true;
	}

	return false;
}

bool AEnemyBase::TryStorePlayerTargetRef(ABaseCharacter* PlayerTargetActor)
{
	if(PlayerTargetRef) return true;

	PlayerTargetRef = PlayerTargetActor;
	return true;
}


void AEnemyBase::SetWeaponToZeroDamage()
{
	WeaponOnHand->SetDamage(0,0,0);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	MaxHealthAdjustment(300);

	InitialPos = GetActorLocation();
}

void AEnemyBase::ExecuteNormalAttack(int32 ExecutingComboCount, int32 IndexToExecuteChargeAttack)
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	PerformNAttackComboCount = ExecutingComboCount;
	IndexToChargeAttack = IndexToExecuteChargeAttack;

	NormalAttack();
}

void AEnemyBase::NormalAttack()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	UAnimMontage* CurrentNormalAttackMontage = GetNormalAttackMontage();
	if(!WeaponOnHand || !CurrentNormalAttackMontage)
	{
		FinishBehaviorTask();
		return;
	}

	const bool CacheResult = TryCachePlayerTargetRef();
	if(!CacheResult)
	{
		FinishBehaviorTask();
		return;
	}
	
	const FTransform MotionWarpUpdateTransform = GetMoveToTargetTransform(PlayerTargetRef, GetActorLocation(), 350);
	UpdateMotionWarpingDestination_HorizontalAttack(MotionWarpUpdateTransform);
	
	StopAnimMontage();
	PlayAnimMontage(CurrentNormalAttackMontage, 1, NAME_None);
	
	WeaponOnHand->SetDamageType(CharacterDamageType::LightDamage);
	SetCurrentCombatStatus(CombatStatus::NormalAttack);
	
	CurrentNAttackCount++;
}


void AEnemyBase::ExecuteParry()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	
	ResetComboCount();

	const bool CacheResult = TryCachePlayerTargetRef();
	if(!CacheResult)
	{
		FinishBehaviorTask();
		return;
	}

	const FVector PlayerTargetPosition = PlayerTargetRef->GetActorLocation();

	const FVector DirToPlayerTarget = UKismetMathLibrary::Normal((PlayerTargetPosition - GetActorLocation()));
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToPlayerTarget));
	
	StopAnimMontage();

	const bool bIsHard = EDifficulty == EnemyDifficulty::Hard;
	const float BeginParryAnimSpeed = bIsHard ? 0.5f : 1.0f; 
	SetCurrentCombatStatus(CombatStatus::BeginParry);
	PlayAnimMontage(StartParryMontage, BeginParryAnimSpeed, NAME_None);
}

void AEnemyBase::ExecuteLaunchAttack()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	const bool CacheResult = TryCachePlayerTargetRef();
	if(!WeaponOnHand || !CacheResult || !LaunchAttackMontage)
	{
		FinishBehaviorTask();
		return;
	}
	
	const FTransform MotionWarpUpdateTransform = GetMoveToTargetTransform(PlayerTargetRef, GetActorLocation(), 450);
	UpdateMotionWarpingDestination_HorizontalAttack(MotionWarpUpdateTransform);
	
	StopAnimMontage();
	PlayAnimMontage(LaunchAttackMontage, 1, NAME_None);
	
	WeaponOnHand->SetDamageType(CharacterDamageType::HeavyDamage);
	SetCurrentCombatStatus(CombatStatus::LaunchAttack);
}

void AEnemyBase::ExecuteChargeAttack()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	ChargeAttack();
}

void AEnemyBase::ExecuteRangeAttack()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	const bool CacheResult = TryCachePlayerTargetRef();
	if(!WeaponOnHand || !CacheResult || !RangeAttackMontage)
	{
		FinishBehaviorTask();
		return;
	}

	const FVector PlayerTargetPosition = PlayerTargetRef->GetActorLocation();

	const FVector DirToPlayerTarget = UKismetMathLibrary::Normal((PlayerTargetPosition - GetActorLocation()));
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToPlayerTarget));

	SetCurrentCombatStatus(CombatStatus::RangeAttack);
	
	StopAnimMontage();
	PlayAnimMontage(RangeAttackMontage, 1, NAME_None);
}

void AEnemyBase::ExecuteRageAttack()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	const bool CacheResult = TryCachePlayerTargetRef();
	if(!WeaponOnHand || !CacheResult || !RangeAttackMontage)
	{
		FinishBehaviorTask();
		return;
	}

	const FTransform MotionWarpUpdateTransform = GetMoveToTargetTransform(PlayerTargetRef, GetActorLocation(), 650);
	UpdateMotionWarpingDestination_HorizontalAttack(MotionWarpUpdateTransform);
	
	StopAnimMontage();
	PlayAnimMontage(RageAttackMontage, 1, NAME_None);
	
	WeaponOnHand->SetDamageType(CharacterDamageType::FlyingDamage);
	SetCurrentCombatStatus(CombatStatus::SpecialAttack);
	RageGage = 0;
	
}

void AEnemyBase::ExecuteUseHealItem()
{
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::RageAttackVictim)
	{
		FinishBehaviorTask();
		return;
	}
	
	ExecuteHeal();
}

void AEnemyBase::ClearTaskVariables()
{
	SetExecutingTask(false);
	UpdateStoredFrameReplayCommand(EEnemyReplayCommand::None);
	OnOverwriteEnemyNASequence(EnemyNormalAttackSequence(0,99,0));
	OnResetBBVariablesDelegate.ExecuteIfBound();
}

void AEnemyBase::CharacterOnDeath()
{
	if(!DeathMontage) return;
	
	TryStopBehaviorTree();
	CurrentCombatStatus = CombatStatus::Died;
	PlayAnimMontage(DeathMontage, 1, NAME_None);
	
	OnEnemyDeathDelegate.ExecuteIfBound();
	
	Super::CharacterOnDeath();
}

void AEnemyBase::TryStopBehaviorTree()
{
	if(!BTCompRef) return;
	BTCompRef->StopLogic("Stop");
}

void AEnemyBase::TryResumeBehaviorTree()
{
	if(!BTCompRef) return;
	BTCompRef->RestartLogic();
}

void AEnemyBase::OnTryLockBTBrain()
{
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ContinueLockBrainTimerHandle,this, &AEnemyBase::TryLockBehaviourTree, 0.03f, true, -1);
	}
}

void AEnemyBase::OnTryUnLockBTBrain()
{
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ContinueUnLockBrainTimerHandle,this, &AEnemyBase::TryUnlockBehaviourTree, 0.03f, true, -1);
	}
}

void AEnemyBase::TryLockBehaviourTree()
{
	if(!BTCompRef) return;
	
	UBlackboardComponent* ReturnBBComp = BTCompRef->GetBlackboardComponent();
	if(ReturnBBComp)
	{
		ReturnBBComp->SetValueAsBool("BrainLocked", true);
		if(UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ContinueLockBrainTimerHandle);
			StopAnimMontage();
			if(InitialPos != FVector(0,0,0))
			{
				ResetEnemyPosAndRot();
			}
		}
	}
}

void AEnemyBase::TryUnlockBehaviourTree()
{
	if(!BTCompRef) return;

	
	UBlackboardComponent* ReturnBBComp = BTCompRef->GetBlackboardComponent();
	if(ReturnBBComp)
	{
		ReturnBBComp->SetValueAsBool("BrainLocked", false);
		if(UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ContinueUnLockBrainTimerHandle);

		}
	}
}



void AEnemyBase::OnOverwriteEnemyNASequence(EnemyNormalAttackSequence NewSequence)
{
	PerformNAttackComboCount = NewSequence.GetNAttackComboExecutionCount();
	IndexToChargeAttack = NewSequence.GetIndexToChargeAttackSetting();
	CurrentNAttackCount = NewSequence.GetStoredCurrentNAttackCount();

	//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Yellow, FString::Printf(TEXT("Execute Combo: %i || Index Charge: %i || Current Count: %i"), PerformNAttackComboCount, IndexToChargeAttack, CurrentNAttackCount));
}

void AEnemyBase::OnBeginBehaviourConditionCheck()
{
	OnEnemyBeginBehaviourConditionCheckDelegate.ExecuteIfBound();
}


FTransform AEnemyBase::GetMoveToTargetTransform(ABaseCharacter* PlayerTarget, FVector SelfPosition, float AttackTravelLimitation)
{
	const FVector PlayerTargetPosition = PlayerTarget->GetActorLocation();

	const FVector DirToPlayerTarget = UKismetMathLibrary::Normal((PlayerTargetPosition - SelfPosition));
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToPlayerTarget));

	// Object blocking checking
	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	IgnoreActors.Add(GetWeaponOnHand());
	
	
	FVector Start = GetActorLocation();
	FVector Offset = GetActorForwardVector() * AttackTravelLimitation;
	FVector End = Start + Offset; 
	
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 35.0f, UEngineTypes::ConvertToTraceType(ECC_Camera), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bHit)
	{
		const FVector DirToSelf = UKismetMathLibrary::Normal((SelfPosition - PlayerTargetPosition));
		return UKismetMathLibrary::MakeTransform(PlayerTargetPosition + (DirToSelf * 100.0f), GetActorRotation());
	}

	return UKismetMathLibrary::MakeTransform(End, GetActorRotation());
	

}

bool AEnemyBase::IsPlayerPerformChargeAttackNearby()
{
	if(EDifficulty != EnemyDifficulty::Hard) return false;
	
	if(!TryCachePlayerTargetRef()) return false;

	const bool IsPlayerPerformChargeAttack = PlayerTargetRef->GetCurrentCombatStatus() == CombatStatus::PreChargeAttack;
	const bool IsPlayerNearby = UKismetMathLibrary::Vector_Distance(PlayerTargetRef->GetActorLocation(), GetActorLocation()) < 200;
	if(IsPlayerPerformChargeAttack && IsPlayerNearby)
		return true;

	return false;
}

void AEnemyBase::OnChangeToParryState()
{
	StoredFrameReplayCommand = EEnemyReplayCommand::Counter;
	ExecuteParry();
}

void AEnemyBase::OnNormalAttackContinue()
{
	if(IsPlayerPerformChargeAttackNearby())
	{
		OnChangeToParryState();
		return;
	}

	
	if(CurrentNAttackCount >= IndexToChargeAttack)
	{
		ResetComboCount();
		ChargeAttack();
		return;
	}


	if (CurrentNAttackCount < PerformNAttackComboCount)
	{
		NormalAttack();
	}
}

void AEnemyBase::ResetComboCount()
{
	OnResetNAData();
}

UAnimMontage* AEnemyBase::GetNormalAttackMontage()
{
	const TArray<UAnimMontage*> PlayingCombo = (EDifficulty == EnemyDifficulty::Hard) ? NormalAttacks_Long : NormalAttacks_Short;
	
	const int32 MaxComboMontageCount = PlayingCombo.Num(); 
	
	if(CurrentNAttackCount >= MaxComboMontageCount) return nullptr;
	
	return PlayingCombo[CurrentNAttackCount];
}

bool AEnemyBase::DecideShouldContinueParry()
{
	TryCachePlayerTargetRef();
	if(!PlayerTargetRef) return false;

	const CombatStatus PlayerCombatStatus = PlayerTargetRef->GetCurrentCombatStatus();
	if(PlayerCombatStatus == CombatStatus::NormalAttack || PlayerCombatStatus == CombatStatus::PreAttacking) return true;
	
	return false;
}

float AEnemyBase::GetPlayRateOfChargeAction()
{
	if(EDifficulty == EnemyDifficulty::Easy) return 0.6f;

	if(EDifficulty == EnemyDifficulty::Normal) return 0.8f;
	
	return 1.0f;
}

void AEnemyBase::FinishBehaviorTask()
{
	if(!BTCompRef) return;
	ClearTaskVariables();
	const bool bBTIsRunning = BTCompRef->IsRunning();
	
	if(!bBTIsRunning)
	{
		BTCompRef->StartLogic();
		OnFinishConditionChangeActionTask.ExecuteIfBound();
		return;
	}
	
	const bool bFinishTaskSuccess = OnEnemyFinishTask.ExecuteIfBound(BTCompRef, false);
	if(bFinishTaskSuccess) OnFinishConditionChangeActionTask.ExecuteIfBound();
}

void AEnemyBase::OnStoppingBT()
{
	

	
}


void AEnemyBase::ChargeAttack()
{
	if(!WeaponOnHand || !ChargeAttackMontage)
	{
		FinishBehaviorTask();
		return;
	}
	
	WeaponOnHand->SetDamageType(CharacterDamageType::HeavyDamage);
	SetCurrentCombatStatus(CombatStatus::NormalAttack);

	const float PlayRate = GetPlayRateOfChargeAction();
	
	PlayAnimMontage(ChargeAttackMontage, PlayRate, "ChargeAction");
	
}

void AEnemyBase::ParryContinueCheck()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Check Continue Parry!"));
	
	const bool bContinueParry = DecideShouldContinueParry();
	if(bContinueParry) return;

	const UWorld* World = GetWorld();
	if(World == nullptr) return;
		
	World->GetTimerManager().ClearTimer(ContinueParryCheckTimerHandle);


	
	PlayAnimMontage(FinishParryMontage, 1, NAME_None);
}

void AEnemyBase::PerformCounterAttack()
{
	if(!WeaponOnHand || !ParryCounterAttack)
	{
		FinishBehaviorTask();
		return;
	}

	const bool CacheResult = TryCachePlayerTargetRef();
	if(!CacheResult)
	{
		FinishBehaviorTask();
		return;
	}
	
	const FTransform MotionWarpUpdateTransform = GetMoveToTargetTransform(PlayerTargetRef, GetActorLocation(), 650);
	UpdateMotionWarpingDestination_HorizontalAttack(MotionWarpUpdateTransform);

	WeaponOnHand->SetDamageType(CharacterDamageType::HeavyDamage);
	CurrentCombatStatus = CombatStatus::SpecialAttack;
	StopAnimMontage();
	PlayAnimMontage(ParryCounterAttack, 1, NAME_None);
}

void AEnemyBase::PerformGuardReceiveDmgAnim()
{
	StopAnimMontage();
	PlayAnimMontage(ParryReceiveDamageMontage, 1, NAME_None);

	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Blue, TEXT("Receive Damage While Blocking!"));
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	World->GetTimerManager().ClearTimer(ContinueParryCheckTimerHandle);
}

void AEnemyBase::ResetEnemyPosAndRot()
{
	SetActorLocation(InitialPos);
	SetActorRotation(FRotator(0,0,0));
}


// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::ReceiveDamage_Implementation(AActor* PlayerActor, float DamageAmount, FVector DamageReceiveLocation,
	CharacterDamageType ReceivingDamageType)
{
	Super::ReceiveDamage_Implementation(PlayerActor, DamageAmount, DamageReceiveLocation, ReceivingDamageType);

	if(!PlayerActor) return;
	
	if(CurrentCombatStatus == CombatStatus::ReceiveDamage || CurrentCombatStatus == CombatStatus::Died) return;

	
	
	if(CurrentCombatStatus == CombatStatus::BeginParry)
	{
		PerformCounterAttack();
		return;
	}

	// if enemy receive rage attack
	if(ReceivingDamageType == CharacterDamageType::FlyingDamage)
	{
		// early return if animation is nullptr
		if(!ReceiveDamage_RageAttack) return;

		// if enemy is already in receive damage state, only deal damaga
		if(CurrentCombatStatus == CombatStatus::RageAttackVictim)
		{
			if(!IsTutorialEnemy)
				if(!DamageReduction(DamageAmount, false)) CharacterOnDeath();
			return;
		}

		// stop montage and task and play receive damage animation
		StopAnimMontage();
		if(CurrentCombatStatus == CombatStatus::NormalAttack) ResetComboCount();
		FaceToDamageCauser(PlayerActor);
		CurrentCombatStatus = CombatStatus::RageAttackVictim;
		OnEnemyGetInterruptedByStrikeDelegate.ExecuteIfBound();

		// if alive, play receive rage attack animation
		if(!IsTutorialEnemy)
			if(DamageReduction(DamageAmount, false))
			{
				PlayAnimMontage(ReceiveDamage_RageAttack, 1, NAME_None);
				return;
			}

		// if dead, character on death
		CharacterOnDeath();
		return;
	}

	
	
	if(ReceivingDamageType == CharacterDamageType::HeavyDamage)// && CurrentCombatStatus != CombatStatus::Idle)
	{
		if(CurrentCombatStatus == CombatStatus::NormalAttack) ResetComboCount();
		CurrentCombatStatus = CombatStatus::ReceiveDamage;
		OnDamageReceived(PlayerActor, ReceivingDamageType);
		OnEnemyGetInterruptedByStrikeDelegate.ExecuteIfBound();
		if(!IsTutorialEnemy)
			if(!DamageReduction(DamageAmount, false))
			{
				// if dead, character on death
				CharacterOnDeath();
				return;
			}
	}
	else if(ReceivingDamageType == CharacterDamageType::LightDamage)
	{
		if(CurrentCombatStatus == CombatStatus::Parry)
		{
			if(!IsTutorialEnemy)
				if(!DamageReduction(DamageAmount, false))
				{
					// if dead, character on death
					CharacterOnDeath();
					return;
				}
			
			PerformGuardReceiveDmgAnim();
			OnDamageReceived(PlayerActor, ReceivingDamageType);
			return;
		}

		if(!DamageReduction(DamageAmount, false))
		{
			// if dead, character on death
			CharacterOnDeath();
			return;
		}
		
		OnDamageReceived(PlayerActor, ReceivingDamageType);
	}
	
	PlayReceiveDamageAnimation(DamageReceiveLocation, ReceivingDamageType, true);
}

void AEnemyBase::OnEnemyEnterSpecificCombatState_Implementation(CombatStatus EnteringStatus)
{
	IEnemyActionInterface::OnEnemyEnterSpecificCombatState_Implementation(EnteringStatus);

	if(IsInRewind) return;
	
	if(EnteringStatus == CombatStatus::Idle)
	{
		CurrentCombatStatus = CombatStatus::Idle;
		FinishBehaviorTask();
	}

	if(EnteringStatus == CombatStatus::BeforeRecovering)
	{
		OnNormalAttackContinue();
	}

	if(EnteringStatus == CombatStatus::Parry)
	{
		CurrentCombatStatus = EnteringStatus;

		const UWorld* World = GetWorld();
		if(World == nullptr) return;
		
		World->GetTimerManager().SetTimer(ContinueParryCheckTimerHandle,this, &AEnemyBase::ParryContinueCheck, 0.2f, true, -1);
	}

}

void AEnemyBase::OnUpdateRotationToTarget_Implementation()
{
	Super::OnUpdateRotationToTarget_Implementation();

	if(!PlayerTargetRef) return;

	const FVector PlayerLocation = PlayerTargetRef->GetActorLocation();
	const FVector SelfLocation = GetActorLocation();

	const FVector DirToPlayer = UKismetMathLibrary::Normal((PlayerLocation - SelfLocation));
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(DirToPlayer));
}

void AEnemyBase::PlayChargeAttackDamageAnim_Implementation()
{
	IEnemyActionInterface::PlayChargeAttackDamageAnim_Implementation();

	
	
	const bool CacheResult = TryCachePlayerTargetRef();
	if(!CacheResult)
	{
		FinishBehaviorTask();
		return;
	}
	
	const FTransform MotionWarpUpdateTransform = GetMoveToTargetTransform(PlayerTargetRef, GetActorLocation(), 650);
	UpdateMotionWarpingDestination_HorizontalAttack(MotionWarpUpdateTransform);
	
	PlayAnimMontage(ChargeAttackMontage, 1, "Default");
}

void AEnemyBase::OnGameClearWhenEnemyDie_Implementation()
{
	IEnemyActionInterface::OnGameClearWhenEnemyDie_Implementation();

	OnGameClearDelegate.ExecuteIfBound();
}

void AEnemyBase::OnFinishHealItemAnim_Implementation()
{
	Super::OnFinishHealItemAnim_Implementation();

	if(IsInRewind) return;
	
	OnEnemyEnterSpecificCombatState_Implementation(CombatStatus::Idle);
}

