// Copyright © 2023 by Jim Chen. All rights reserved.


#include "SceneActors/TutorialEnemyBlocker.h"

#include "Components/BoxComponent.h"

// Sets default values
ATutorialEnemyBlocker::ATutorialEnemyBlocker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TutorialEnemyBlocker"));
	BoxCollision->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATutorialEnemyBlocker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATutorialEnemyBlocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

