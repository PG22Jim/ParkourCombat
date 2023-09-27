// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Components/BasePlayerComponent.h"

// Sets default values for this component's properties
UBasePlayerComponent::UBasePlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBasePlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBasePlayerComponent::TryGetOwnerReference()
{
	// if successfully cast owner actor to AParkourCombatCharacter, store it as component's owner
	AActor* OwnerActor = GetOwner();
	if(OwnerActor == nullptr) return;
	
	AParkourCombatCharacter* CastedOwnerActor = Cast<AParkourCombatCharacter>(OwnerActor);
	if(CastedOwnerActor == nullptr) return;

	PlayerOwnerRef = CastedOwnerActor;
}

void UBasePlayerComponent::TryGetCapsuleComponent()
{
	// try cache owner if reference is null, and return it as result is false
	if(!PlayerOwnerRef) TryGetOwnerReference();
	if(!PlayerOwnerRef) return;

	// try cache capsule component from owner
	UCapsuleComponent* PlayerCapsule = PlayerOwnerRef->GetCapsuleComponent();
	if(!PlayerCapsule) return;
	PlayerCapsuleCompRef = PlayerCapsule;
}

void UBasePlayerComponent::TryGetCharacterMovementComponent()
{
	// try cache owner if reference is null, and return it as result is false
	if(!PlayerOwnerRef) TryGetOwnerReference();
	if(!PlayerOwnerRef) return;

	// try cache movement component from owner
	UCharacterMovementComponent* PlayerChaMovement = PlayerOwnerRef->GetCharacterMovement();
	if(!PlayerChaMovement) return;
	PlayerChaMoveCompRef = PlayerChaMovement;
}

void UBasePlayerComponent::InitializeDelegate()
{
	// empty function
}


// Called every frame
void UBasePlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

