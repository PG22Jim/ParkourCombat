// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Managers/ProjectileInterface.h"
#include "RangeAttackProjectile.generated.h"



DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDestroyProjectile, AActor*, DestroyingProjectile);
UCLASS()
class PARKOURCOMBAT_API ARangeAttackProjectile : public AActor, public IProjectileInterface
{
	GENERATED_BODY()


private:

	AActor* ProjectileOwner;

	bool IsActivated = false;

	void OnHitTrigger();

	FVector PoolPosition = FVector(0,0,0);
	
public:	
	// Sets default values for this actor's properties
	ARangeAttackProjectile();

	
	
protected:

	
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComp;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileInitialSpeed = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileMaxSpeed = 1000.0f;
	


	
	UFUNCTION(BlueprintImplementableEvent)
	void DebugSphere(FVector Vector, float Delta);
	
	
	UFUNCTION()
	void OnProjectileBeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION(BlueprintImplementableEvent)
	void DestroyNiagara();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	FOnDestroyProjectile OnDestroyProjectileDelegate;

	bool GetIsActivated() const {return IsActivated;}
	
	void DeactivateProjectile();
	void ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation);


	void SetPoolPosition(FVector NewPos) {PoolPosition = NewPos;}

	

	AActor* GetProjectileOwner() const {return ProjectileOwner;}
	void SetProjectileOwner(AActor* NewOwner) {ProjectileOwner = NewOwner;}

	virtual void OnProjectileFakeSpawn_Implementation(FVector SpawnLocation, FRotator SpawnRotation) override;



	






	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
