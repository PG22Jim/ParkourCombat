// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Widget/UW_BasicMenu.h"

WidgetButtonNode::WidgetButtonNode()
{
	WidgetButton = nullptr;
	NextNode = nullptr;
	PreviousNode = nullptr;	
}

WidgetButtonNode::WidgetButtonNode(UButton* Button)
{
	WidgetButton = Button;
	NextNode = nullptr;
	PreviousNode = nullptr;	
}

WidgetMenuManager::WidgetMenuManager()
{
	HeadMenu = nullptr;
	TailMenu = nullptr;
	SelectedMenu = nullptr;
	ListSize = 0;
}

void WidgetMenuManager::AddAtTail(UButton* Button)
{
	//If Head pointer is nullptr
	if (!HeadMenu)
	{
		// Create new data to add at both head and tail
		WidgetButtonNode* NewMenu = new WidgetButtonNode(Button);
		HeadMenu = NewMenu;
		TailMenu = NewMenu;
		ListSize++;
		return;
	}

	// if list only contains head
	if(ListSize == 1)
	{
		// Create new data to add at tail
		WidgetButtonNode* NewMenu = new WidgetButtonNode(Button);
		HeadMenu->NextNode = NewMenu;
		HeadMenu->PreviousNode = NewMenu;
		NewMenu->NextNode = HeadMenu;
		NewMenu->PreviousNode = HeadMenu;
		TailMenu = NewMenu;
		ListSize++;
		return;
	}

	// if list has more than one node

	// Create new data to add at tail
	WidgetButtonNode* NewMenu = new WidgetButtonNode(Button);
	TailMenu->NextNode = NewMenu;
	NewMenu->PreviousNode = TailMenu;
	HeadMenu->PreviousNode = NewMenu;
	NewMenu->NextNode = HeadMenu;
	TailMenu = NewMenu;
	
	ListSize++;
}

WidgetButtonNode* WidgetMenuManager::GetNodeInList(UButton* RequestButton)
{
	WidgetButtonNode* IterationNode = HeadMenu;
	int32 IterationNum = 0;

	while (IterationNum <= ListSize)
	{
		if(IterationNode->WidgetButton == RequestButton) return IterationNode;

		IterationNum++;
		IterationNode = IterationNode->NextNode;
	}

	return nullptr;
}

void WidgetMenuManager::ClearList()
{	// If head is null pointer
	if (!HeadMenu)
	{
		//no need to clear
		return;
	}

	// while loop until Head become null pointer
	while (HeadMenu)
	{
		// delete tail if Head data is still valid
		RemoveTailData();
	}

	// Set LinkedListSize as 0
	ListSize = 0;
}

void WidgetMenuManager::RemoveTailData()
{
	// insure tail data is valid
	if(!TailMenu) return;

	// set removing tail data's previous data as tail data if previous data is valid
	WidgetButtonNode* CurrentTailData = GetTail();
	WidgetButtonNode* TailPreviousData = CurrentTailData->PreviousNode;

	if(TailPreviousData)
		TailMenu = TailPreviousData;
	else
		HeadMenu = nullptr;
		
	// remove tail data
	CurrentTailData = nullptr;
}

void WidgetMenuManager::UnSelectCurrentSelectedMenuNode()
{
	FString ButtonName = SelectedMenu->WidgetButton->GetName();
	SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::White);
}

void WidgetMenuManager::OnSelectRequestMenuNode(WidgetButtonNode* RequestMenuNode)
{
	FString ButtonName = RequestMenuNode->WidgetButton->GetName();
	RequestMenuNode->WidgetButton->SetBackgroundColor(FLinearColor::Green);
}

void WidgetMenuManager::OnExecuteRequestMenuNode()
{
	FString ButtonName = SelectedMenu->WidgetButton->GetName();
	SelectedMenu->WidgetButton->SetBackgroundColor(FLinearColor::Red);

	SelectedMenu->ButtonFunctionDelegate.ExecuteIfBound();
}

void WidgetMenuManager::ExecuteButton(WidgetButtonNode* RequestMenuNode)
{
	FString ButtonName = RequestMenuNode->WidgetButton->GetName();
	RequestMenuNode->ButtonFunctionDelegate.ExecuteIfBound();
}

void WidgetMenuManager::OnExecuteButton(UButton* PressedButton)
{
	WidgetButtonNode* ReturnNode = GetNodeInList(PressedButton);
	if(!ReturnNode) return;
	ExecuteButton(ReturnNode);
}

void WidgetMenuManager::OnSelectNextMenuNode()
{
	UnSelectCurrentSelectedMenuNode();
	SelectedMenu = SelectedMenu->NextNode;
	OnSelectRequestMenuNode(SelectedMenu);
}

void WidgetMenuManager::OnSelectPreviousMenuNode()
{
	UnSelectCurrentSelectedMenuNode();
	SelectedMenu = SelectedMenu->PreviousNode;
	OnSelectRequestMenuNode(SelectedMenu);
}

void WidgetMenuManager::SelectMenuNodeByHovering(WidgetButtonNode* HoveredMenu)
{
	UnSelectCurrentSelectedMenuNode();
	SelectedMenu = HoveredMenu;
	OnSelectRequestMenuNode(HoveredMenu);
}

void WidgetMenuManager::OnHoverButton(UButton* HoveredButton)
{
	WidgetButtonNode* ReturnNode = GetNodeInList(HoveredButton);
	if(!ReturnNode || ReturnNode == SelectedMenu) return;
	SelectMenuNodeByHovering(ReturnNode);
}




void UUW_BasicMenu::NativeConstruct()
{
	MenuManager = new WidgetMenuManager();
}

void UUW_BasicMenu::SelectNextMenuNode()
{
	if(MenuManager) MenuManager->OnSelectNextMenuNode();
}

void UUW_BasicMenu::SelectPreviousMenuNode()
{
	if(MenuManager) MenuManager->OnSelectPreviousMenuNode();
}

void UUW_BasicMenu::ExecuteSelectedMenuNode()
{
	if(MenuManager) MenuManager->OnExecuteRequestMenuNode();
}
