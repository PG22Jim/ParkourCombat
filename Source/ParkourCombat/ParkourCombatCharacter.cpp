// Copyright Epic Games, Inc. All Rights Reserved.

#include "ParkourCombatCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// AParkourCombatCharacter

void AParkourCombatCharacter::ResetPlayerMeshPosition()
{
	USkeletalMeshComponent* PlayerSkelMesh = GetMesh();
	UCapsuleComponent* PlayerCaps = GetCapsuleComponent();
	if(PlayerCaps && PlayerSkelMesh)
	{
		const FVector PlayerCapsWorldPos = PlayerCaps->GetComponentLocation();
		const FVector CorrectWorldPos = FVector(PlayerCapsWorldPos.X, PlayerCapsWorldPos.Y, PlayerCapsWorldPos.Z - MeshCapsuleVerticalOffset);
		PlayerSkelMesh->SetWorldLocation(CorrectWorldPos);
	}
}

void AParkourCombatCharacter::ClearRecordFunction(const FInputActionValue& Value)
{
	MovementInputVector = FVector(0,0,0);
}

AParkourCombatCharacter::AParkourCombatCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	
	
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), "spine_03");
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AParkourCombatCharacter::TryJump()
{
	OnJumpExecuteDelegate.ExecuteIfBound();
}

void AParkourCombatCharacter::TryNormalAttack()
{
	OnExecuteCombatAction.ExecuteIfBound(CombatStatus::NormalAttack);
}

void AParkourCombatCharacter::TryParry()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Try Parry!"));	
	OnExecuteCombatAction.ExecuteIfBound(CombatStatus::Parry);
}

void AParkourCombatCharacter::TryCancelParry()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Try Cancel Parry!"));	

	if(CurrentParryStatus != ParryStatus::Idle || CurrentParryStatus != ParryStatus::CancelParry) OnCancelParry.ExecuteIfBound();
}

void AParkourCombatCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	
	USkeletalMeshComponent* PlayerSkelMesh = GetMesh();
	UCapsuleComponent* PlayerCaps = GetCapsuleComponent();
	if(PlayerCaps && PlayerSkelMesh)
	{
		MeshCapsuleVerticalOffset = PlayerCaps->GetComponentLocation().Z - PlayerSkelMesh->GetComponentLocation().Z;
	}

	// Bind the event to the function
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AParkourCombatCharacter::OnResumeMeshZPosition);
	}


	// const UWorld* World = GetWorld();
	// if(!World) return;
	//
	// FTimerManager& WorldTimerManager = World->GetTimerManager();
	//
	// WorldTimerManager.SetTimer(RecordPositionTimerHandle,this, &AParkourCombatCharacter::RecordCurrentPositionToList, 0.015, true, -1);
	//
	//
}

void AParkourCombatCharacter::LinkListTest_AddRandomData()
{
	const float RandomX = UKismetMathLibrary::RandomFloatInRange(0,100);
	const float RandomY = UKismetMathLibrary::RandomFloatInRange(0,100);
	const float RandomZ = UKismetMathLibrary::RandomFloatInRange(0,100);
	const FVector RandomVector = FVector(RandomX, RandomY, RandomZ);
	const FTransform RandomTransData = FTransform(RandomVector);
	
	//CurrenBackTrackData->Push(RandomTransData);
}

void AParkourCombatCharacter::LinkListTest_PrintAll()
{
	if(CurrenBackTrackData->GetSizeNum() > 0)
	{
		ParkourPositionData* IterationData = CurrenBackTrackData->Peek();

		while (IterationData)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,IterationData->TransformData.ToString());
			IterationData = IterationData->NextTransform;
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Print Finish"));	

		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("No Data in List"));	
}

void AParkourCombatCharacter::LinkListTest_Pop()
{
	CurrenBackTrackData->Pop();
}

void AParkourCombatCharacter::LinkListTest_ClearAll()
{
	CurrenBackTrackData->ClearList();
}

void AParkourCombatCharacter::LinkListTest_DrawPosSphere()
{
	bool isBound = OnStartRewindDelegate.ExecuteIfBound();
	
	// const UWorld* World = GetWorld();
	// if(!World) return;
	//
	// FTimerManager& WorldTimerManager = World->GetTimerManager();
	//
	// WorldTimerManager.PauseTimer(RecordPositionTimerHandle);
	// WorldTimerManager.SetTimer(BackTrackingTimerHandle,this, &AParkourCombatCharacter::BackTrackTransform, 0.015, true, -1);
	// WorldTimerManager.SetTimer(StopBackTrackTimerHandle,this, &AParkourCombatCharacter::StopBackTracking, 10.0f, false, -1);
}

void AParkourCombatCharacter::RecordCurrentPositionToList()
{
	const FTransform CurrentTrans = GetActorTransform();
	//CurrenBackTrackData->Push(CurrentTrans);
}

