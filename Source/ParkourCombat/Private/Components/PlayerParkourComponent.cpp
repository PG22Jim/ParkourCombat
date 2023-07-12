// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Components/PlayerParkourComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


void UPlayerParkourComponent::EarlyFinishParkourAction()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Finish Parkour Action!"));
	
	FinishParkourAction();

	PlayerOwnerRef->StopAnimMontage(CurrentMontage);
}

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
	TryGetCapsuleComponent();
	TryGetCharacterMovementComponent();
	
	InitializeDelegate();

	ParkourTickStart();
}

void UPlayerParkourComponent::InitializeDelegate()
{
	Super::InitializeDelegate();

	if(!PlayerOwnerRef) TryGetOwnerReference();
	
	PlayerOwnerRef->OnExecuteFinishParkourAction.BindDynamic(this, &UPlayerParkourComponent::FinishParkourAction);
	PlayerOwnerRef->OnJumpExecuteDelegate.BindDynamic(this, &UPlayerParkourComponent::OnJumpExecution);
}


void UPlayerParkourComponent::TryGetOwnerReference()
{
	Super::TryGetOwnerReference();
}

void UPlayerParkourComponent::ParkourTickStart()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	World->GetTimerManager().SetTimer(ParkourTickTimerHandle, this, &UPlayerParkourComponent::ParkourTickCheck, ParkourTick, true, -1);
}

