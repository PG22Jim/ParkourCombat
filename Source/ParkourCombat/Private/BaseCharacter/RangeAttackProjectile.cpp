// Copyright © 2023 by Jim Chen. All rights reserved.


#include "BaseCharacter/RangeAttackProjectile.h"

#include "BaseCharacter/BaseDamageReceiveInterface.h"
#include "Components/CapsuleComponent.h"

void ARangeAttackProjectile::OnHitTrigger()
{
	DeactivateProjectile();
	//DestroyNiagara();
	//Destroy();
}

// Sets default values
ARangeAttackProjectile::ARangeAttackProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collision"));
	if(CapsuleCollision)
	{
		CapsuleCollision->SetupAttachment(GetRootComponent());
		CapsuleComp = CapsuleCollision;
	}

	CapsuleCollision->SetupAttachment(GetRootComponent());
	

	UProjectileMovementComponent* ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	if(ProjectileMovement)
	{
		ProjectileMovementComp = ProjectileMovement;
	}

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement->UpdatedComponent = CapsuleComp;
	ProjectileMovement->InitialSpeed = 300.f;
	ProjectileMovement->MaxSpeed = 300.f;

	//// Die after 3 seconds by default
	//InitialLifeSpan = 0;
	
}

void ARangeAttackProjectile::OnProjectileBeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	const FHitResult& Hit)
{
	if(!IsActivated) return;
	
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

	OnHitTrigger();
}

// Called when the game starts or when spawned
void ARangeAttackProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if(CapsuleComp)
		CapsuleComp->OnComponentHit.AddDynamic(this, &ARangeAttackProjectile::OnProjectileBeginHit);

	DeactivateProjectile();
	
}

void ARangeAttackProjectile::DeactivateProjectile()
{
	IsActivated = false;

	ProjectileOwner = nullptr;
	SetActorLocation(PoolPosition);
	ProjectileMovementComp->InitialSpeed = 0;
	ProjectileMovementComp->MaxSpeed = 0;
}

void ARangeAttackProjectile::ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation)
{
	IsActivated = true;
	SetActorLocation(SpawnLocation);
	SetActorRotation(SpawnRotation);

	ProjectileMovementComp->InitialSpeed = ProjectileInitialSpeed;
	ProjectileMovementComp->MaxSpeed = ProjectileMaxSpeed;
}

void ARangeAttackProjectile::OnProjectileFakeSpawn_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	IProjectileInterface::OnProjectileFakeSpawn_Implementation(SpawnLocation, SpawnRotation);
}

// Called every frame
void ARangeAttackProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

