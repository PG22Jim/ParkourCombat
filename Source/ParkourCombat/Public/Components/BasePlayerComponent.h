// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "BasePlayerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PARKOURCOMBAT_API UBasePlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBasePlayerComponent();

protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AParkourCombatCharacter* PlayerOwnerRef;

	UCapsuleComponent* PlayerCapsuleCompRef;
	
	UCharacterMovementComponent* PlayerChaMoveCompRef;

	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TryGetOwnerReference();

	void TryGetCapsuleComponent();
	void TryGetCharacterMovementComponent();

	virtual void InitializeDelegate();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
