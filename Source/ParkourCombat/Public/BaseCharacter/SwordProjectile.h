// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SwordProjectile.generated.h"


class ASwordProjectile;

struct FProjectileFrameData
{
	
public:
	//TArray<bool> ProjectileFrameIsActive;
	//TArray<FTransform> ProjectileFrameTransforms;

	bool ProjectileFrameIsActive;
	FTransform ProjectileFrameTransforms;
	FVector ProjectileFrameVelocity;
	AActor* ProjectileFrameOwner;
	ASwordProjectile* ProjectileRef;
	

	FProjectileFrameData();
	FProjectileFrameData(ASwordProjectile* Ref, bool bActive, FTransform FrameTransform, FVector CurrentVelocity, AActor* Owner)
	{
		ProjectileRef = Ref;
		ProjectileFrameIsActive = bActive;
		ProjectileFrameTransforms = FrameTransform;
		ProjectileFrameVelocity = CurrentVelocity;
		ProjectileFrameOwner = Owner;
	}
};


UCLASS()
class PARKOURCOMBAT_API ASwordProjectile : public AActor
{
	GENERATED_BODY()

	bool IsActivated = false;

	FVector PoolPosition = FVector(0,0,0);

	void MoveProjectile();
	void StopProjectile();

	AActor* ProjectileOwner;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
public:	
	// Sets default values for this actor's properties
	ASwordProjectile();
	
	FProjectileFrameData ExtractProjectileFrameData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileInitialSpeed = 1000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ProjectileMaxSpeed = 1000.0f;
	
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnFreezeProjectile() {CustomTimeDilation = 0.0f;}
	void UnFreezeProjectile();
	
	bool GetIsActivated() const {return IsActivated;}
	void SetIsActivated(bool bActivate) {IsActivated = bActivate;}
	
	void DeactivateProjectile();
	void ActivateProjectile(FVector SpawnLocation, FRotator SpawnRotation);
	
	void SetPoolPosition(FVector NewPos) {PoolPosition = NewPos;}
	
	AActor* GetProjectileOwner() const {return ProjectileOwner;}
	void SetProjectileOwner(AActor* NewOwner) {ProjectileOwner = NewOwner;}


	UFUNCTION(BlueprintImplementableEvent)
	void PauseNiagara();

	UFUNCTION(BlueprintImplementableEvent)
	void UnPauseNiagara();
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartNiagara();
	
	
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