bool UPlayerParkourComponent::IsPlayerDoingParkourAction()
{
	if(!PlayerOwnerRef) return false;

	const ParkourStatus CurrentParkourStatus = PlayerOwnerRef->GetCurrentParkourStatus();
	return CurrentParkourStatus == ParkourStatus::SlideThrough || CurrentParkourStatus == ParkourStatus::QuickWallClimb || CurrentParkourStatus == ParkourStatus::VaultThroughSmallWall;
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
	// if player is not running and player is not performing parkour action, enter Idle state
	if(!IsRunning() && !IsPlayerDoingParkourAction())
	{
		EnterIdleState();
	}

	if(!PlayerOwnerRef) return;

	if(IsExecutingCombat()) return;
	
	const ParkourStatus CurrentParkourStatus = PlayerOwnerRef->GetCurrentParkourStatus();

	
	// if player is in idle state but has velocity, enter running state
	if(CurrentParkourStatus == ParkourStatus::Idle)
	{
		EnterRunningState();
		return;
	}

	// if player is in running state, run logic in running state
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
	{
		ClearParkourList();
		TryToGetPositionsToVault();
		BeginVaulting();
	}
	
	if(SlidingCheck())
		BeginSlide();

	if(WallClimbCheck())
		BeginWallClimb();
		
		
	
	
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
	float HemisphereCapRadius = 0.0f;
	HemisphereCapRadius += 15;
	
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
	
	//PlayerOwnerRef->OnUpdateDestination_Implementation();
	
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
	const FVector CheckBlockingEnd = CheckBlockingStart + (CurrentForwardDir * 600.0f);

	// Sphere trace by channel from Kismet System Library
	const bool bIsBlocked = UKismetSystemLibrary::SphereTraceSingle(this, CheckBlockingStart, CheckBlockingEnd, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bIsBlocked) return false;


	// THIRD CHECK: Has space to get up
	const float OffsetToGetUp = PlayerCapsuleHalfHeight * 2;
	const FVector GetUpEndPos = FVector(CheckBlockingEnd.X, CheckBlockingEnd.Y, (CheckBlockingEnd.Z + OffsetToGetUp));
	const bool bUnableToGetUp = UKismetSystemLibrary::SphereTraceSingle(this, CheckBlockingEnd, GetUpEndPos, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bUnableToGetUp) return false;


	// All of them are true, which means player can perform slide, store slide destinations for later usage
	FRotator CurrentRotation = PlayerOwnerRef->GetActorRotation();
	const FTransform FirstDestTrans = UKismetMathLibrary::MakeTransform(CheckBlockingStart, CurrentRotation);
	const FTransform SecondDestTrans = UKismetMathLibrary::MakeTransform(CheckBlockingEnd, CurrentRotation);

	PlayerOwnerRef->UpdateMotionWarpingDestination_Sliding(FirstDestTrans, SecondDestTrans);
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

// =================================================== Quick Wall Climb =========================================

bool UPlayerParkourComponent::WallClimbCheck()
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
	const float OffsetToBlockCheck = PlayerCapsuleHalfHeight - 30;
	const FVector InRangeStartPos = FVector(CurrentPlayerPos.X, CurrentPlayerPos.Y, (CurrentPlayerPos.Z - OffsetToBlockCheck));
	const FVector InRangeEndPos = InRangeStartPos + (CurrentForwardDir * 325.0f);

	// Sphere trace by channel from Kismet System Library
	const bool bIsInRange = UKismetSystemLibrary::SphereTraceSingle(this, InRangeStartPos, InRangeEndPos, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(!bIsInRange) return false;
	
	// SECOND CHECK: has enough space for player to run before wall climbing
	const FVector InRangeHitPos = Hit.Location;
	FVector InRangeHitPosWithSameZ = InRangeHitPos;
	InRangeHitPosWithSameZ.Z = CurrentPlayerPos.Z;
	const float DistanceToWall = UKismetMathLibrary::Vector_Distance(CurrentPlayerPos, InRangeHitPosWithSameZ);
	const bool IsFarEnough = DistanceToWall > 310.0f;

	if(!IsFarEnough) return false;
	
	
	// THIRD CHECK: has enough space to climb
	const FVector HasSpaceToClimbStart = Hit.Location - (CurrentForwardDir * 30.0f);
	const FVector HasSpaceToClimbEnd = FVector(HasSpaceToClimbStart.X, HasSpaceToClimbStart.Y, (HasSpaceToClimbStart.Z + 250.0f));

	const bool bNoSpaceToClimb = UKismetSystemLibrary::SphereTraceSingle(this, HasSpaceToClimbStart, HasSpaceToClimbEnd, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bNoSpaceToClimb) return false;

	
	// FOURTH CHECK: has enough space to land
	const FVector HasSpaceToLandEnd = HasSpaceToClimbEnd + (CurrentForwardDir * 100.0f);
	const bool bNoSpaceToLand = UKismetSystemLibrary::SphereTraceSingle(this, HasSpaceToClimbEnd, HasSpaceToLandEnd, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(bNoSpaceToLand) return false;

	// FIFTH CHECK: has ground to land
	const FVector IsValidLandingEnd = HasSpaceToLandEnd - FVector(0,0,10);
	const bool bIsValidLanding = UKismetSystemLibrary::SphereTraceSingle(this, HasSpaceToLandEnd, IsValidLandingEnd, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
	if(!bIsValidLanding) return false;



	// All conditions valid, start store parkour moving positions
	const FRotator CurrentPlayerRotation = PlayerOwnerRef->GetActorRotation();
	const FTransform FirstDestTrans = UKismetMathLibrary::MakeTransform(InRangeHitPos, CurrentPlayerRotation);

	const FVector ExpectPosToTouchWallStart = UKismetMathLibrary::VLerp(HasSpaceToClimbStart, HasSpaceToClimbEnd, 0.4);
	const FVector ExpectPosToTouchWallEnd = ExpectPosToTouchWallStart + (CurrentForwardDir * 100.0f);
	
	FVector PosToTouchWall = ExpectPosToTouchWallEnd;
	if(	UKismetSystemLibrary::SphereTraceSingle(this, ExpectPosToTouchWallStart, ExpectPosToTouchWallEnd, 25.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true))
		PosToTouchWall = Hit.Location;

	const FTransform SecondDestTrans = UKismetMathLibrary::MakeTransform(PosToTouchWall, CurrentPlayerRotation);


	const FVector PlaceToGrabEdge = UKismetMathLibrary::VLerp(HasSpaceToClimbStart, HasSpaceToClimbEnd, 0.7);
	const FTransform ThirdDestTrans = UKismetMathLibrary::MakeTransform(PlaceToGrabEdge, CurrentPlayerRotation);

	const FVector PlaceToBeginRun = FVector(HasSpaceToLandEnd.X, HasSpaceToLandEnd.Y, PlaceToGrabEdge.Z);
	const FTransform FourthDestTrans = UKismetMathLibrary::MakeTransform(PlaceToBeginRun, CurrentPlayerRotation);

	PlayerOwnerRef->UpdateMotionWarpingDestination_Climbing(FirstDestTrans, SecondDestTrans, ThirdDestTrans,FourthDestTrans);
	
	return true;
}

void UPlayerParkourComponent::BeginWallClimb()
{
	CurrentMontage = WallClimbToSprint;
	if(!CurrentMontage) return;
	
	EnterWallClimbState();

	BeginParkourAction();

	PlayerOwnerRef->UpdateJumpClimbStartZ();
	
	PlayerOwnerRef->PlayAnimMontage(CurrentMontage,1.2,"Default");
}

void UPlayerParkourComponent::EnterWallClimbState()
{
	SetNewParkourState(ParkourStatus::QuickWallClimb);
}

bool UPlayerParkourComponent::IsWallClimbing()
{
	return (PlayerOwnerRef->InRequestParkourState(ParkourStatus::QuickWallClimb) && CurrentMontage == WallClimbToSprint);
}


// ============================================= Jump Execution =============================================

void UPlayerParkourComponent::OnJumpExecution()
{
	if(!IsWallClimbing()) return;

	// PlayerOwnerRef->StopAnimMontage(CurrentMontage);
	// //EarlyFinishParkourAction();
	//
	// // TODO: Check if enemy in range
	// if(UWorld* World = GetWorld())
	// {
	// 	TArray<AActor*> FoundActorList;
	// 	TArray<AActor*> IgnoreActors;
	// 	IgnoreActors.Add(PlayerOwnerRef);
	// 	UKismetSystemLibrary::SphereOverlapActors(World, PlayerOwnerRef->GetActorLocation(), DetectingRadius, FilterType, FilteringClass, IgnoreActors, FoundActorList);
	// 	TestFunction(PlayerOwnerRef->GetActorLocation(), DetectingRadius);
	// 	
	// 	// TODO: Jump to closest enemy
	//
	// 	
	//
	//
	// 	// TODO: Perform small wall jump
	// 	CurrentMontage = JumpFromWall;
	// 	if(!CurrentMontage) return;
	//
	// 	PlayerOwnerRef->PlayAnimMontage(CurrentMontage);
	// 	
	// }
	
	
	
}

// ============================================= Utility =============================================
bool UPlayerParkourComponent::IsRunning()
{
	if(!PlayerChaMoveCompRef) return false;

	const FVector LastVelocity = PlayerChaMoveCompRef->GetLastUpdateVelocity();
	const float VelocityLength = LastVelocity.Size2D();

	return VelocityLength > 0;
	
}

bool UPlayerParkourComponent::IsExecutingCombat()
{
	const CombatStatus Status = PlayerOwnerRef->GetCurrentCombatStatus();
	
	return (Status != CombatStatus::Idle);
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

void UPlayerParkourComponent::ClearParkourList()
{
	ParkourMovingDestinations.Empty();
}

void UPlayerParkourComponent::StoreFoundDestinations(TArray<FVector> FoundDestArray)
{
	if(!PlayerOwnerRef) return;
	
	// Erase All Stored Parkour destination on the list
	PlayerOwnerRef->StoredDestinationList->ClearList();

	
	
	const FRotator CurrentPlayerRotation = PlayerOwnerRef->GetActorRotation();

	for (FVector EachDest : FoundDestArray)
	{
		FTransform EachDestTransform = UKismetMathLibrary::MakeTransform(EachDest, CurrentPlayerRotation);
		PlayerOwnerRef->StoredDestinationList->AddAtHead(EachDestTransform);
	}
}

void UPlayerParkourComponent::TryToGetPositionsToVault()
{
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);

	// 1ST POSITION: Get Hit point from first running end
	const FVector CurrentForwardDir = PlayerOwnerRef->GetActorForwardVector();
	const FVector PlayerCurrentPos = PlayerOwnerRef->GetActorLocation();
	const FVector InRangeEndPos = PlayerCurrentPos + (CurrentForwardDir * 200.0f);

	// Sphere trace by channel from Kismet System Library
	const bool bIsInRange = UKismetSystemLibrary::SphereTraceSingle(this, PlayerCurrentPos, InRangeEndPos, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);

	if(bIsInRange)
	{
		FVector FirstDestination = Hit.Location;
		FirstDestination -= (CurrentForwardDir * 20);
		
		
		// 2ND POSITION: Get position for player to place hand to vault

		const float HemisphereCapRadius = 15.0f;

		
		// Get start and end positions
		FVector PlaceHandPosEnd = FirstDestination + (CurrentForwardDir * 40);
		const FVector PlayerUPVector = PlayerOwnerRef->GetActorUpVector();
		
		FVector PlaceHandPosStart = PlaceHandPosEnd + (PlayerUPVector * VaultHeight);
		
		
		// Sphere trace by channel from Kismet System Library
		const bool bPlaceHandPos = UKismetSystemLibrary::SphereTraceSingle(this, PlaceHandPosStart, PlaceHandPosEnd, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
		if(bPlaceHandPos)
		{
			const FVector SecondDestination = Hit.Location;

			
			// 4TH POSITION: Position to land
			
			const FVector PlaceToLandStart = FirstDestination + (CurrentForwardDir * MaxVaultDistance);
			

		
			const bool bPlaceToLandHorizontal = UKismetSystemLibrary::SphereTraceSingle(this, PlaceToLandStart, FirstDestination, 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true);
			if(bPlaceToLandHorizontal)
			{
				FVector FourthDestination = Hit.Location + (CurrentForwardDir * GapBetweenLandPosAndWall);


				// 3RD POSITION: Position in mid air
				
				FVector ThirdDestination = UKismetMathLibrary::VLerp(SecondDestination, FourthDestination,0.5f);
				ThirdDestination.Z = SecondDestination.Z + (VaultHeight / 6);

				FirstDestination.Z = PlayerCurrentPos.Z;

				if(UKismetSystemLibrary::SphereTraceSingle(this, FourthDestination, FourthDestination - (PlayerOwnerRef->GetActorUpVector() * 200), 10.0f, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, IgnoreActors, EDrawDebugTrace::None, Hit, true))
					FourthDestination.Z = Hit.Location.Z;
				else FourthDestination.Z = PlayerCurrentPos.Z - (PlayerOwnerRef->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2);
				
				FRotator CurrentPlayerRotation = PlayerOwnerRef->GetActorRotation();
				FTransform FirstTransform = UKismetMathLibrary::MakeTransform(FirstDestination, CurrentPlayerRotation);
				FTransform SecondTransform = UKismetMathLibrary::MakeTransform(SecondDestination, CurrentPlayerRotation);
				FTransform ThirdTransform = UKismetMathLibrary::MakeTransform(ThirdDestination, CurrentPlayerRotation);
				FTransform FinalTransform = UKismetMathLibrary::MakeTransform(FourthDestination, CurrentPlayerRotation);

				PlayerOwnerRef->UpdateMotionWarpingDestination_Vaulting(FirstTransform, SecondTransform, ThirdTransform, FinalTransform);
			}
		}
	}


	
}


