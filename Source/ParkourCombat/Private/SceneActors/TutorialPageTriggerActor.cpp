// Copyright © 2023 by Jim Chen. All rights reserved.


#include "SceneActors/TutorialPageTriggerActor.h"

#include "Components/BoxComponent.h"
#include "ParkourCombat/ParkourCombatCharacter.h"

// Sets default values
ATutorialPageTriggerActor::ATutorialPageTriggerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialTriggerCollision"));
	BoxCollision->SetupAttachment(RootComponent);
}

void ATutorialPageTriggerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor) return;

	if(AParkourCombatCharacter* CastedResult = Cast<AParkourCombatCharacter>(OtherActor))
		if(OnUnlockTutorialPageDelegate.IsBound())
		{
			OnUnlockTutorialPageDelegate.Broadcast(UnlockPageCount);
			BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
}

// Called when the game starts or when spawned
void ATutorialPageTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	if(BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ATutorialPageTriggerActor::OnOverlapBegin);
	}
	
}

// Called every frame
void ATutorialPageTriggerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

