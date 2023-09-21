// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UW_WidgetWithInputDetection.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "UW_PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UUW_PlayerHUD : public UUW_WidgetWithInputDetection
{
	GENERATED_BODY()

private:
	AParkourCombatCharacter* PlayerRef;

	ABaseCharacter* EnemyBaseRef;

	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABaseCharacter> BaseCharacterClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName EnemyBossTag = "Boss";
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UCanvasPanel* HUDCanvas;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* HealthIcon;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* HealthProgressBar;

	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* RevengeProgressBar;

	
	
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* StaminaIcon;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* StaminaProgressBar;

	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* RageGaugeIcon;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* RageGageProgressBar;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* RageAttackUseIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush RageAttackUseIcon_Gamepad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush RageAttackUseIcon_Keyboard;

	

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* HealthPotionProgressBar;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* HealthPotionAvailableTimeTextBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* HealthPotionUseIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush HealthPotionUseIcon_Gamepad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush HealthPotionUseIcon_Keyboard;

	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* RewindProgressBar;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* RewindAvailableTimeTextBlock;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* RewindAbilityUseIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush RewindAbilityUseIcon_Gamepad;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush RewindAbilityUseIcon_Keyboard;
	

	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* EnemyBossText;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* EnemyBossHealthProgressBar;


	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool TryCachePlayerRef();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool TryCacheEnemyBaseRef();

	UFUNCTION()
	float GetEnemyHealthPercentage();
	
	UFUNCTION()
	float GetPlayerHealthPercentage();

	UFUNCTION()
	float GetPlayerStaminaPercentage();

	UFUNCTION()
	bool GetPlayerStaminaUnlock();
	
	UFUNCTION()
	bool GetPlayerRageGaugeUnlock();

	UFUNCTION()
	bool GetPlayerRevengeGaugeUnlock();
	
	UFUNCTION()
	float GetPlayerRagePercentage();
	
	UFUNCTION()
	float GetPlayerRevengePercentage();
	
	UFUNCTION()
	float GetRewindAvailableTimePercentage();

	UFUNCTION()
	FText GetRewindAvailableTimeText();
	
	UFUNCTION()
	float GetHealthPotionPercentage();

	UFUNCTION()
	FText GetHealthPotionAvailableTimeText();

	
	virtual void NativeConstruct() override;

	

	
public:

	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	
	bool IsWidgetInitialized = false;
	
	void UnlockOrLockDodge(bool bEnable);
	void UnlockOrLockGuard(bool bEnable);
	void UnlockOrLockRangeAttack(bool bEnable);
	void UnlockOrLockRageAttack(bool bEnable);
	void UnlockOrLockHealItem(bool bEnable);
	void UnlockOrLockRewindAbility(bool bEnable);

	void ShowOrHideStaminaBar(bool bShow);
	void ShowOrHideRageGaugeBar(bool bShow);
	void ShowOrHideRevengeGaugeBar(bool bShow);
	void ShowOrHideHealthItemIcon(bool bShow);
	void ShowOrHideRewindAbilityIcon(bool bShow);
	void ShowOrHideEnemyBossHealthBar(bool bShow);
	
};
