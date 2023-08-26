// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "ParkourCombat/ParkourCombatCharacter.h"
#include "UW_PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UUW_PlayerHUD : public UUserWidget
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
	UProgressBar* HealthProgressBar;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* RevengeProgressBar;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UProgressBar* RageGageProgressBar;

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
	float GetPlayerRagePercentage();
	
	UFUNCTION()
	float GetPlayerRevengePercentage();
	

	virtual void NativeConstruct() override;
	
};
