// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "UW_BasicMenu.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE(FOnExecuteButtonFunction);



class WidgetButtonNode
{
public:

	UButton* WidgetButton = nullptr;
	WidgetButtonNode* NextNode = nullptr;
	WidgetButtonNode* PreviousNode = nullptr;
	WidgetButtonNode();
	WidgetButtonNode(UButton* Button);

	void BindButtonFunction(UObject* BindingObject, FName FunctionName) { ButtonFunctionDelegate.BindUFunction(BindingObject, FunctionName); }
	FOnExecuteButtonFunction ButtonFunctionDelegate;
	
};

class WidgetMenuManager
{
protected:

	
	
	
public:
	WidgetMenuManager();
	~WidgetMenuManager() {ClearList();};

	void AddAtTail(UButton* Button);
	int32 GetListSize() const {return ListSize;}
	WidgetButtonNode* GetTail() const {return TailMenu;}
	WidgetButtonNode* GetNodeInList(UButton* RequestButton);
	void ClearList();
	void RemoveTailData();
	
	void UnSelectCurrentSelectedMenuNode();
	void OnSelectRequestMenuNode(WidgetButtonNode* RequestMenuNode);

	void OnExecuteRequestMenuNode();
	void ExecuteButton(WidgetButtonNode* RequestMenuNode);
	void OnExecuteButton(UButton* PressedButton);
	
	void OnSelectNextMenuNode();
	void OnSelectPreviousMenuNode();
	void SelectMenuNodeByHovering(WidgetButtonNode* HoveredMenu);
	void OnHoverButton(UButton* HoveredButton);

	
	WidgetButtonNode* HeadMenu = nullptr;
	WidgetButtonNode* TailMenu = nullptr;
	WidgetButtonNode* SelectedMenu = nullptr;
	int32 ListSize = 0;
};

UCLASS()
class PARKOURCOMBAT_API UUW_BasicMenu : public UUserWidget
{
	GENERATED_BODY()
protected:

	WidgetMenuManager* MenuManager = nullptr;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SelectNextMenuNode();
	
	UFUNCTION(BlueprintCallable)
	void SelectPreviousMenuNode();

	UFUNCTION(BlueprintCallable)
	void ExecuteSelectedMenuNode();
};
