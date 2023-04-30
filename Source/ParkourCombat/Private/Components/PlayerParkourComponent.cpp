// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Components/PlayerParkourComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UPlayerParkourComponent::UPlayerParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPlayerParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	TryGetOwnerReference();

	ParkourTickStart();
}

void UPlayerParkourComponent::TryGetOwnerReference()
{
	AActor* OwnerActor = GetOwner();
	if(OwnerActor == nullptr) return;
	
	AParkourCombatCharacter* CastedOwnerActor = Cast<AParkourCombatCharacter>(OwnerActor);
	if(CastedOwnerActor == nullptr) return;

	PlayerOwnerRef = CastedOwnerActor;

	UCapsuleComponent* PlayerCapsule = PlayerOwnerRef->GetCapsuleComponent();
	if(!PlayerCapsule) return;

	PlayerCapsuleCompRef = PlayerCapsule;
	
	UCharacterMovementComponent* PlayerChaMovement = PlayerOwnerRef->GetCharacterMovement();
	if(!PlayerChaMovement) return;

	PlayerChaMoveCompRef = PlayerChaMovement;

	InitializeDelegate();
	
}

void UPlayerParkourComponent::InitializeDelegate()
{
	PlayerOwnerRef->OnExecuteFinishParkourAction.BindDynamic(this, &UPlayerParkourComponent::FinishParkourAction);
}

void UPlayerParkourComponent::ParkourTickStart()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	World->GetTimerManager().SetTimer(ParkourTickTimerHandle, this, &UPlayerParkourComponent::ParkourTickCheck, ParkourTick, true, -1);
}

// Called every frame
void UPlayerParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



// ============================================================ Parkour ============================================================

void UPlayerParkourComponent::ParkourTickCheck()
{
	const bool PlayerRunning = IsRunning();
	
	if(!PlayerRunning)
	{
		EnterIdleState();
		return;
	}

	if(CurrentParkourStatus == ParkourStatus::Idle)
	{
		EnterRunningState();
		return;
	}

	if(CurrentParkourStatus == ParkourStatus::Running)
	{
		InRunningState();
	}

	
}

// ============================================= Running =============================================
void UPlayerParkourComponent::EnterRunningState()
{
	SetNewParkourState(ParkourStatus::Running);
}
	
void UPlayerParkourComponent::InRunningState()
{
	if(VaultingCheck())
		BeginVaulting();

	if(SlidingCheck())
		BeginSlide();
		
		
	
	
}

