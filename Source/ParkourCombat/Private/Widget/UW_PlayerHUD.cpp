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

bool UUW_PlayerHUD::GetPlayerStaminaUnlock()
{
	if(TryCachePlayerRef())
	{
		return PlayerRef->GetEnableDodge() || PlayerRef->GetEnableGuard();
	}
	
	return false;
}

bool UUW_PlayerHUD::GetPlayerRageGaugeUnlock()
{
	if(TryCachePlayerRef())
	{
		const bool bUnlock = PlayerRef->GetEnableRageAttack();
		return bUnlock;
	}
	
	return false;
}

bool UUW_PlayerHUD::GetPlayerRevengeGaugeUnlock()
{
	if(TryCachePlayerRef())
	{
		return PlayerRef->GetEnableGuard();
	}
	
	return false;
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

FText UUW_PlayerHUD::GetRewindAvailableTimeText()
{
	if(TryCachePlayerRef())
	{
		const int32 RewindAvailableTime = PlayerRef->GetRewindAvailableTime(); 
		return FText::FromString(FString::FromInt(RewindAvailableTime));
	}
	
	return FText::FromString("NULL");
}

float UUW_PlayerHUD::GetRewindAvailableTimePercentage()
{
	if(TryCachePlayerRef())
	{
		return static_cast<float>(PlayerRef->GetRewindAvailableTime()) / static_cast<float>(PlayerRef->GetMaxRewindAvailableTime());
	}
	
	return 0;
}

float UUW_PlayerHUD::GetHealthPotionPercentage()
{
	if(TryCachePlayerRef())
	{
		return static_cast<float>(PlayerRef->GetAvailableHealTime()) / 4.0f;
	}
	
	return 0;
}

FText UUW_PlayerHUD::GetHealthPotionAvailableTimeText()
{
	if(TryCachePlayerRef())
	{
		const int32 HealthPotionAvailableTime = PlayerRef->GetAvailableHealTime(); 
		return FText::FromString(FString::FromInt(HealthPotionAvailableTime));
	}
	
	return FText::FromString("NULL");
	
}

void UUW_PlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	if(!IsWidgetInitialized)
	{
		TryCachePlayerRef();
		TryCacheEnemyBaseRef();

		HealthProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerHealthPercentage");
		HealthProgressBar->SynchronizeProperties();

		StaminaProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerStaminaPercentage");
		//StaminaProgressBar->VisibilityDelegate.BindUFunction(this, "GetPlayerStaminaUnlock");
		StaminaProgressBar->SynchronizeProperties();
		
		//StaminaIcon->VisibilityDelegate.BindUFunction(this, "GetPlayerStaminaUnlock");
		//StaminaIcon->SynchronizeProperties();
		

		RageGageProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerRagePercentage");
		//RageGageProgressBar->VisibilityDelegate.BindUFunction(this, "GetPlayerRageGaugeUnlock");
		RageGageProgressBar->SynchronizeProperties();
		
		//RageGaugeIcon->VisibilityDelegate.BindUFunction(this, "GetPlayerRageGaugeUnlock");
		//RageGaugeIcon->SynchronizeProperties();
		
		
		RevengeProgressBar->PercentDelegate.BindUFunction(this, "GetPlayerRevengePercentage");
		//RevengeProgressBar->VisibilityDelegate.BindUFunction(this, "GetPlayerRevengeGaugeUnlock");
		RevengeProgressBar->SynchronizeProperties();


		RewindProgressBar->PercentDelegate.BindUFunction(this, "GetRewindAvailableTimePercentage");
		RewindProgressBar->SynchronizeProperties();

		RewindAvailableTimeTextBlock->TextDelegate.BindUFunction(this, "GetRewindAvailableTimeText");
		RewindAvailableTimeTextBlock->SynchronizeProperties();

		
		HealthPotionProgressBar->PercentDelegate.BindUFunction(this, "GetHealthPotionPercentage");
		HealthPotionProgressBar->SynchronizeProperties();

		HealthPotionAvailableTimeTextBlock->TextDelegate.BindUFunction(this, "GetHealthPotionAvailableTimeText");
		HealthPotionAvailableTimeTextBlock->SynchronizeProperties();
		
		
		EnemyBossHealthProgressBar->PercentDelegate.BindUFunction(this, "GetEnemyHealthPercentage");
		EnemyBossHealthProgressBar->SynchronizeProperties();

		IsWidgetInitialized = true;
	}
}

