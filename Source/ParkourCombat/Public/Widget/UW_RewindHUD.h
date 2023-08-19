// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Slider.h"
#include "UW_RewindHUD.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UUW_RewindHUD : public UUserWidget
{
	GENERATED_BODY()
private:

	
protected:

	virtual void NativeConstruct() override;
	virtual bool Initialize() override;


public:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	USlider* RewindSlider;

	


	
};
