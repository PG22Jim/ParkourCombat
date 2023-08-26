// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_PlayerHUD.h"

#include "Kismet/GameplayStatics.h"

bool UUW_PlayerHUD::TryCachePlayerRef()
{
	if(PlayerRef) return true;

	AParkourCombatCharacter* CastResult = Cast<AParkourCombatCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(!CastResult) return false;
	
	PlayerRef = CastResult;
	return true;
}

bool UUW_PlayerHUD::TryCacheEnemyBaseRef()
{
	if(EnemyBaseRef) return true;

	TArray<AActor*> ResultActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), BaseCharacterClass, EnemyBossTag, ResultActors);
	if(ResultActors.Num() == 0) return false;

	for(AActor* EachResultActor : ResultActors)
	{
		ABaseCharacter* CastResult = Cast<ABaseCharacter>(EachResultActor);
		if(!CastResult) continue;

		EnemyBaseRef = CastResult;
		return true;
	}
	
	return false;
}

float UUW_PlayerHUD::GetEnemyHealthPercentage()
{
	if(TryCacheEnemyBaseRef())
	{
		return EnemyBaseRef->GetCurrentHealthPercentage() / 100.0f;
	}
	
	return 0;
}

float UUW_PlayerHUD::GetPlayerHealthPercentage()
{
	if(TryCachePlayerRef())
	{
		return PlayerRef->GetCurrentHealthPercentage() / 100.0f;
	}
	
	return 0;
}

float UUW_PlayerHUD::GetPlayerStaminaPercentage()
{
	if(TryCachePlayerRef())
	{
		return PlayerRef->GetCurrentStamina() / PlayerRef->GetMaxStamina();
	}
	
	return 0;
}

float UUW_PlayerHUD::GetPlayerRagePercentage()
{
	if(TryCachePlayerRef())
	{
		return PlayerRef->GetRageGage() / PlayerRef->GetMaxRageGage();
	}
	
	return 0;
}

float UUW_PlayerHUD::GetPlayerRevengePercentage()
{
	if(TryCachePlayerRef())
	{
		return PlayerRef->GetRevengeMeter() / PlayerRef->GetMaxRevenge();
	}
	
	return 0;
}

void UUW_PlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	TryCachePlayerRef();
	TryCacheEnemyBaseRef();

	HealthProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerHealthPercentage");
	HealthProgressBar->SynchronizeProperties();

	StaminaProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerStaminaPercentage");
	StaminaProgressBar->SynchronizeProperties();

	RageGageProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerRagePercentage");
	RageGageProgressBar->SynchronizeProperties();
	
	RevengeProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerRevengePercentage");
	RevengeProgressBar->SynchronizeProperties();
	
	EnemyBossHealthProgressBar->PercentDelegate.BindUFunction(this, "GetEnemyHealthPercentage");
	EnemyBossHealthProgressBar->SynchronizeProperties();
}
