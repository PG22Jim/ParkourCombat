// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"



class ParkourPositionData
{
public:

	FTransform TransformData;

	ParkourPositionData* NextTransform;
	ParkourPositionData* PreviousTransform;
	
	ParkourPositionData(const FTransform StoringTransform);
	ParkourPositionData(const FTransform StoringTransform, ParkourPositionData* StoringNextTransform);
		
	
};



/**
 * 
 */
class PARKOURCOMBAT_API ParkourMovementLinkedList
{
public:
	ParkourMovementLinkedList();
	~ParkourMovementLinkedList();
	
	void AddAtTail(const FTransform DataToAdd);
	void AddAtHead(const FTransform DataToAdd);
	
	ParkourPositionData* GetHead() const {return HeadData;}
	ParkourPositionData* GetTail() const {return TailData;}
	int32 GetSizeNum() const {return ListSize;}

	ParkourPositionData* GetAt(const int32 IndexToGet);

	void RemoveTailData();
	
	void ClearList();

	

private:

	ParkourPositionData* HeadData;
	ParkourPositionData* TailData;

	int32 ListSize;
	
};


class PARKOURCOMBAT_API BackTrackData_Stack
{
public:
	BackTrackData_Stack();
	~BackTrackData_Stack();
	
	ParkourPositionData* Peek() const {return TopData;}
	int32 GetSizeNum() const {return ListSize;}

	bool IsEmpty() const {return TopData == nullptr;} 
	
	void Push(const FTransform DataToAdd);
	
	void Pop();
	void RemoveTailData();

	void ClearList();

	

private:

	ParkourPositionData* TopData = nullptr;
	ParkourPositionData* TailData = nullptr;


	int32 ListSize;

	int32 MaxListSize = 700;
	
};
