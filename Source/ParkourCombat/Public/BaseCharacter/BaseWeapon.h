// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnumList.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"


struct WeaponRewindData
{
protected:

	CharacterDamageType RecordDamageType;
	bool IsCollisionActivated;

public:
	// default constructor
	WeaponRewindData()
	{
		RecordDamageType = CharacterDamageType::LightDamage;
		IsCollisionActivated = false;
	}

	// constructor
	WeaponRewindData(CharacterDamageType DamageType, bool ColliActivated)
	{
		RecordDamageType = DamageType;
		IsCollisionActivated = ColliActivated;
	}

	// getter
	CharacterDamageType GetRecordDamageType() const {return RecordDamageType;}
	bool GetIsCollisionActivated() const {return IsCollisionActivated;}
	
};

UCLASS(BlueprintType)
class PARKOURCOMBAT_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	bool IsInRewind = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;

	// damage variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "DamageSetting")
	float BaseLightDamage = 10.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "DamageSetting")
	float BaseHeavyDamage = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "DamageSetting")
	float BaseRageDamage = 20.0f;
	
private:

	// owner reference
	UPROPERTY()
	AActor* WeaponOwner;

	CharacterDamageType CurrentDamageType = CharacterDamageType::LightDamage;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getter and setter
	void SetDamage(float NewLightDamage, float NewHeavyDamage, float NewRageDamage){BaseLightDamage = NewLightDamage; BaseHeavyDamage = NewHeavyDamage; BaseRageDamage = NewRageDamage;}


	WeaponRewindData GetCurrentWeaponRewindData();
	
	UStaticMeshComponent* GetWeaponMesh() const {return WeaponMesh;}

	CharacterDamageType GetDamageType() const {return CurrentDamageType;}
	void SetDamageType(CharacterDamageType NewDamageType) {CurrentDamageType = NewDamageType;}
	
	void EnableOrDisableWeaponMeshCollision(bool bEnableCollision);
	bool IsWeaponCollisionEnabled() const {return WeaponMesh->GetCollisionResponseToChannel(ECC_Pawn) == ECollisionResponse::ECR_Overlap;}

	bool GetIsInRewind() const {return IsInRewind;}
	void SetIsInRewind(bool InRewind) {IsInRewind = InRewind;}
	
	void StoreOwnerReference(AActor* StoringOwner) {WeaponOwner = StoringOwner;}
};