bool UPlayerParkourComponent::VaultingCheck() const
{
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);

	// FIRST CHECK: Is player in range to perform vaulting action
	// Get start and end positions
	const FVector CurrentForwardDir = PlayerOwnerRef->GetActorForwardVector();
	const FVector PlayerCurrentPos = PlayerOwnerRef->GetActorLocation();
	const FVector InRangeEndPos = PlayerCurrentPos + (CurrentForwardDir * 200.0f);

	// Sphere trace by channel from Kismet System Library
	const bool bIsInRange = UKismetSystemLibrary::SphereTraceSingle(this, PlayerCurrentPos, InRangeEndPos, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(!bIsInRange)
		return false;
	

	// SECOND CHECK: has enough space for player to run before vaulting
	const FVector InRangeHitPos = Hit.Location;
	FVector InRangeHitPosWithSameZ = InRangeHitPos;
	InRangeHitPosWithSameZ.Z = PlayerCurrentPos.Z;
	const float DistanceToWall = UKismetMathLibrary::Vector_Distance(PlayerCurrentPos, InRangeHitPosWithSameZ);
	const bool IsFarEnough = DistanceToWall > 180.0f;

	if(!IsFarEnough) return false;
	
	// THIRD CHECK: is wall small enough for vaulting

	const float VaultingWallHeight = 100.0f;
	const float HemisphereCapRadius = 15.0f;

	
	// Get start and end positions
	FVector EnoughSpaceStartPos = InRangeHitPos + (CurrentForwardDir * 30);
	EnoughSpaceStartPos.Z += (VaultingWallHeight + HemisphereCapRadius);

	const FVector PlayerDownVector = PlayerOwnerRef->GetActorUpVector() * -1;
	
	const float PlayerGroundZ = (PlayerCurrentPos + (PlayerDownVector * PlayerCapsuleCompRef->GetScaledCapsuleHalfHeight())).Z;
	const FVector EnoughSpaceEndPos = FVector(EnoughSpaceStartPos.X, EnoughSpaceStartPos.Y, (PlayerGroundZ + VaultingWallHeight + HemisphereCapRadius));
	
	// Sphere trace by channel from Kismet System Library
	const bool bNoSpaceToVault = UKismetSystemLibrary::SphereTraceSingle(this, EnoughSpaceStartPos, EnoughSpaceEndPos, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(bNoSpaceToVault)
		return false;
	

	// FOURTH CHECK: has enough space to land

	FVector AbleLandStartPos = EnoughSpaceStartPos + (CurrentForwardDir * 75);
	AbleLandStartPos.Z = PlayerGroundZ + HemisphereCapRadius;
	
	const FVector AbleLandEndPos = AbleLandStartPos + (CurrentForwardDir * 200);
	
	const bool bObjectBlocking = UKismetSystemLibrary::SphereTraceSingle(this, AbleLandStartPos, AbleLandEndPos, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(bObjectBlocking)
		return false;
	
	// Return True if all checks are valid
	return true;
}

// ============================================= Idle =============================================
void UPlayerParkourComponent::EnterIdleState()
{
	SetNewParkourState(ParkourStatus::Idle);
}

// ============================================= Vaulting =============================================

void UPlayerParkourComponent::BeginVaulting()
{
	CurrentMontage = StylishJumpThroughSmallWall;
	if(!CurrentMontage) return;
	
	EnterVaultingState();

	BeginParkourAction();

	PlayerOwnerRef->PlayAnimMontage(CurrentMontage,1,"Default");

}

void UPlayerParkourComponent::EnterVaultingState()
{
	SetNewParkourState(ParkourStatus::VaultThroughSmallWall);
}

// ============================================= Slide =============================================
bool UPlayerParkourComponent::SlidingCheck()
{
	const FVector CurrentPlayerPos = PlayerOwnerRef->GetActorLocation();
	const FVector CurrentForwardDir = PlayerOwnerRef->GetActorForwardVector();
	const float PlayerCapsuleHalfHeight = PlayerCapsuleCompRef->GetScaledCapsuleHalfHeight();

	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);


	// FIRST CHECK: is close enough to trigger sliding
	// Get start and end positions
	const FVector InRangeStartPos = FVector(CurrentPlayerPos.X, CurrentPlayerPos.Y, (CurrentPlayerPos.Z + PlayerCapsuleHalfHeight));
	const FVector InRangeEndPos = InRangeStartPos + (CurrentForwardDir * 220.0f);

	// Sphere trace by channel from Kismet System Library
	const bool bIsInRange = UKismetSystemLibrary::SphereTraceSingle(this, InRangeStartPos, InRangeEndPos, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(!bIsInRange) return false;

	// SECOND CHECK: no object blocking player's slide
	const float OffsetToBlockCheck = PlayerCapsuleHalfHeight - 30;
	const FVector CheckBlockingStart = FVector(CurrentPlayerPos.X, CurrentPlayerPos.Y, (CurrentPlayerPos.Z - OffsetToBlockCheck));
	const FVector CheckBlockingEnd = CheckBlockingStart + (CurrentForwardDir * 550.0f);

	// Sphere trace by channel from Kismet System Library
	const bool bIsBlocked = UKismetSystemLibrary::SphereTraceSingle(this, CheckBlockingStart, CheckBlockingEnd, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bIsBlocked) return false;


	// THIRD CHECK: Has space to get up
	const float OffsetToGetUp = PlayerCapsuleHalfHeight * 2;
	const FVector GetUpEndPos = FVector(CheckBlockingEnd.X, CheckBlockingEnd.Y, (CheckBlockingEnd.Z + OffsetToGetUp));
	const bool bUnableToGetUp = UKismetSystemLibrary::SphereTraceSingle(this, CheckBlockingEnd, GetUpEndPos, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bUnableToGetUp) return false;
	
	
	return true;
}

void UPlayerParkourComponent::BeginSlide()
{
	CurrentMontage = StylishSlideThrough;
	if(!CurrentMontage) return;
	
	EnterSlideState();

	BeginParkourAction();

	PlayerOwnerRef->PlayAnimMontage(CurrentMontage,1.2,"Default");
}


void UPlayerParkourComponent::EnterSlideState()
{
	SetNewParkourState(ParkourStatus::SlideThrough);
}

// ============================================= Utility =============================================
bool UPlayerParkourComponent::IsRunning()
{
	if(!PlayerOwnerRef) return false;

	UCharacterMovementComponent* PlayerChaMovement = PlayerOwnerRef->GetCharacterMovement();

	const FVector LastVelocity = PlayerChaMovement->GetLastUpdateVelocity();
	const float VelocityLength = LastVelocity.Size2D();

	return VelocityLength > 0;
	
}

void UPlayerParkourComponent::BeginParkourAction()
{
	if(!PlayerCapsuleCompRef || !PlayerChaMoveCompRef) return;
	
	PlayerCapsuleCompRef->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayerChaMoveCompRef->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UPlayerParkourComponent::FinishParkourAction()
{
	if(!PlayerCapsuleCompRef || !PlayerChaMoveCompRef) return;
	
	PlayerCapsuleCompRef->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlayerChaMoveCompRef->SetMovementMode(EMovementMode::MOVE_Walking);

	SetNewParkourState(ParkourStatus::Idle);
}


