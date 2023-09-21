// Copyright © 2023 by Jim Chen. All rights reserved.


#include "BaseCharacter/BaseWeapon.h"

#include "BaseCharacter/BaseCharacterInterface.h"
#include "BaseCharacter/BaseDamageReceiveInterface.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	if(Mesh)
	{
		Mesh->SetupAttachment(GetRootComponent());
		WeaponMesh = Mesh;
	}
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(WeaponMesh)
		WeaponMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnWeaponBeginOverlap);

	EnableOrDisableWeaponMeshCollision(false);
}

void ABaseWeapon::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!WeaponOwner || IsInRewind) return;
	
	// check if owner class has character action interface
	if(OtherActor != WeaponOwner && OtherActor)
		if(OtherActor->GetClass()->ImplementsInterface(UBaseDamageReceiveInterface::StaticClass()))
		{
			FVector BeginOverLapLocation = WeaponOwner->GetActorLocation();
			BeginOverLapLocation.Z = OtherActor->GetActorLocation().Z;

			if(CurrentDamageType == CharacterDamageType::FlyingDamage)
			{
				// if it has character action interface, it means its base character, execute its SwitchToIdleState function
				IBaseDamageReceiveInterface::Execute_ReceiveDamage(OtherActor, WeaponOwner, BaseRageDamage, BeginOverLapLocation, CurrentDamageType);
				EnableOrDisableWeaponMeshCollision(false);
				IBaseCharacterInterface::Execute_OnStoreRageAttackTarget(WeaponOwner, OtherActor);
				return;
			}
			


			
			const float CurrentDamage = CurrentDamageType == CharacterDamageType::LightDamage ? BaseLightDamage : BaseHeavyDamage;
			
			// if it has character action interface, it means its base character, execute its SwitchToIdleState function
			IBaseDamageReceiveInterface::Execute_ReceiveDamage(OtherActor, WeaponOwner, CurrentDamage, BeginOverLapLocation, CurrentDamageType);

			EnableOrDisableWeaponMeshCollision(false);
		}
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

WeaponRewindData ABaseWeapon::GetCurrentWeaponRewindData()
{
	const bool IsCollisionEnable = WeaponMesh->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn) == ECollisionResponse::ECR_Overlap;

	return WeaponRewindData(CurrentDamageType, IsCollisionEnable);
}

void ABaseWeapon::EnableOrDisableWeaponMeshCollision(bool bEnableCollision)
{
	if(bEnableCollision)
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	else
		WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