void AParkourCombatCharacter::BackTrackTransform()
{
	ParkourPositionData* CurrentTopData = CurrenBackTrackData->Peek();
	if(!CurrentTopData) return;

	// 
	SetActorTransform(CurrentTopData->TransformData);
	CurrenBackTrackData->Pop();
	
}

void AParkourCombatCharacter::StopBackTracking()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	FTimerManager& WorldTimerManager = World->GetTimerManager();

	WorldTimerManager.UnPauseTimer(RecordPositionTimerHandle);
	WorldTimerManager.ClearTimer(BackTrackingTimerHandle);
	
}

UAnimMontage* AParkourCombatCharacter::GetCurrentAnimMontage() const
{
	// TODO: Add method to decide which montage should be playing
	return nullptr;
}

void AParkourCombatCharacter::UpdateJumpClimbStartZ()
{
	JumpClimbStartZ = GetMesh()->GetComponentLocation().Z;
}

void AParkourCombatCharacter::OnUpdateDestination_Implementation()
{
	IParkourInterface::OnUpdateDestination_Implementation();
	
	// if Current MotionWarp Destination is nullptr, it means we haven't store yet
	if(!CurrentMotionWarpDest)
	{
		ParkourPositionData* HeadPosition = StoredDestinationList->GetHead();
		if(!HeadPosition) return;
		
		CurrentMotionWarpDest = HeadPosition;
	}
	else
	{
		ParkourPositionData* NewParkourDest = CurrentMotionWarpDest->NextTransform;
		if(!NewParkourDest) return;
	
		CurrentMotionWarpDest = NewParkourDest;
	}
	
	UpdateCurrentMotionWarpingDest(CurrentMotionWarpDest->TransformData);
}

void AParkourCombatCharacter::OnParkourActionEnd_Implementation()
{
	IParkourInterface::OnParkourActionEnd_Implementation();

	const bool IsExecuted = OnExecuteFinishParkourAction.ExecuteIfBound();
}

void AParkourCombatCharacter::OnUpdateMeshPosition_Implementation()
{
	IParkourInterface::OnUpdateMeshPosition_Implementation();

	USkeletalMeshComponent* PlayerMeshComp = GetMesh();
	if(!PlayerMeshComp) return;

	const FVector MeshWorldPos = PlayerMeshComp->GetComponentLocation();
	const FVector CorrectWorldPos = FVector(MeshWorldPos.X, MeshWorldPos.Y, JumpClimbStartZ);
	PlayerMeshComp->SetWorldLocation(CorrectWorldPos);
}

void AParkourCombatCharacter::OnFinishNormalAttack_Implementation()
{
	IPlayerCombatActionInterface::OnFinishNormalAttack_Implementation();

	OnFinishCombatAction.ExecuteIfBound(CombatStatus::NormalAttack);
}

void AParkourCombatCharacter::OnEnterSpecificState_Implementation(CombatStatus State)
{
	IPlayerCombatActionInterface::OnEnterSpecificState_Implementation(State);

	CurrentCombatStatus = State;

	if(CurrentCombatStatus == CombatStatus::BeforeRecovering)
	{
		OnBufferCheck.ExecuteIfBound();
	}
}

void AParkourCombatCharacter::OnEnterSpecificParryState_Implementation(ParryStatus State)
{
	IPlayerCombatActionInterface::OnEnterSpecificParryState_Implementation(State);

	SetCurrentParryStatus(State);
}

//////////////////////////////////////////////////////////////////////////
// Input


void AParkourCombatCharacter::OnResumeMeshZPosition(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == WallClimbMontage)
	{
		ResetPlayerMeshPosition();
	}
}

void AParkourCombatCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AParkourCombatCharacter::TryJump);

		// NormalAttack
		EnhancedInputComponent->BindAction(NormalAttackAction, ETriggerEvent::Triggered, this, &AParkourCombatCharacter::TryNormalAttack);

		// Parry
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &AParkourCombatCharacter::TryParry);
		EnhancedInputComponent->BindAction(CancelParryAction, ETriggerEvent::Triggered, this, &AParkourCombatCharacter::TryCancelParry);
		
		
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AParkourCombatCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::None, this, &AParkourCombatCharacter::ClearRecordFunction);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AParkourCombatCharacter::Look);

	}

}

void AParkourCombatCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovementInputVector = FVector(MovementVector.X, MovementVector.Y, 0);
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		// const FVector SelfPos = GetActorLocation();
		// const FVector RightX = UKismetMathLibrary::GetRightVector(YawRotation);
		// const FVector ForwardY = UKismetMathLibrary::GetForwardVector(YawRotation);
		// const FVector RightInputDir = RightX * MovementVector.X;
		// const FVector ForwardInputDir = ForwardY * MovementVector.Y;
		// const FVector InputDest = SelfPos + ((RightInputDir * 50) + (ForwardInputDir * 50));
		// MovementInputDirection = UKismetMathLibrary::GetDirectionUnitVector(SelfPos, InputDest);
		
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AParkourCombatCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




