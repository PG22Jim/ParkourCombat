// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialPageTriggerActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnlockTutorialPage, int32, UnlockPageNum);

UCLASS(Blueprintable, BlueprintType)
class PARKOURCOMBAT_API ATutorialPageTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialPageTriggerActor();

protected:


	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UnlockPageCount = 1;
	
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	FOnUnlockTutorialPage OnUnlockTutorialPageDelegate;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
