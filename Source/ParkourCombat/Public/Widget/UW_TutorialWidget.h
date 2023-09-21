// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UW_WidgetWithInputDetection.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UW_TutorialWidget.generated.h"

/**
 * 
 */

struct TutorialPageData
{
protected:
	UTexture2D* PageImageTexture;
	FString TutorialSectionName;
	FString TutorialDescription;
	UTexture2D* ButtonPressTexture_Gamepad;
	UTexture2D* ButtonPressTexture_Keyboard;
	FString ButtonPressText;

public:

	UTexture2D* GetPageImageTexture() const {return PageImageTexture;}
	UTexture2D* GetButtonPressTexture_Gamepad() const {return ButtonPressTexture_Gamepad;}
	UTexture2D* GetButtonPressTexture_Keyboard() const {return ButtonPressTexture_Keyboard;}
	FString GetTutorialSectionName() const {return TutorialSectionName;}
	FString GetTutorialDescription() const {return TutorialDescription;}
	FString GetButtonPressText() const {return ButtonPressText;}
	
	
	TutorialPageData() {PageImageTexture = nullptr; TutorialSectionName = ""; TutorialDescription = ""; ButtonPressTexture_Gamepad = nullptr; ButtonPressTexture_Keyboard = nullptr; ButtonPressText = "";}
	TutorialPageData(UTexture2D* PageTexture, FString SectionName, FString Description, UTexture2D* ButtonImageTextureGamepad, UTexture2D* ButtonImageTextureKeyboard, FString ButtonText)
	{
		PageImageTexture = PageTexture;
		TutorialSectionName = SectionName;
		TutorialDescription = Description;
		ButtonPressTexture_Gamepad = ButtonImageTextureGamepad;
		ButtonPressTexture_Keyboard = ButtonImageTextureKeyboard;
		ButtonPressText = ButtonText;
	}
	
};

UENUM(BlueprintType)
enum class TutorialUnLockAbility: uint8
{
	RangeAttack = 0 UMETA(DisplayName = "RangeAttack"),
	RageAttack = 1 UMETA(DisplayName = "RageAttack"),
	Dodge = 2 UMETA(DisplayName = "Dodge"),
	Guard = 3 UMETA(DisplayName = "Guard"),
	Heal = 4 UMETA(DisplayName = "Heal"),
	Rewind = 5 UMETA(DisplayName = "Rewind"),
};

// FOnCheckIfPauseMenuIsClosedSignature
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FOnCheckPauseMenuIsClosed);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTryToUnlockAbility, TutorialUnLockAbility, UnLockAbility);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPlayerHUDShowOrHide, bool, bVisible);
DECLARE_DYNAMIC_DELEGATE(FOnExitTutorial);



UCLASS()
class PARKOURCOMBAT_API UUW_TutorialWidget : public UUW_WidgetWithInputDetection
{
	GENERATED_BODY()

private:

	int32 CurrentTutorialPageIndex = 0;
	int32 UnlockedPageNum = 0;
	bool IsWidgetInitialized = false;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	TArray<UTexture2D*> TutorialImages;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	TArray<FString> SectionNames;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	TArray<FString> Descriptions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	TArray<UTexture2D*> ButtonImages_Gamepad;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	TArray<UTexture2D*> ButtonImages_Keyboard;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Setting)
	TArray<FString> ButtonTexts;
	
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* NextPageButton;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* PreviousPageButton;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* ExitTutorialMenuButton;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* DisplayImage;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayDescription;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplaySectionName;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* DisplayButtonImg_Gamepad;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* DisplayButtonImg_Keyboard;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* DisplayButtonText;

	

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* NextAndPreviousKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush NextAndPreviousKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush NextAndPreviousKeyImage_Keyboard;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UImage* CancelKeyImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Gamepad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush CancelKeyImage_Keyboard;

	
	
	TArray<TutorialPageData> PageDataList;

	

	void InitializeTutorialData();
	
	void BindButtonFunctions();
	
	virtual void NativeConstruct() override;

	

	UFUNCTION()
	void OnNextTutorial();
	
	UFUNCTION()
	void OnPreviousTutorial();

	UFUNCTION()
	void ExitTutorial();

	void UpdateTutorialPage();

public:

	virtual void OnUpdateControlUI(ControlType NewControlType) override;
	virtual void OnReceiveMenuInputKey(PlayerMenuInputType ReceiveKey) override;
	
	FOnCheckPauseMenuIsClosed OnCheckPauseMenuIsCloseDelegate;
	FOnTryToUnlockAbility OnTryToUnlockAbilityDelegate;
	FOnPlayerHUDShowOrHide FOnPlayerHUDShowOrHideDelegate;
	FOnExitTutorial OnExitTutorialDelegate;
	
	void FlipToNewestPage();
	void UnlockedPageNumIncrement(const int32 UnlockPageCount);
};
