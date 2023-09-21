// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialEnemyBlocker.generated.h"

UCLASS()
class PARKOURCOMBAT_API ATutorialEnemyBlocker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialEnemyBlocker();

protected:

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxCollision;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
