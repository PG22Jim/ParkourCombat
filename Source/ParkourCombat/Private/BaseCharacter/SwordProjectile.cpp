// Copyright © 2023 by Jim Chen. All rights reserved.


#include "BaseCharacter/SwordProjectile.h"

#include "BaseCharacter/BaseDamageReceiveInterface.h"

void ASwordProjectile::MoveProjectile()
{
	StartNiagara();
	ProjectileMovement->SetUpdatedComponent(CollisionComp);
	const FVector NewVelocity = GetActorForwardVector() * ProjectileInitialSpeed;
	ProjectileMovement->Velocity = NewVelocity;
}

void ASwordProjectile::StopProjectile()
{
	PauseNiagara();
	ProjectileMovement->Velocity = FVector(0,0,0);
}

// Sets default values
ASwordProjectile::ASwordProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASwordProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	
}

FProjectileFrameData ASwordProjectile::ExtractProjectileFrameData()
{
	return FProjectileFrameData(this, IsActivated, GetActorTransform(), ProjectileMovement->Velocity, ProjectileOwner);
}

void ASwordProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             FVector NormalImpulse, const FHitResult& Hit)
{
	//if(!IsActivated) return;
	
	if(OtherActor == ProjectileOwner) return;
	
	if ((OtherActor != nullptr))
	{
		if(OtherActor->GetClass()->ImplementsInterface(UBaseDamageReceiveInterface::StaticClass()))
		{
			FVector BeginOverLapLocation = GetActorLocation();
			BeginOverLapLocation.Z = OtherActor->GetActorLocation().Z;
			
			// if it has character action interface, it means its base character, execute its SwitchToIdleState function
			IBaseDamageReceiveInterface::Execute_ReceiveDamage(OtherActor, ProjectileOwner, 1, BeginOverLapLocation, CharacterDamageType::LightDamage);
		}
	}
	
	DeactivateProjectile();
}

void ASwordProjectile::DeactivateProjectile()
{
	IsActivated = false;

	ProjectileOwner = nullptr;
	SetActorLocation(PoolPosition);
	StopProjectile();
}

void ASwordProjectile::ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation)
{
	IsActivated = true;
	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);

	MoveProjectile();
}

// Called when the game starts or when spawned
void ASwordProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASwordProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

