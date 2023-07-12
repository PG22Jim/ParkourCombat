// Copyright © 2023 by Jim Chen. All rights reserved.


#include "ParkourMovementLinkedList.h"

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform)
{
	TransformData = StoringTransform;
	NextTransform = nullptr;
	PreviousTransform = nullptr;
	CurrentMontage = nullptr;
	CurrentParkourStatus = ParkourStatus::Idle;
	CurrentAnimationTime = 0.0f;
}

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform, ParkourPositionData* StoringNextTransform)
{
	TransformData = StoringTransform;
	NextTransform = StoringNextTransform;
	PreviousTransform = nullptr;
	CurrentMontage = nullptr;
	CurrentParkourStatus = ParkourStatus::Idle;
	CurrentAnimationTime = 0.0f;
}

ParkourPositionData::ParkourPositionData(const FTransform StoringTransform,
	ParkourStatus CurrentStatus, UAnimMontage* PlayingMontage, float StoringAnimationTime)
{
	TransformData = StoringTransform;
	NextTransform = nullptr;
	PreviousTransform = nullptr;
	CurrentMontage = PlayingMontage;
	CurrentParkourStatus = CurrentStatus;
	CurrentAnimationTime = StoringAnimationTime;
	
}


ParkourMovementLinkedList::ParkourMovementLinkedList()
{
	HeadData = nullptr;
	TailData = nullptr;

	ListSize = 0;
}

ParkourMovementLinkedList::~ParkourMovementLinkedList()
{
	ClearList();
}

void ParkourMovementLinkedList::AddAtTail(const FTransform DataToAdd)
{
	//If Head pointer is nullptr
	if (!HeadData)
	{
		// Create new data to add at both head and tail
		ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
		HeadData = NewData;
		TailData = NewData;
		ListSize++;
		return;
	}

	//If Linklist already has Head Data
	ParkourPositionData* IteratingData = HeadData;

	// While loop until CurrentNode is Tail
	while(IteratingData->NextTransform)
	{	
		//Setting up CurrentNode as NewData for next while loop check
		IteratingData = IteratingData->NextTransform;
	}

	// Create new data to add at tail
	ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);

	// Current Tail's NextTransform become NewData
	IteratingData->NextTransform = NewData;
	NewData->PreviousTransform = IteratingData;
	
	// NewData become Tail
	TailData = NewData;

	ListSize++;
	
}

void ParkourMovementLinkedList::AddAtHead(const FTransform DataToAdd)
{
	//If Head pointer is nullptr
	if (!HeadData)
	{
		// Create new data to add at both head and tail
		ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
		HeadData = NewData;
		TailData = NewData;
		ListSize++;
		return;
	}

	// If list only has head
	if(ListSize == 1)
	{
		ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
		HeadData = NewData;
		HeadData->NextTransform = TailData;
		TailData->PreviousTransform = HeadData;
		ListSize++;
		return;
	}
	
	//If Linklist already has Head Data and list is bigger than 1
	ParkourPositionData* NewData = new ParkourPositionData(DataToAdd);
	NewData->NextTransform = HeadData;
	HeadData->PreviousTransform = NewData;
	HeadData = NewData;

	ListSize++;
}

ParkourPositionData* ParkourMovementLinkedList::GetAt(const int32 IndexToGet)
{
	// If head data is null pointer
	if (!HeadData)
		// It means no data in list, just return
		return nullptr;
	
	
	// Head is not null pointer
	ParkourPositionData* CurrentNode = HeadData;

	//If The Index is 0
	if (IndexToGet == 0)
	{
		// 0 is the first one in LinkedList
		return HeadData;
	}

	//If the user inputs nIndex is less than 0 or greater than LinkedList Size
	if (IndexToGet < 0 || IndexToGet > ListSize)
	{
		return nullptr;
	}

	//If the user inputs nIndex is between 0 and Linkedlist size
	int CurrentIndexCounter = 0;
	// While loop until reaching Index number
	while (CurrentIndexCounter != IndexToGet)
	{
		//setting up CurrentNode next node in the list
		CurrentNode = CurrentNode->NextTransform;
		CurrentIndexCounter++;
	}
	return CurrentNode;
}

void ParkourMovementLinkedList::RemoveTailData()
{
	// insure tail data is valid
	if(!TailData) return;

	// set removing tail data's previous data as tail data if previous data is valid
	ParkourPositionData* CurrentTailData = GetTail();
	ParkourPositionData* TailPreviousData = CurrentTailData->PreviousTransform;

	if(TailPreviousData)
		TailData = TailPreviousData;
	else
		HeadData = nullptr;
		
	// remove tail data
	CurrentTailData = nullptr;
}

void ParkourMovementLinkedList::ClearList()
{
	// If head is null pointer
	if (!HeadData)
	{
		//no need to clear
		return;
	}

	// while loop until Head become null pointer
	while (HeadData)
	{
		// delete tail if Head data is still valid
		RemoveTailData();
	}

	// Set LinkedListSize as 0
	ListSize = 0;
}

// ==================================================== Stack ==========================================================



BackTrackData_Stack::BackTrackData_Stack()
{
	TopData = nullptr;
	TailData = nullptr;
	ListSize = 0;
}

BackTrackData_Stack::~BackTrackData_Stack()
{
	ClearList();
}

void BackTrackData_Stack::Push(const FTransform DataToAdd, const ParkourStatus StoringParkourStatus, UAnimMontage* StoringAnimation ,const float StoringAnimationTime)
{
	// if is empty, create new node and assign it as Top
	if(IsEmpty())
	{
		TopData = new ParkourPositionData(DataToAdd, StoringParkourStatus, StoringAnimation, StoringAnimationTime);
		TailData = TopData;
		
		return;
	}

	//if not empty, create new Top
	ParkourPositionData* NewData = new ParkourPositionData(DataToAdd, StoringParkourStatus, StoringAnimation, StoringAnimationTime);
	
	NewData->NextTransform = TopData;
	TopData->PreviousTransform = NewData;
	TopData = NewData;

	ListSize++;
	if(ListSize > MaxListSize)
	{
		Pop();
	}
}

void BackTrackData_Stack::Pop()
{
	// if is empty, return
	if(IsEmpty())
	{
		return;
	}

	ParkourPositionData* RemovingData = TopData;
	TopData = RemovingData->NextTransform;
	RemovingData = nullptr;
	
	ListSize--;
}

void BackTrackData_Stack::RemoveTailData()
{
	// insure tail data is valid
	if(!TailData) return;

	
	// set removing tail data's previous data as tail data if previous data is valid
	ParkourPositionData* CurrentTailData = TailData;
	ParkourPositionData* TailPreviousData = CurrentTailData->PreviousTransform;

	if(TailPreviousData)
		TailData = TailPreviousData;
	else
		TopData = nullptr;
		
	// remove tail data
	CurrentTailData = nullptr;
}

void BackTrackData_Stack::ClearList()
{
	// If head is null pointer
	if (!TopData)
	{
		//no need to clear
		return;
	}

	// while loop until Head become null pointer
	while (TopData)
	{
		// delete tail if Head data is still valid
		Pop();
	}

	// Set LinkedListSize as 0
	ListSize = 0;
	
}