void UUW_PlayerHUD::OnUpdateControlUI(ControlType NewControlType)
{
	Super::OnUpdateControlUI(NewControlType);

	if(NewControlType == ControlType::Gamepad)
	{
		RageAttackUseIcon->SetBrush(RageAttackUseIcon_Gamepad);
		RewindAbilityUseIcon->SetBrush(RewindAbilityUseIcon_Gamepad);
		HealthPotionUseIcon->SetBrush(HealthPotionUseIcon_Gamepad);
	}
	else
	{
		RageAttackUseIcon->SetBrush(RageAttackUseIcon_Keyboard);
		RewindAbilityUseIcon->SetBrush(RewindAbilityUseIcon_Keyboard);
		HealthPotionUseIcon->SetBrush(HealthPotionUseIcon_Keyboard);
	}

}

void UUW_PlayerHUD::UnlockOrLockDodge(bool bEnable)
{
	if(TryCachePlayerRef())
	{
		PlayerRef->SetEnableDodge(bEnable);
		ShowOrHideStaminaBar(bEnable);
	}
}

void UUW_PlayerHUD::UnlockOrLockGuard(bool bEnable)
{
	if(TryCachePlayerRef())
	{
		PlayerRef->SetEnableGuard(bEnable);
		ShowOrHideStaminaBar(bEnable);
		ShowOrHideRevengeGaugeBar(bEnable);
	}
}

void UUW_PlayerHUD::UnlockOrLockRangeAttack(bool bEnable)
{
	if(TryCachePlayerRef())
	{
		PlayerRef->SetEnableRangeAttack(bEnable);
	}
}

void UUW_PlayerHUD::UnlockOrLockRageAttack(bool bEnable)
{
	if(TryCachePlayerRef())
	{
		PlayerRef->SetEnableRageAttack(bEnable);
		ShowOrHideRageGaugeBar(bEnable);
	}
}

void UUW_PlayerHUD::UnlockOrLockHealItem(bool bEnable)
{
	if(TryCachePlayerRef())
	{
		PlayerRef->SetEnableHealItem(bEnable);
		ShowOrHideHealthItemIcon(bEnable);
	}
}

void UUW_PlayerHUD::UnlockOrLockRewindAbility(bool bEnable)
{
	if(TryCachePlayerRef())
	{
		PlayerRef->SetEnableRewindAbility(bEnable);
		ShowOrHideRewindAbilityIcon(bEnable);
	}
}

void UUW_PlayerHUD::ShowOrHideStaminaBar(bool bShow)
{
	if(bShow)
	{
		StaminaIcon->SetVisibility(ESlateVisibility::Visible);
		StaminaProgressBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		StaminaIcon->SetVisibility(ESlateVisibility::Hidden);
		StaminaProgressBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUW_PlayerHUD::ShowOrHideRageGaugeBar(bool bShow)
{
	if(bShow)
	{
		RageGaugeIcon->SetVisibility(ESlateVisibility::Visible);
		RageGageProgressBar->SetVisibility(ESlateVisibility::Visible);
		RageAttackUseIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		RageGaugeIcon->SetVisibility(ESlateVisibility::Hidden);
		RageGageProgressBar->SetVisibility(ESlateVisibility::Hidden);
		RageAttackUseIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUW_PlayerHUD::ShowOrHideRevengeGaugeBar(bool bShow)
{
	if(bShow)
	{
		RevengeProgressBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		RevengeProgressBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUW_PlayerHUD::ShowOrHideHealthItemIcon(bool bShow)
{
	if(bShow)
	{
		HealthPotionAvailableTimeTextBlock->SetVisibility(ESlateVisibility::Visible);
		HealthPotionProgressBar->SetVisibility(ESlateVisibility::Visible);
		HealthPotionUseIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HealthPotionAvailableTimeTextBlock->SetVisibility(ESlateVisibility::Hidden);
		HealthPotionProgressBar->SetVisibility(ESlateVisibility::Hidden);
		HealthPotionUseIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUW_PlayerHUD::ShowOrHideRewindAbilityIcon(bool bShow)
{
	if(bShow)
	{
		RewindProgressBar->SetVisibility(ESlateVisibility::Visible);
		RewindAvailableTimeTextBlock->SetVisibility(ESlateVisibility::Visible);
		RewindAbilityUseIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		RewindProgressBar->SetVisibility(ESlateVisibility::Hidden);
		RewindAvailableTimeTextBlock->SetVisibility(ESlateVisibility::Hidden);
		RewindAbilityUseIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUW_PlayerHUD::ShowOrHideEnemyBossHealthBar(bool bShow)
{
	if(bShow)
	{
		EnemyBossHealthProgressBar->SetVisibility(ESlateVisibility::Visible);
		EnemyBossText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		EnemyBossHealthProgressBar->SetVisibility(ESlateVisibility::Hidden);
		EnemyBossText->SetVisibility(ESlateVisibility::Hidden);
	}
}
